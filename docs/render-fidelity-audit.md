# 렌더 출력 대조 — 2026-10-24

이 문서는 RGB565 수정 당시의 조사 기록이다. 보간·atlas·GL_REPLACE 후속 수정과 실제 CGL/UIKit 검사는 [렌더 iOS 연결](sprite-native-integration.md)을 우선한다. 아래의 수정 보류·미측정 표시는 당시 상태다.

당시 HMMedia와 보존된 REA 원본 `0x69b0` Texture2D initWithImage, `0x634c` texture upload, `0x6d9c` tileSplit, `0x6f28` Tile2D draw의 pseudo/asm을 대조했다. 새로 확인한 구체 차이는 아래 세 가지다. RGB565 로딩만 수정했다. 보간·atlas 처리와 게임 코어는 변경하지 않았다. 보스 runner도 변경하지 않았다.

## 1. no-alpha 이미지 RGB565 변환 누락 — 좁은 수정 완료

원본 `0x69b0`의 분기는 다음과 같다.

- CGImageGetColorSpace가 nil이면 alpha-only format3.
- color space가 있고 CGImageAlphaInfo가1..4이면 RGBA8888 format1.
- 나머지는 RGB565 format2. 픽셀이 모두 불투명한지 검사하는 조건이 아니다.

format2는 bitmapInfo0x4005(big-endian RGBA, alpha noneSkipLast)로 이미지를 디코드한다. ARM `0x6d24..0x6d42`에서 little-endian으로 읽은 RGBA word의 R/G/B를 `(R>>3)<<11 | (G>>2)<<5 | B>>3`로 묶는다. `0x634c`는 GL_RGB/GL_UNSIGNED_SHORT_5_6_5로 업로드한다. 기존 HMMedia는 UIImage를 그대로 저장했으므로 이 색 양자화가 없었다. 예를 들어 decoded RGB(123,45,67)는 기존 경로에서 B67을 유지하지만 원본5-bit B는8/31이며8-bit 최근접 값은66이다.

원본 파일과 파생 PNG695개를 전수 조사했다. 원본 PNG RGBA679개는 파생RGBA679개, JPEG16개는 파생RGB16개다. macOS ImageIO의 실제 CGImageSourceCreateImageAtIndex 결과도 alphaInfo3(last)679개, alphaInfo5(noneSkipLast)16개로 일치했다. JPEG16개는 GameLoading_01..07 및 E variant14개, StageClear.jpg, 실제 JPEG인 Icon.png다. 파일 확장자로 판정하면 Icon.png를 놓치므로 그렇게 구현하지 않았다.

`Tools/export_resources.py:108` 이후 원본PNG는 RGBA로 재인코딩하고 JPEG는 sips PNG 변환을 사용한다. 현재 원본에는 RGB PNG가 없으므로 파생 CGImage의 alpha 분기가 원본과 같다. 향후 RGB PNG를 추가하면 exporter가RGBA로 바꿀 수 있으므로 이 전수 대응을 다시 확인해야 한다. 현재 실행에는 별도 metadata가 필요하지 않다. `Resources/texture-formats.json`은695개 원본/파생 SHA256·형식·분기 결과를 보존한 증거이며 앱에서 읽지 않는다.

수정은 HMMedia의 image 로딩 단계에만 추가했다. 원본 조건에 해당하는 이미지만 전체 이미지 디코드→565 packing→정규화된 RGB를8-bit에 최근접 저장한다. alpha255를 유지한다. atlas crop 전에 적용하며 RGBA679개는 기존 경로를 따른다. 원본 파일과 기존 파생 PNG는 다시 쓰지 않았다.

`Sources/Core/TextureFormat.hpp`의 함수는 header-only다. GL의5/6-bit 정규화값을 CG8-bit에 저장할 때 각 채널 최대0.5/255의 표현 오차가 남는다. 이후 GL과CoreGraphics의 보간까지 완전히 같다는 의미는 아니다. 이 좁은 수정은 원본 색 단계 누락을 복원한다.

검사:

```sh
clang++ -std=c++20 -Wall -Wextra -Werror -fsanitize=address,undefined HeavyMach/Tests/TextureFormatTests.cpp -o /tmp/hm-texture-format-tests
/tmp/hm-texture-format-tests
xcrun --sdk iphonesimulator clang++ -std=c++20 -fobjc-arc -fsyntax-only -target arm64-apple-ios16.0-simulator -isysroot "$(xcrun --sdk iphonesimulator --show-sdk-path)" -I HeavyMach/Sources/Core HeavyMach/Sources/iOS/HMMedia.mm
```

host 테스트 exit0: alpha format 분기, R/G/B bit packing·최하위 bit 경계, 위 B67→66 회귀,65536개 packed 값의 roundtrip 및 정확 정규화값 대비0.5byte 이내 오차. iOS Simulator syntax 검사 exit0. 기존 draw/trail의 enum OR warning3개는 남아 있다. 전체 Xcode 빌드와CTest 통합은 부모 담당이며 이 문서의 host 검사로 대체하지 않는다.

## 2. alpha 곱을 texture 보간 전에 수행 — 차이 확인, 수정 보류

