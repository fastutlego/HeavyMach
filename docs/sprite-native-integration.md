> 과거 구현·검사 기록이다. 현행 출력 경로는 [그래픽 정책](rendering-policy.md)과 [시간·저장·음향 정책](runtime-policy.md)을 따른다. 이 문서의 UIKit/AVAudioPlayer 설명을 새 작업의 구현 기준으로 사용하지 않는다.

# 원본 렌더 규칙의 iOS 연결

2026-10-24. 전체 텍스처 보간과 원본 blend 규칙을 `SpriteRaster`/`TrailRaster` → `HMMedia` → 480×320 논리 프레임에 연결했다. 코어 CTest 14개, 실제 Air 시뮬레이터의 UIKit 단위 검사 4개, 일반 iPhone용 서명 제외 빌드가 통과했다. 이번 변경을 물리 iPhone에 추가 설치하지 않았다.

## 원본 근거와 수정

원본 초기화 `0x2120`은 `GL_REPLACE`를 설정한다. RGBA 텍스처의 vertex 색과 alpha는 최종 fragment에 반영되지 않는다. RGB565 텍스처는 vertex alpha를 사용한다. 앞선 GL_MODULATE 가정을 수정했다. [원본 주소와 샘플링 계약](sprite-raster.md), [독립 OpenGL 출력](sprite-gl-reference.md)에 근거를 보존했다.

기존 tile별 crop과 alpha 선처리를 제거했다. 전체 텍스처의 인접 tile과 POT 패딩을 먼저 보간하고 그 뒤 source alpha를 곱한다. GL_REPEAT, RGB565의 5/6-bit 정규화, 분수 atlas 크기, 회전·반전과 실제 GL 경계 포함 규칙을 적용한다. Tile blend3은 normal, Trail blend3은 additive다.

Trail도 같은 sampler를 사용한다. `GameSession`은 실제 이미지 크기로 maxS/maxT를 계산한다. 예를 들어 40×64 LaserTrail은 64×64 POT 텍스처에서 maxS=.625, maxT=1이다. 기존 content 끝을 UV1로 지정하던 차이를 수정했다. RGBA vertex alpha를 무시하는 렌더 규칙과 별개로 Trail의 alpha 감쇠·제거·RNG 소비 규칙은 유지한다.

`HMMedia`는 전체 디코드 이미지를 캐시하고 camera offset을 래스터화 전에 뺀다. 출력 행 방향을 변환한 뒤 UIKit에 1:1로 합성한다. `HMGameView`는 scale1의 불투명 480×320 프레임을 먼저 완성하고 Air viewport에 한 번 확대한다. sprite마다 먼저 확대하면 보간·합성 순서가 달라지므로 이 순서를 고정했다. 논리 화면 전체를 safe area에 맞추는 기존 viewport를 사용한다.

## 실행 증거

| 검사 | 결과 | 증거 |
|---|---|---|
| 새 Trail 회귀, 수정 전 | Core/Session assertion 실패, 각각 exit134 | `build/trail-original-state-before-{core,session}.log` |
| 수정 후 Debug CTest | 14/14 통과 | `build/sprite-raster-optimized-ctest.log` |
| Core/Session ASan·UBSan | 통과 | `build/sprite-raster-asan-{core,session}.log` |
| Sprite 대 실제 CGL | 7개 fixture의 전체 480×320 RGB 최대 차이 0~1 byte, 1 초과 0채널 | `build/render-reference/sprite-cpu-comparison.log` |
| Trail 대 실제 CGL | fixture 전체 RGB 차이 0 | `build/render-reference/trail-compare-final.txt` |
| 실제 UIKit 합성 | normal, additive, 분수 camera offset, RGBA alpha0의 4개 검사 통과 | `build/sprite-raster-uikit-tests-retry.log`, `build/render-reference/UIKitTests.xcresult` |
| 일반 iphoneos 빌드 | BUILD SUCCEEDED, 서명 제외 | `build/sprite-raster-device-optimized-build.log` |

UIKit 검사는 테스트용 2×1 텍스처를 실제 `HMMedia`에 공급하고 offscreen UIGraphics 렌더 결과를 독립 CGL의 probe/경계 픽셀과 비교한다. 게임 화면 캡처 또는 직접 플레이 검사가 아니다. RGB를 비교하며, opaque UIKit 프레임 alpha255와 원본 GL framebuffer alpha를 같은 값으로 요구하지 않는다. 첫 UIKit 빌드의 framework 링크 실패는 `build/sprite-raster-uikit-tests.log`에 남겼고 의존성을 추가한 뒤 재검사했다.

소스 해시와 검사 경로는 `build/render-reference/integration-evidence.json`에 있다. 해당 렌더 통합 검사 당시의 소스 해시다. 후속 음원 변경의 HMMedia 해시와 최신 iOS7개 재검사는 `build/audio-preload-evidence.json`을 따른다. 이전 native 구현은 `build/sprite-native-before/`에 보존했다.

## 성능과 남은 범위

[Mac 측정](render-performance.md)에서 최적화 없는 렌더는 게임 표본 평균 54.18~74.36ms였고 O2는 4.00~5.59ms였다. 이 실패를 근거로 CMake와 두 Xcode 앱에서 SpriteRaster.cpp/TrailRaster.cpp에만 `-O2`를 적용했다. 게임 코어의 Debug assertion과 기존 부동소수점 설정은 유지했다. 적용 후 위 CTest·UIKit 검사·iphoneos 빌드를 통과했다.

Mac 측정은 iPhone FPS를 증명하지 않는다. 최신 앱 화면의 직접 비교, 실기기 성능·발열, 전체 게임 영상·음향의 원본 일치는 남아 있다. SRC_COLOR blend1과 이전 GL 상태를 상속하는 mode는 범용 API에서 지원하지 않는다. 다만 [원본 호출·데이터 조사](blend-reachability.md)에서 실제 생성·설정·draw 값은0/2였고 미지원 mode의 활성 경로는 찾지 못했다. 현재 원본 게임의 기능 누락으로 단정하지 않는다. 현재 695개 자산에는 1024 초과 이미지가 없으며 그 크기의 원본 downscale도 구현 범위 밖이다. 통과한 fixture를 전체 장면 동등성으로 확대하지 않는다.

## 실제 게임 이미지 후속 비교

작은 fixture의0~1byte 결과를 모든 자산에 일반화하지 않는다. [Stage01/28/35의 고정 draw list 비교](scene-render-comparison.md)에서 전체 프레임 최대 RGB 차이는2/2/3, 차이가1을 넘는 채널은11/41/86이었다. 156개 개별 draw의 CPU 출력→CG black 합성 차이는0이었다. 일부 단일 자산에서 이미GL대비2~3차이가 있어 단순 합성 누적만으로 설명할 수 없다. 첫 차이 발생 draw·픽셀·이전 destination·source contribution을 보존했다.

실제 앱과 같은 `-ffp-contract=off -fno-fast-math`로 별도 빌드·측정해도 이전 raw출력과 isolated/prefix 기록15파일이 byte-identical이었다. `build/render-reference/scenes-app-flags/execution.json`이 정확한 명령·해시 근거다. 측정 완료 exit0은 픽셀 동등성 통과가 아니다. 제품 코드·기준을 변경하지 않았고 원본 runtime의 draw list 순서까지 확인한 검사는 아니다.
