# Frozen scene composition comparison

후속 HP 막대 수정 후 새3프레임 결과는 최대 RGB 차이2/2/2다. 새 목록은154draw이며 아래 수정 전156draw 결과와 구분한다. [최신 원본 근거·측정](gauge-registration.md)을 따른다. 두 실행 모두 픽셀 일치는 실패다.


## Result

Actual CGL and CPU raster plus CoreGraphics do **not** produce identical RGB for the three frozen game frames. No tolerance was relaxed and no product code was changed. This tool reports measurements; exit0 means the measurements completed, not pixel equivalence.

| Frozen frame | Sprites | Trails | Total draws | Max RGB difference | Channels different | Channels differing by >1 | Mean absolute RGB difference |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Stage01, tick180 | 41 | 0 | 41 | 2 | 8726 | 11 | .0189605 |
| Stage28, tick180 | 30 | 7 | 37 | 2 | 14374 | 41 | .0312826 |
| Stage35, tick180 | 74 | 4 | 78 | 3 | 25550 | 86 | .0556359 |

Each frame compares all480x320x3 RGB channels. Alpha is excluded: the CPU contribution/coverage buffer is not original GL framebuffer alpha. All156 draw entries were also rendered independently over black.

## Independence and scope

`Tools/compare_scene_renderer.mm` reuses `benchmark_renderer.mm`'s actual image decoder, CPU raster-to-CoreGraphics composition function, and snapshot conventions. It creates a separate legacy CGL context/FBO and uploads whole power-of-two textures to actual GL. The GL path does not invoke the CPU sampler, coverage, transform or blending formulas. It uses original GL_REPLACE, GL_LINEAR, default GL_REPEAT, sprite T*S*R and actual strip arrays; Trail modes2/3 use additive factors. The framebuffer is480x320 RGBA8 with original black clear alpha1 and dithering enabled.

The frame simulation runs once with deterministic seed11, advances180 ticks with the benchmark's input, then copies `renderItems()`. Both renderers consume that same ordered list. Stage01 is a fresh original slot. Stage28/35 use the benchmark's validation boss-start, grade6 and missile conditions. Rendering the saved list does not tick the game or consume RNG.

**This does not prove the native draw list or its order matches original runtime.** It compares two render implementations given the same native list. It does not capture a window, run an iPhone, install an app, or exercise UIKit. Original derived assets are reused without modification. Current native UIKit bridge has separate tests.

## Where the differences arise

All156 isolated draws have maximum RGB difference **0** between CPU raster output and that output composited by CoreGraphics over black. Thus image conversion/orientation and the single-image CoreGraphics copy do not add RGB error in these fixtures.

Compared directly with actual GL, individual CPU raster results are:

- Stage01: all41 draws differ by at most1. The full-frame maximum2 therefore involves accumulation.
- Stage28: `P_Body_07.png`, draw4, differs by2 in2 channels. `Bullet.png`, draw35, differs by2 in1 channel. Other isolated draws differ by at most1.
- Stage35: `HPGaugeBoss.png`, draw31, differs by3 in6 channels. Other isolated draws differ by at most1.

These isolated >1 differences are already present between actual GL and CPU raster output, before CoreGraphics composition. They cannot be explained solely by multiple-draw accumulation. GPU texture-filter/transform precision versus CPU arithmetic is a possible cause, but its exact internal source is **not established** by this test. Do not label it verified GPU fixed-filter rounding.

The first ordered prefix whose difference exceeds1 is recorded separately:

| Frame | Last draw index (zero based) | Pixel (+Y up), channel | CPU/GL before draw | CPU/GL source contribution over black | CPU coverage byte | CPU/GL after draw |
| --- | ---: | --- | --- | --- | ---: | --- |
| Stage01 | 5 | (479,107), G | 168 / 167 | 2 / 2 | 28 | 152 / 150 |
| Stage28 | 1 | (381,59), R | 33 / 34 | 0 / 0 | 37 | 28 / 30 |
| Stage35 | 1 | (28,284), R | 92 / 93 | 0 / 0 | 7 | 89 / 91 |

This locates additional error at a compositing step with already-different destination values. Equal rounded source contributions do not imply equal unrounded source alpha. CPU coverage quantization, GL sample precision and destination/blend rounding remain distinguishable unresolved inputs; the data does not assign the entire difference to CoreGraphics. No product patch was made from this inference.

