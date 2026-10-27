# HeavyMach REA 증거와 현재 구현 대조 — 2026-10-26

## 판정

현재 복원판에는 직접 컴파일되는 C++ 게임 로직과 Objective-C++ iOS 코드가 있다. 원작 실행 파일을 실행하는 코드로 게임 기능을 대신한 구조는 발견하지 않았다. Codex가 현재 파일의 구현과 REA 산출물, 실제 앱 호출 경로를 대조했다. IPA 없는 빌드·실행 결과는 [독립 빌드 검증](ipa-independent-build-20261026.md)에 있다.

REA 산출물은 문서 선언만 남아 있는 상태가 아니다. 색인 2,067개의 실제 파일과 SHA-256이 모두 맞았다. 25함수군의 현재 구현 파일·테스트 파일이 모두 존재했다. 대표 주소 70개 항목에 대한 의사코드·어셈블리 140개 Evidence에서 원본 SHA, 주소, 연산 종류, Ghidra 메타데이터와 읽기용 코드가 일치했다. 70은 매핑 항목 수이며 중복을 제거한 고유 함수 수로 표현하지 않는다.

검사한 제품 코드에서 구현을 비워 둔 stub이나 placeholder를 발견하지 않았다. 원작의 모든 Objective-C selector와 라이브러리·컴파일러 함수가 1:1로 새 소스에 옮겨졌다는 주장은 성립하지 않는다. 범용 노드·컨테이너·컴파일러·iOS 런타임은 현재 플랫폼과 게임별 구현으로 대체돼 있다. 이것을 미구현 게임 기능으로 세지 않았다.

## 범위와 원본

- 작업 저장소: `저장소 루트`
- 감사 시작 HEAD: `264a410e432adb4e7c3f1dc040fbfedbe1ffcfa6`
- 원작 실행 파일 SHA-256: `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`
- 제품 소스·원본·Git 상태는 이 감사에서 변경하지 않았다.
- 기계 판독 결과: `independence/rea-provenance.json`

`Analysis/export-manifest.json`의 원작 함수 목록은 1,133개다. 파일 본문 658, 파일 thunk 186, 합성 외부 thunk 56, 외부 import 233으로 분류돼 있다. 로컬 900항목은 추출 대상으로 기록돼 있다. 합성 외부 thunk 56항목의 본문 추출 실패를 56개 게임 기능 누락으로 해석하면 안 된다. Ghidra 의사코드는 원작 개발자의 원본 소스가 아니며, manifest도 `executable_source: false`로 표시한다.

## 기존 증거 전수 대조

`docs/public/rea/artifact-index.csv`를 읽고 각 실제 파일의 SHA-256을 다시 계산했다.

| 구분 | 파일 수 | 누락·불일치 |
|---|---:|---:|
| Analysis | 1,820 | 0 |
| 원본 아카이브 분석 증거 | 154 | 0 |
| 타이틀 분석 증거 | 20 | 0 |
| Resources/Evidence | 73 | 0 |
| 합계 | 2,067 | 0 |

이 목록에는 역사적 분석 증거를 보관한 `로컬 보존 분석 폴더/` 경로도 있다. 이 파일들의 존재는 분석 이력을 확인하기 위한 것이며 앱 빌드 입력이라는 뜻이 아니다.

## 이번 세션에서 REA 재조회

`binary_session`에서 열린 대상이 없음을 확인한 뒤, 보존된 원작 Mach-O를 Ghidra 12.1.4로 열고 `procedure_pseudo_code({procedure: "0x106e0"})`를 실행했다. 결과는 기존 `Analysis/evidence/0x106e0.pseudo.json`의 의사코드 문자열과 정확히 일치했다. Evidence ID도 같다.

- 함수: `GlobalInfo.addExp`, `0x106e0`
- Evidence: `ev_ee96914bf20181a47dc856df63729955f699db45e5ddb8aa51ab6b7607bb7e34`
- 원문: `build/no-ipa-audit-20261026/rea-fresh-106e0.json`
- 조회 뒤 `close_binary` 성공. 원작 파일에는 쓰지 않았다.