HMMedia는 frame cache 생성 때 각 texel의 premultiplied RGB에 texel alpha를 미리 곱한다. 그 뒤 UIImage를 scale/rotate/subpixel 위치로 그린다. 후속 원본 초기화 대조에서 GL_REPLACE를 확인했다. RGBA texture는 먼저 샘플링한 뒤 SRC_ALPHA를 적용하며 vertex color/alpha를 무시한다. 초기 GL_MODULATE 가정은 정정한다. 따라서 두 연산의 순서가 다르다.

원본 RGBA: `filter(Cpremul) × filter(A)`.
당시 구현: `filter(Cpremul × A) × vertexAlpha`.

불투명 흰 texel과 투명 검정 texel의 중간은 원본 source RGB0.25, 미리 곱한 cache의 선형 보간은0.5다. 이 차이는 반투명 텍셀의 원본 이중 alpha 곱 자체를 없애야 한다는 뜻이 아니다. 곱하는 시점이 다르다.

실제 `Efx_Damage.png`(370×64)의(x148,y10),(x149,y10) RGBA는[170,255,255,3]과[185,238,238,105]다. 파생 PNG를 stdlib PNG filter/DEFLATE decoder로 읽어 계산했다. 그 중간의 red 기여량은 원본 순서0.0324608, cache 우선 순서0.0615496이다(8-bit rounding 전). UIKit이 어떤 보간 품질을 선택하는지와 최종 화면의 픽셀 차이는 이번에 측정하지 않았다. 다만 이 두 연산이 일반적으로 동등하지 않다는 것은 입력 값으로 확인했다. 원본 texel 중심·보간 없는 경우에는 이 차이가 없다.

필요한 후속 검사는 같은 텍스처·scale·rotation·alpha의 GL 및 native 출력을 비교하는 것이다. 이번에는 sampler나renderer를 대규모 변경하지 않았다.

## 3. atlas crop이 원본 texture 경계 샘플을 제거 — 차이 확인, 수정 보류

원본 `0x69b0`은 power-of-two 크기 bitmap을 clear하고 content를 배치한다. `0x634c`는 MIN_FILTER=GL_LINEAR를 지정한다. `0x6d9c`는maxS/splitX,maxT/splitY를 UV간격으로 사용한다. `0x6f28`은 이 UV경계 그대로 전체 texture를 샘플링한다. half-texel inset이나 tile별 texture 생성은 없다.

HMMedia는 CGImageCreateWithImageInRect로 tile만 잘라서 별도UIImage로 만든다. 따라서 원본 texture 경계 주변의 인접 tile texel 및power-of-two zero padding은 이 이미지에 없다. scale/rotation으로 경계 부근을 샘플링하면 원본의 이웃 texel 기여를 재현할 수 없다. non-integer tile 크기의 crop은 추가로 정수 pixel crop과 원본 UV구간의 차이를 만들 수 있다. atlas 열/행 및 draw 위치는 같아도 경계 sampling까지 같지 않다.

패딩이 없는 이유로 모든 sprite 크기가 잘못됐다는 결론은 내리지 않는다. 원본 halfWidth는 paddedWidth×maxS/splitX/2=contentWidth/splitX/2이므로 현재 w/2와 같다.695개 파생 이미지에는1024를 넘는 크기가 없어 원본의1024제한 축소 누락은 현재 이미지의 활성 결함으로 분류하지 않았다.

## flip·rotation·blend 검토 범위

원본 행 우선 tile 번호, 상한 frame clamp, T×S×R 변환 순서는 현재 구현과 일치했다. 화면Y 변환 후 `-rotation`을 적용하는 부호 및 음수 scale을 flip으로 사용하는 경로에서 새 불일치를 찾지 않았다. 이 결론은 실제 pixel 비교를 수행했다는 의미는 아니다.

mode1의 SRC_COLOR와상태상속 blend는 기존 문서에서 미지원으로 기록돼 있으므로 새 활성 결함으로 반복 보고하지 않았다. 현재 주요sprite의mode0/2를 대상으로 위 보간 순서와RGB565 차이를 조사했다. raw Tile2D switch bytes0x707e는 `04 09 03 06 09 0d`이며 mode3은Tile2D에서normal 경로에 합류한다. Trail의mode3 additive와 혼동해 HMMedia의mode3을 일괄 additive로 바꾸면 안 된다.

실기기·UI 조작과 원본 화면 캡처 비교는 수행하지 않았다. 이번 산출물은 실행 코드/원본 명령/파일 형식에 근거한 차이와 좁은 RGB565 로딩 수정이다.

## 통합 검사 및 독립 검토

CMake TextureFormat target 연결 후 CTest13개가 통과했다. simulator 검증 앱과 iphoneos 일반 앱의 서명 제외 빌드가 성공했다. 첫 device 빌드의 scheme 누락은 project.yml에 두 scheme을 명시해 수정했고 실패와 재시도 로그를 분리 보존했다. 위13개/두 빌드 로그는 `build/texture-format-*`에 있다.

독립 검토에서 필수 수정 결함을 찾지 못했다. 새로 확인한 CG bitmap의 비대칭6픽셀 방향·채널·양자화 및 원본 버퍼 소멸 후 CGImage 수명 검사는 ASan/UBSan을 통과했다. 소스 사본은 `build/render-review/hm-texture-bridge-review.cpp`다. 이 host 검사는 실제 UIKit 화면 출력 동등성 검사를 대신하지 않는다.