## Reproduction

From HeavyMach, using the existing validation core library:

```sh
clang++ -std=c++20 -O2 -DHM_VALIDATION=1 -Wno-deprecated-declarations -Wno-deprecated-anon-enum-enum-conversion Tools/compare_scene_renderer.mm Sources/Core/SpriteRaster.cpp Sources/Core/TrailRaster.cpp build/core/libHeavyMachValidationCore.a -framework CoreFoundation -framework CoreGraphics -framework ImageIO -framework OpenGL -o /tmp/hm-compare-scene
/tmp/hm-compare-scene Resources build/render-reference/scenes
```

Compile and measurement completed with exit0. The initial link attempt omitted CoreFoundation and failed; adding its explicit framework fixed that tool-only build error. No comparison output was used from the failed invocation.

Final actual GL vendor/renderer: Apple M4 Pro, version2.1 Metal -91.7. Cold GL texture uploads are included in recorded GL timings; CPU timing excludes simulation and is not a performance benchmark here. Only three full frames,156 isolated draws, and prefixes through first >1 were evaluated. No repeated animation or visual inspection was performed.

Artifacts under `build/render-reference/scenes`:

- `comparison.jsonl`: environment and all full-frame measurements.
- `Stage*-gl.rgba`, `Stage*-cpu.rgba`, `Stage*-diff.rgba`: full bottom-to-top RGBA8 arrays. Diff RGB holds absolute error; its alpha is255.
- `Stage*-isolated.jsonl`: every individual draw, texture name, direct-raster-to-GL and CoreGraphics-to-raster maximum differences.
- `Stage*-prefix.jsonl`: ordered prefix results through the first difference greater than1, including that pixel's source and destination evidence.

Source/library SHA256 at execution:

```text
89ceca28e351fc87456c6fabf79170078b89cffcf2af9739c8991695330ffd22 Tools/compare_scene_renderer.mm
da1ad67e47fbc92f0af9d2046157bf42c7c7cf34c415950a8fd091ee2626ecd8 Tools/benchmark_renderer.mm
b398aead1850e5b9c3ef68872a8d48cdbfc1caa2992f97102fa0228b45492005 Sources/Core/SpriteRaster.cpp
8275c75a9e0082c6936acfc9667e8b136a8aa1bf3f2e7e8a32cb468d29a6d1e2 Sources/Core/TrailRaster.cpp
577440f3f5d3e4e31c66e4b537269d513d505cdeb148880917a81e3bf3b6b290 build/core/libHeavyMachValidationCore.a
```

The earlier tiny-texture CGL fixtures established their own max0/1 bounds. Those results are not a blanket tolerance guarantee for scaled real assets or whole-scene composition.

## App floating-point flags confirmation

The first command above omitted the app's explicit `-ffp-contract=off -fno-fast-math` options for directly compiled raster sources. Its output remains unchanged in `scenes/` as earlier evidence. One subsequent build and measurement used both options:

```sh
clang++ -std=c++20 -O2 -DHM_VALIDATION=1 -ffp-contract=off -fno-fast-math -Wno-deprecated-declarations -Wno-deprecated-anon-enum-enum-conversion Tools/compare_scene_renderer.mm Sources/Core/SpriteRaster.cpp Sources/Core/TrailRaster.cpp build/core/libHeavyMachValidationCore.a -framework CoreFoundation -framework CoreGraphics -framework ImageIO -framework OpenGL -o /tmp/hm-compare-scene-app-flags
/tmp/hm-compare-scene-app-flags Resources build/render-reference/scenes-app-flags
```

Both commands completed with exit0. All9 full-frame GL/CPU/difference RGBA files and all6 isolated/prefix JSONL files are **byte-identical** to the earlier results. Thus the maximum RGB differences2/2/3, >1 channel counts11/41/86, all156 single-draw findings, and first-prefix pixels are unchanged with app floating-point flags. Timing fields naturally differ and were not included in the byte-equality claim. The earlier measurements are not used to claim a weaker floating-point contract is acceptable.

`scenes-app-flags/execution.json` preserves the exact compiler/run commands, exit status, input source/library hashes and old/new hashes for all15 compared artifacts. No product source was changed and no numerical tolerance was added.
