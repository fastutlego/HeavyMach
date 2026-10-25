> 과거 구현·검사 기록이다. 현행 출력 경로는 [그래픽 정책](rendering-policy.md)과 [시간·저장·음향 정책](runtime-policy.md)을 따른다. 이 문서의 UIKit/AVAudioPlayer 설명을 새 작업의 구현 기준으로 사용하지 않는다.

# SpriteRaster — 원본 texture sampling과 blend 복원

2026-10-24. `Sources/Core/SpriteRaster.hpp/.cpp`는 원본 OpenGL texture·quad 계약을 CoreGraphics에 전달하는 portable C++20 어댑터다. atlas를 tile별로 자르지 않으며 전체 texture에서 linear filtering을 수행한 뒤 source blend factor를 곱한다. 게임 로직을 변경하지 않았다.

## 원본 근거와 수정한 가정

| 원본 주소 | 근거 | 구현 |
|---|---|---|
|0x69b0|POT bitmap clear→content 배치, RGBA8888 또는RGB565|content view+POT 크기; padding을 sampler에서 생성|
|0x6d24..0x6d42|R5/G6/B5 packing|RGB565 texel을 sampling 전에 양자화하고31/63으로 정규화|
|0x634c|GL_LINEAR MIN filter, wrap 변경 없음|실제CGL에서 확인한 default GL_REPEAT/MAG_LINEAR|
|0x6d9c|maxS/splitX,maxT/splitY|분수 크기atlas를 반올림하지 않는 UV 구간|
|0x6f28|T×S×R, 위쪽v0, tile 상한clamp|원본 row-major 및 양/음scale·rotation|
|0x707e raw switch|`04 09 03 06 09 0d`|Tile blend0/3normal,2additive;1/상태상속 거절|
|0x2120/0x21ac|glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,0x1e01)|**GL_REPLACE**|

초기 구현 가정은GL_MODULATE였다. 독립 REA 검토에서 원본 앱 초기화의GL_REPLACE를 확인해 수정했다. RGBA texture는 incoming vertex alpha를 무시한다. RGB565(GL_RGB)는 texture RGB와 incoming vertex alpha를 사용한다. 이 동작은 실제 OpenGL readback으로도 확인했다. 기존 문서의 `sample×vertexAlpha` 설명을 모든 texture에 일반화하면 안 된다.

최종 source RGB기여량은 RGBA의 경우 `filteredPremultipliedRGB × filteredTextureAlpha`, RGB565의 경우 `filteredQuantizedRGB × clamp(vertexAlpha)`다. normal의 destination 계수는1−sourceAlpha, additive는1이다. RGBA 이미지의alpha0 draw도 원본GL_REPLACE에서는texturealpha를 사용하므로 임의로 skip하지 않는다.

## API와 저장 방향

`SpriteTextureView`는 원본 전체content의width/height/stride/RGBA span, format, optional paddedWidth/paddedHeight다. 첫 buffer row는 원본 이미지 위쪽(v0)이다. pad dimensions0은nextPOT다. 명시한padding은POT이고content이상이어야 한다. 이미 padding이 포함된 rawbuffer도content크기와padded stride를 주면 읽을 수 있지만 padding값은 원본 clear규칙으로 재현한다.

- RGBA는 premultiplied RGBA8 입력이다. padding은(0,0,0,0)이다.
- RGB565는 입력RGB를 원본packing으로 처리하고alpha는 무시한다. GL_RGB이므로padding은검정RGB·alpha1이다. 파생8-bit를565로 다시packing해도 동일한5/6-bit 값이다.
- 현재 원본695개 이미지는1024이하다.1024초과입력은 원본Texture2D downscale을host가 먼저 적용해야 하며 함수가 조용히 다른 크기로 그리지 않는다.

`validateOriginalTexture(view)`는 크기·stride·buffer범위·format·POT를 한 번 검사하고`OriginalTextureSampler`를 반환한다. `sampleOriginalTexture(sampler,u,v)`의UV는 **전체POT texture의정규화좌표**다. per-pixel검증/할당은 없고GL_REPEAT/linear sampling만 한다. 원본maxS/maxT를 무시하고content끝을u/v1로 잘못 매핑하지 않는다. buffer수명은호스트가보유한다. nonfiniteUV는zero를반환한다.