실제 새 구현 `Sources/Core/GlobalInfo.cpp:60`은 경험치 32비트 합산, 레벨 99 상한, 호출당 한 단계 증가, 업그레이드 포인트 증가, 다음 경험치 계산, 게이지 1.0 상한을 구현한다. `Sources/Core/Player.cpp:52`는 레벨업 결과를 받아 업그레이드 UI와 사운드 6을 요청하고 경험치 게이지 이벤트를 보낸다. `Sources/iOS/HMGameView.mm:405`와 `:407`은 이 이벤트를 실제 HUD에 전달한다. 분석된 계산식을 코드로 옮기고 화면·사운드 소비자까지 연결한 사례다.

## 25함수군 대응

다음 목록은 역사적 `family-audit-snapshot.json`의 경로와 대표 주소를 출발점으로 삼았다. 현재 파일 존재·내용을 확인하고, 뒤에 추가된 GLES/OpenAL 소비자를 보완했다. 주소별 Evidence ID와 파일 해시는 JSON 결과에 있다. 모든 행의 원작 전체 함수에 대해 이번에 줄 단위 의미 동등성을 재검사했다는 뜻은 아니다.

| 함수군 | 원작 대표 주소 | 현재 구현 | 구현 역할 |
|---|---|---|---|
| geometry | `0x5874`, `0x7250` | `Sources/Core/Geometry.cpp` | 벡터·선분 계산 |
| data_records | `0x11344` | `Sources/Core/GameData.cpp` | MAP/MCH 바이너리 구조 파싱 |
| global_progress_save | `0x105f8`, `0x106e0`, `0x1082c`, `0x10a0c`, `0x3ea0`, `0x3f44` | `Sources/Core/GlobalInfo.cpp`<br>`Sources/Core/SaveData.cpp`<br>`Sources/Core/GameData.cpp`<br>`Sources/iOS/HMGameView.mm` | 경험치·레벨·무기·저장 슬롯 |
| machine_motion | `0x479c`, `0x428c` | `Sources/Core/Motion.cpp`<br>`Sources/Core/GameSession.cpp` | 지상·공중·부스터 이동 |
| player | `0x1bd04`, `0x1be84`, `0x1c64c`, `0x1cc70` | `Sources/Core/Player.cpp`<br>`Sources/Core/GameSession.cpp` | 플레이어 업그레이드·아이템·경험치 |
| enemy | `0x14918` | `Sources/Core/Enemy.cpp`<br>`Sources/Core/GameSession.cpp` | 적 AI·피격·파괴·드롭 |
| combat | `0x9dc4`, `0xa198`, `0xa444`, `0xab04`, `0x11e64`, `0x12a40`, `0x13214` | `Sources/Core/Combat.cpp`<br>`Sources/Core/BulletVisual.cpp`<br>`Sources/Core/GameSession.cpp` | 탄 발사·이동·충돌·데미지·연사 |
| particles_parts | `0x92b4`, `0x94a8`, `0x968c`, `0x11818`, `0x118fc`, `0x11a48` | `Sources/Core/Visuals.cpp`<br>`Sources/Core/GameSession.cpp` | 파티클·기체 부품 애니메이션 |
| trails | `0x1ab20`, `0x1ac60`, `0x1acd0` | `Sources/Core/Trail.cpp`<br>`Sources/Core/TrailRaster.cpp`<br>`Sources/Core/GameSession.cpp`<br>`Sources/iOS/HMMedia.mm` | 궤적 생성·수명·메시 |
| items | `0x1d538` | `Sources/Core/ItemBox.cpp`<br>`Sources/Core/GameSession.cpp` | 아이템 이동·접촉 |
| world | `0xe3f4`, `0xc8b8`, `0xcd00`, `0xb8f0`, `0xbf74`, `0xc228` | `Sources/Core/World.cpp`<br>`Sources/Core/GameSession.cpp` | 맵·카메라·충돌·생성 |
| title | `0xf464`, `0xf934` | `Sources/Core/TitleState.cpp`<br>`Sources/iOS/HMGameView.mm` | 타이틀 시간·입력·난수 |
| menus | `0x18158`, `0x18fc0`, `0x19ba0`, `0x1a05c`, `0x1a900` | `Sources/iOS/HMMenus.mm` | 모드·미션·설정 메뉴 |
| hud | `0x151a0`, `0x161d0`, `0x16a98`, `0x17280` | `Sources/iOS/HMGameHUD.mm`<br>`Sources/iOS/HMGameView.mm` | 게이지·업그레이드·미션 결과 |
| screen_routing | `0x3374`, `0x3778`, `0x3890`, `0x38b0` | `Sources/Core/LoadingTransition.cpp`<br>`Sources/iOS/HMGameView.mm` | 문 전환·화면 변경 |
| camera_quake | `0xed80`, `0xeef8` | `Sources/Core/CameraShake.cpp`<br>`Sources/iOS/HMGameView.mm` | 진폭·위상·감쇠 |
| base_visual_font | `0x76a0` | `Sources/Core/GameSession.cpp`<br>`Sources/Core/ItemBox.cpp`<br>`Sources/iOS/HMMenus.mm`<br>`Sources/iOS/HMGameHUD.mm` | 게임에서 사용하는 글자·스프라이트 경로 구현; 원작 범용 노드 selector 전체 복제 아님 |
| buttons | `0x84d8`, `0x850c`, `0x87f8` | `Sources/iOS/HMButtonState.hpp`<br>`Sources/iOS/HMMenus.mm`<br>`Sources/iOS/HMGameHUD.mm` | 눌림·해제·지연·파동 상태 |
| containers_dispatch | `0xf9a8` | `Sources/Core/GameSession.cpp`<br>`Sources/iOS/HMGameView.mm` | C++ 컨테이너와 현재 입력 전달로 대체 |
| graphics_resources | `0x69b0`, `0x6f28` | `Sources/Core/SpriteRaster.cpp`<br>`Sources/Core/TrailRaster.cpp`<br>`Sources/iOS/HMMedia.mm`<br>`Sources/iOS/HMGameView.mm`<br>`Sources/iOS/HMGLRenderer.mm` | 현재 GLES 렌더러와 번들 리소스 소비 |
| audio | `0x13bd0`, `0x13d0c`, `0x13d2c` | `Sources/Core/AudioContract.hpp`<br>`Sources/iOS/HMMedia.mm`<br>`Sources/iOS/HMOpenALAudio.mm` | OpenAL 실제 출력·31 WAV 계약 |
| ios_platform | `0x20b8` | `Sources/iOS/Application.mm`<br>`Sources/iOS/HMGameView.mm`<br>`Sources/Core/Viewport.hpp`<br>`Sources/Core/TiltState.hpp` | 현재 UIApplication·NSTimer·화면·센서 처리 |
| linked_runtime_audio | `0x1db50` | `Sources/iOS/HMMedia.mm`<br>`Sources/iOS/HMOpenALAudio.mm` | 원작 내장 런타임의 현재 OpenAL/플랫폼 대체 |
| compiler_entry_helpers | `0x2000` | `Sources/iOS/Application.mm` | 현재 컴파일러와 UIApplication 진입점으로 대체 |
| analysis_fragments | `0x128ec` | `Sources/Core/Combat.cpp`<br>`Sources/Core/Enemy.cpp` | 무기/적 함수 안에 해당 분기 구현 |

