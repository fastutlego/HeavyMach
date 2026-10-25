# Trail CPU rasterizer와 native 합성 계약

2026-10-24. `TrailRaster.hpp/.cpp`는 원본 TrailMesh를 CoreGraphics bitmap으로 전달하기 위한 새 플랫폼 어댑터다. 원본 게임 로직 함수의 decompile 복원이라고 표현하지 않는다. 기하·색·UV·GL blend 근거는 `core-trail.md`와 원본 `0x1b8dc`다. 지원하는 Combat trail은 blend2이며 normal blend0 및 같은 additive blend3도 구현했다.

## API와 좌표

`rasterizeTrail(mesh, texture, clip)`은 `TrailRaster{bounds,rgba,blend}`를 반환한다. texture는 `TrailTextureView{width,height,stride,span}`의 premultiplied RGBA8888이고 첫 행은 v=0이다. 자동 세로 반전은 없다. host가 PNG 디코드의 행 방향을 맞춰야 한다.

mesh.translation은 **화면의 y-up 좌표**를 입력한다. Trail 코어 draw의 world translation을 그대로 전달하면 안 된다. vertices.position은 여전히 local y-up이다. 출력 bounds도 화면 y-up, 출력 첫 행은 bounds의 최소 y다. UIKit 이미지 생성/표시에서 필요한 세로 반전은 host가 한 번만 수행한다. clip은480×320 안으로 제한하며 기본값은 전체 화면이다. 출력의 최대 크기는480×320×4바이트다.

실제 덮인 삼각형의 bbox를 화면과 교차하여 출력 영역을 줄인다. UV/color/position이 비유한 삼각형과 면적0인 삼각형은 생략한다. 잘못된 texture stride/buffer는 명시적 예외다. 큰 좌표를 int로 먼저 캐스팅하지 않는다.

## 채움과 샘플링

Triangle strip의 홀수 삼각형은 첫 두 vertex를 교환한다. 양쪽 winding을 동일하게 처리하며 cull하지 않는다. pixel center(x+.5,y+.5)에 대해 barycentric 가중치를 계산한다. y-up에서 왼쪽/위 경계만 포함하는 top-left 규칙으로 공유 대각선을 한 번만 채운다. 원래 겹치는 별도 삼각형은 제출 순서대로 합성한다.

UV는 전체 텍스처의 normalized 좌표이며 좌표를0..1로 clamp한다. texel 중심 기준 `(u×width−.5,v×height−.5)`에서 네 이웃을 bilinear 보간하고, 이웃 인덱스는 edge texel에 clamp한다. 보간은 premultiplied 채널 그대로다. per-vertex RGBA도 barycentric로 보간해0..1로 제한한 뒤 texel RGBA와 각 채널을 곱한다. gamma 변환은 추가하지 않는다.

실제 OpenGL ES 하드웨어의 subpixel 고정소수점·중간 rounding·멀티샘플 상태까지 재현한 것은 아니다. 이 어댑터는 double edge/barycentric 계산, 단일 pixel-center coverage를 사용한다. 최종8bit 양자화 전 내부 합성은 double이다. 따라서 원본 GL framebuffer와 마지막1LSB까지 같다는 주장은 하지 않는다.

## 원본 텍스처와 알파

원본 `Analysis/functions/0x69b0.pseudo.c`의 Texture2D 이미지 업로드 경로는 CGBitmapContextCreate에0x4001을 사용한다. 이는32-bit big-endian + premultiplied last RGBA 입력이며 그 buffer를 texture에 전달한다. Trail draw는 SRC_ALPHA blend factor를 사용한다. 따라서 sampled premultiplied RGB에 vertex RGB를 곱한 `S.rgb`에 **S.a를 다시 곱하는** 원본 결과를 보존했다. 샘플을 임의로 unpremultiply하면 반투명 texel이 원본보다 밝아진다.

각 fragment의 texture×vertex 결과를 S, 내부 누적 RGB를 D라 하면:

- blend0: RGB=S.rgb×S.a+D.rgb×(1−S.a).
- blend2/3: RGB=min(S.rgb×S.a+D.rgb,1).

출력 bitmap은 native compositing용 premultiplied 기여량이다. 출력 alpha는 원본 GL framebuffer alpha가 아닌 **effective coverage**다. normal은 A=S.a+D.a×(1−S.a), additive는 A=min(S.a+D.a,1)이다. 이렇게 해야 host가 bitmap을 한 번 합성할 때 기존 framebuffer RGB에 대한 감쇠를 보존한다. 원본 GL alpha-channel 자체는 SRC_ALPHA 때문에 S.a² 항을 갖는다. 이 어댑터는 불투명 게임 화면의 최종 RGB를 목표로 하며 원본 framebuffer alpha 읽기 동작을 제공하지 않는다.

host는 blend0 결과를 normal source-over, blend2/3 결과를 premultiplied plus-lighter로 합성한다. bitmap RGB에 alpha를 다시 수동 곱하지 않는다. 하나의 draw mesh 안에서의 겹침은 이미 내부 합성했으므로 삼각형별 UIImage 반복 draw도 하지 않는다.

## SRC_COLOR mode1의 정확한 한계

원본 draw의 mode1은 SRC_COLOR / ONE_MINUS_SRC_ALPHA다(raw switch 증거는 core-trail.md). 수식은 RGB=S.rgb²+D.rgb×(1−S.a), framebuffer alpha=S.a²+D.a×(1−S.a)이다. S.rgb²가 S.a보다 커질 수 있다. 예를 들어 opaque white texture에 vertex alpha.1이면 RGB source contribution1, destination attenuation.9다. 이를 단일 **유효 premultiplied RGBA** UIImage의 source-over로 표현할 수 없다.

따라서 mode1은 지원하는 척 normal로 바꾸지 않고 `invalid_argument`를 반환한다. 정확한 지원에는 source contribution와 destination coverage를 별도 buffer로 합성하거나 전체 destination framebuffer 접근이 필요하다. 현재 원본 Combat TrailCreate는 모두 blend2이므로 해당 gameplay 경로는 지원한다. 알 수 없는 blend 또는 setBlend=false인 원본 상태상속도 host가 factor를 확정하기 전에는 거절한다.

## 실행한 검사

```
clang++ -std=c++20 -ffp-contract=off -fno-fast-math -fsanitize=address,undefined \
 HeavyMach/Sources/Core/TrailRaster.cpp HeavyMach/Tests/TrailRasterTests.cpp \
 /tmp/hm-trail-raster-main.cpp -o /tmp/hm-trail-raster-tests
/tmp/hm-trail-raster-tests
```

exit0, sanitizer 진단 없음. 검사 범위는4×4 quad 공유 대각선의 pixel당1회 채움, winding 반전, 두 겹 normal/additive 결과,2×2 RGBA bilinear, UV clamp, barycentric vertex color, translation·상단/우측 clip,1e30 좌표 bounded allocation, NaN/퇴화 무시, 짧은 texture buffer 거절이다. UIKit pixel decode/UIImage 합성과 실기기 화면은 부모 담당 범위이며 이 검사로 완료를 주장하지 않는다.