`rasterizeSprite(input,texture,clip)`의input은columns/rows/tile,position,scale,rotation(radians),alpha,blend다. 논리screen은480×320이며+Y가위다. transform은T×S×R이다. 역변환으로각screenpixelcenter의local좌표를 구해UV를 계산한다. convexquad의coverage를한번만계산해대각선seam/중복blend가없다. 실제GLreadback과맞춘y-up bottom/left경계포함규칙을사용한다.

반환값`SpriteRaster`는y-up screenbounds와RGBAbytes다. 첫row는최소screeny다. RGB는이미sourcefactor를곱한기여량이고alpha는native합성용coverage다. host는normal0/3또는additive2로1:1합성한다. RGB에alpha를다시곱하면안된다. GLframebufferalpha는SRC_ALPHA때문에제곱항을가지므로이coverage와직접비교하지않는다. 불투명최종화면RGB를보존하기위한계약이다.

메모리는clip과480×320내부bbox에만할당한다. 전체texture복사나POTbuffer할당은없다. 한quad는한번만sampling하며4texel을보간한다. 음수/0scale,offscreen,nonfinite,너무큰geometry를검사하고invalidbuffer의size_t overflow를거절한다.0scale/nonfinite geometry는empty다.

## 지원하지 않는 blend

mode1 SRC_COLOR/ONE_MINUS_SRC_ALPHA는단일유효premultipliedUIImage로일반적으로표현할수없다. destination coverage와RGBcontribution을별도로처리하는framebuffer합성이필요하다. 알수없는mode는원본에서이전GL상태를상속하므로해당state가없는이API는이를정상mode로대체하지않는다. 둘다invalid_argument다. Tile2D의mode3은normal이며Trail의mode3additive와다르다.

## 검사와 실제 GL 비교

```sh
clang++ -std=c++20 -Wall -Wextra -Werror -fsanitize=address,undefined HeavyMach/Tests/SpriteRasterTests.cpp HeavyMach/Sources/Core/SpriteRaster.cpp -o /tmp/hm-sprite-tests
/tmp/hm-sprite-tests HeavyMach/build/render-reference
```

exit0. 기본검사는alpha경계의보간후곱,GL_REPLACE의RGBA/RGB565분기,atlas인접tile,transparent/opaquePOTpadding,분수atlas,wholetextureREPEAT,upperframeclamp,비균일scale+rotation,수평/수직flip,실제GL경계coverage,clip,nonfinite/degenerate,invalidbuffer/stride/format/blend를포함한다.

optional인자는다른담당자가Core를include하지않고실제CGL의glDrawArrays/glReadPixels로생성한fixture디렉토리다. 최종manifest는AppleM4Pro/OpenGL2.1 Metal91.7,GL_REPLACE,GL_REPEAT,MIN/MAG_LINEAR,dithertrue,clearRGBA(0,0,0,1)이다. RGB480×320전영역을비교했다. GLframebufferalpha는비교대상에서제외했다.

| 실제GLcase | 최대RGBbyte차이 | 차이>1채널수 |
|---|---:|---:|
|alpha_midpoint|0|0|
|alpha_vertex_additive|0|0|
|atlas_neighbor_padding|1|0|
|atlas_flip_rotate|1|0|
|atlas_minification|0|0|
|rgb565_padding|0|0|
|rgb565_vertex_alpha|0|0|

초기top-left coverage구현은GL경계에서최대255차이와90~144채널차이를냈다. 실제GLy-up bottom/left규칙으로수정한뒤위결과를얻었다. atlas의최대1byte차이는GPU/CPUfilter및정수변환정밀도범위로기록하며bit-identical이라고표현하지않는다. GL_MODULATE를가정했던중간reference는`build/render-reference/modulate/`에별도보존했다. 최종CPU결과는`build/render-reference/sprite-cpu-comparison.log`다.

이검사는7개합성fixture의실제OpenGLRGB비교와portable경계검사다. 원본기기GPU나실기기UI전체pixel동등성검사는아니다. HMMedia연결,TrailRaster공유sampler소비및native전체frame합성은부모가담당한다.