## 직접 대조한 계산과 실행 경로

1. `GlobalInfo.cpp`의 `0x105f8`, `0x106e0`, `0x1082c`, `0x10944`: 경험치 누적·레벨 임계값·중복 무기 최대 충전·저장 배열 이동을 확인했다. 임계값을 부동소수점으로 계산한 뒤 정수로 잘라 누적하는 과정과 ARM32 저장 값 의미를 구현한다.
2. `Combat.cpp`의 `0x9dc4`, `0xa198`, `0xa444`, `0x11e64`, `0x12a40`, `0x13214`: 난수 소비·탄속·중력·충돌·폭발음·강화 데미지·공격 버프·방어·연사와 재장전을 확인했다. 원작 탄환 timestep `0x3c888889`, 강화식 `D + (D / 8) * grade`, 공격 1.5배의 정수 변환과 방어 정수 나눗셈이 코드에 있다.
3. `LoadingTransition.cpp`, `TitleState.cpp`, `CameraShake.cpp`: 문 상태 전환, 타이틀 시간 분기와 조건별 난수 소비, 카메라 흔들림 위상·감쇠를 확인했다.
4. `GameData.cpp`는 MAP/MCH 파일을 직접 읽고 재귀 레코드·무기·부품·기체 자료를 파싱한다. 원본 실행 파일을 호출해 값을 얻지 않는다.
5. `HMGameView.mm:101`은 번들의 데이터 디렉터리를 로드하고, GameSession을 생성한다. `:612`의 프레임 처리가 `:633`에서 실제 `GameSession::tick`과 HUD 처리를 호출한다. 현재 타이머는 NSTimer이며 프레임 시간과 난수는 현재 코드에 구현돼 있다.
6. `HMOpenALAudio.mm`은 OpenAL device/context, WAV PCM 파싱, buffer/source 생성과 재생을 구현한다. `AudioContract.hpp`는 실제 31개 WAV 이름과 거리 정책을 정의한다. 현재 오디오 경로를 과거 문서의 AVAudioPlayer 설명으로 판단하면 안 된다.
7. `HMGLRenderer.mm`은 GLES 렌더링을 구현한다. `HMMedia.mm`의 번들 텍스처와 게임 스프라이트 호출이 이 경로를 사용한다. Core에 남은 CPU raster 관련 파일만 보고 현재 화면 전체를 UIKit/CPU로 그린다고 판단하면 안 된다.

