# Original fixed GL reference

`Tools/original_gl_reference.mm` creates a CGL legacy context and an offscreen RGBA8 framebuffer. It calls actual GL texture upload, matrix, array, blend and readback APIs. It does not include `SpriteRaster`, `TrailRaster`, or their sampling/coverage formulas. No window, simulator, screenshot, application installation or device access is used.

## Evidence and original state

- App initialization `0x20b8`: `0x2116..0x2120` calls `glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE)`. Literal `0x21ac` contains bytes `01 1e 00 00` (`0x1e01`). Exported functions contain no other texture-environment setter. The earlier MODULATE assumption was wrong.
- Original clear color is black with alpha 1. Dithering remains enabled by default.
- Texture load `0x69b0`: original CoreGraphics bitmap decode, zero POT padding, translate by padded height minus content height. No-alpha color images are packed RGB565 (`0x6d24..0x6d42`); RGBA images preserve premultiplied bytes.
- Texture upload `0x634c`: only MIN_FILTER is explicitly set to LINEAR. MAG_FILTER defaults LINEAR and wrap S/T default REPEAT. `maxS/maxT` are content dimensions divided by padded dimensions.
- Tile coordinates `0x6d9c` use the full padded texture; tiles are not extracted to separately clamped images.
- Tile draw `0x6f28`: translate, scale, rotate, triangle strip, incoming color alpha; normal modes 0/3 use SRC_ALPHA/ONE_MINUS_SRC_ALPHA, mode 2 uses SRC_ALPHA/ONE.
- Trail UV writer `0x1acd0` queries tile.maxS/maxT when writing each new point. A 40x64 texture padded to64x64 therefore uses maxS=.625,maxT=1. Alternating rows use v=maxT or0. Sampling at the content boundary mixes with padding; repetition occurs at the POT boundary.
- Trail draw `0x1b8dc` binds texture, color/UV/vertex arrays, then draws the strip without changing texture environment. Its blend switch differs from Tile: bytes at `0x1b91e` are `04 03 05 09 09 0d`; cases2/3 both reach `0x1b930` (SRC_ALPHA,ONE). A review claim that Trail3 should be normal was withdrawn after checking this table.

Under GL_REPLACE, RGBA replaces incoming RGB and alpha: vertex alpha and vertex RGB do not modulate the sampled texture. GL_RGB replaces RGB but retains incoming alpha. Therefore the original RGBA Trail color-array fade does not change fragment alpha, although lifetime/geometry still change.

## Execute

From the HeavyMach project directory:

```sh
clang++ -std=c++20 -fobjc-arc -Wno-deprecated-declarations -ffp-contract=off Tools/original_gl_reference.mm -framework Foundation -framework CoreGraphics -framework ImageIO -framework OpenGL -o /tmp/hm-original-gl-reference
/tmp/hm-original-gl-reference build/render-reference
```

Executed successfully on Apple M4 Pro, vendor Apple, OpenGL `2.1 Metal - 91.7`. Final manifest reports textureEnvMode7681 (REPLACE), wrap10497 (REPEAT), min/mag9729 (LINEAR), dither true, viewport480x320. CGL/FBO failures produce failure.json and a nonzero exit.

## Artifacts and observations

`build/render-reference/manifest.json` records eight final cases and probes. Each case has an uncompressed RGBA8 readback and PNG. Raw frame rows run bottom-to-top; source texture dumps retain CoreGraphics storage order with original content top at v0. RGB565 upload words are separately preserved. PNGs are diagnostic views, not screenshots.

| Case | Condition | Actual RGBA at (240,160) |
| --- | --- | --- |
| alpha_midpoint | RGBA opaque/transparent midpoint, normal | 64,64,64,191 |
| alpha_vertex_additive | Same texture, vertex alpha .5, additive | 64,64,64,255 |
| atlas_neighbor_padding | Non-POT atlas, internal neighbors and padding | 170,66,8,225 |
| atlas_flip_rotate | Negative scale, nonuniform scale, rotation .37 | 27,41,44,193 |
| atlas_minification | Scale .7, rotation .2 | 64,0,64,191 |
| rgb565_padding | RGB565 non-POT texture | 16,93,181,255 |
| rgb565_vertex_alpha | Same RGB565 texture, vertex alpha .5 | 8,47,90,191 |
| trail_rgba_passthrough | World-space quad, per-vertex RGB and alpha .1/.3/.5/.7 | 64,64,64,191 |

The Trail passthrough uses the same texture/UV/geometry as alpha_midpoint but translated vertex arrays instead of sprite scale, and different incoming colors. All614400 readback bytes match alpha_midpoint. This directly checks RGBA replacement independently of the CPU raster implementation.

`trail-compare.cpp` compares current TrailRaster to that actual GL readback. Full480x320 RGB comparison passes with max difference0 and zero channels differing by more than1. It also checks Trail blend2/3 equality for overlapping geometry. `trail-compare-before.txt` includes the withdrawn 0-vs-3 expectation; it is not a valid regression failure. `trail-compare-final.txt` is the corrected result.

`no-dither/` and `modulate/` retain earlier experiments. Both used the superseded MODULATE assumption and are not final reference outputs. Root-level outputs use the original REPLACE state.

## Limits

These are macOS GL results for tiny deterministic textures, not observations of the old iPhone GPU. Desktop precision and driver edge behavior can differ from that GPU. Full app UI and real device appearance have not been checked by this tool. Framebuffer alpha (SRC_ALPHA applied to alpha too, over clear alpha1) is not the CPU raster's compositing coverage alpha; RGB is the comparison target. Native CGContext composition and final viewport scaling remain a separate integration boundary.

Read-only native integration review: HMMedia subtracts media camera offset before rasterization; HMGameView resets it before HUD/loading and composes at480x320, scale1 before final viewport expansion. No extra world CGContext translation remains. The actual Trail assets (LaserTrail and SmokeTrail variants) use RGBA, so HMMedia's default RGBA Trail texture view matches those assets. No general RGB565 Trail inference is claimed.