## 원본 IPA 실행 의존성과 빈 구현 검사

`Sources`, `iOS/project.yml`, `tools/build_openal.sh`, `CMakeLists.txt`에서 `.ipa`, `touchHLE`, `Compatibility`, `dlopen`, `dlsym`, `NSTask`, `system(`, `popen`, `fork(`, `execv`, `ARMv6`를 검색했다. 발견한 touchHLE 참조는 타이머·센서·난수·픽셀 처리의 비교 근거 주석과 `HMCompatibilityPixels.hpp`의 자체 계산이다. 원작 IPA나 ARM 실행 파일을 호출하는 실행 경로는 발견하지 않았다.

현재 진입점은 `Application.mm`의 `main → UIApplicationMain → HMAppDelegate → HMSceneDelegate → HMViewController → HMGameView`다. `iOS/project.yml`은 `Sources/Core`와 `Sources/iOS`를 컴파일한다. `Sources/OriginalInterfaces`는 `buildPhase: none`이다. 원작 인터페이스 선언을 제품 실행 코드로 컴파일하지 않는다.

`Compatibility/README.md`에 설명된 ARMv6 해석 실행판은 별도 프로젝트다. 그 호환판 빌드의 원본 IPA 요구를 현재 복원판의 요구로 혼동하면 안 된다.

제품 소스의 TODO/FIXME/stub/placeholder/unimplemented/fatalError/abort 및 빈 반환을 검색했다. 빈 반환이 있는 곳은 대상 없음, 충돌 후보 없음, 비활성·잘못된 크기, 발생한 이벤트 없음 등의 실제 분기였다. 직접 읽은 게임 기능 경로에서 임시 성공값으로 기능을 건너뛰는 구현은 찾지 못했다.

## 결과의 해석

확인한 것은 분석 증거의 실체와 무결성, 25함수군 구현 파일의 존재, 대표 증거 140건의 대응, 핵심 계산과 실제 앱 소비자의 연결, 이번 세션의 원작 함수 재조회다. 새 복원판은 C++/Objective-C++ 구현이다. Swift로 전부 다시 작성했다는 설명은 현재 소스 구성과 다르다.

정적 감사만으로 모든 입력·화면·소리를 원작과 완전히 같다고 판정하지 않는다. 앱 빌드·실행 가능 여부는 상위 작업의 독립 빌드 결과와 함께 판단해야 한다. 이 감사에서 구체적인 누락 게임 함수나 원본 IPA 실행 의존성을 발견하지 않았으므로, 추정으로 기능을 수정하지 않았다.
