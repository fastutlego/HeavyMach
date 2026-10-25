# Renderer CPU benchmark

Measured on Apple M4 Pro, macOS27.0.1 (26A434). Both command-line executables built and exited0. Warm CPU rasterization plus CGImage construction/480×320 compositing averaged4.00–5.59ms for the O2 game samples, but54.18–74.36ms at O0. The unoptimized renderer exceeds a16.67ms frame budget on this Mac. These measurements do not establish iPhone frame rate. The benchmark itself changed no application settings or gameplay code. Subsequent integration applies per-file optimization as recorded below.

## Method

`Tools/benchmark_renderer.mm` loads original Images using ImageIO and decodes into premultiplied RGBA8 bitmap buffers. Original RGB565 classification uses source CGImage alpha/color-space metadata; SpriteRaster performs the corresponding packed-channel sampling. TrailRaster uses the same RGBA decode convention as HMMedia. Actual GameSession render item order is retained.

Stage01 starts a normal fresh slot at the original spawn. Stage28/35 are performance fixtures with grade6, original finite weapons, first-boss approach spawn, and Missile03 selected to exercise actual trail meshes. They are **not journey/clear tests**. Each session runs180 original ticks with fixed seed11 and captures ticks121..180 (60 distinct frames). Tick and RNG capture happen before rendering; rendering repeats frozen draw lists without advancing simulation. No combat result is inferred from these fixtures.

Every referenced image is decoded before measurement. Each case renders its60 snapshots once for warm-up, then once for measurement. O2 and O0 runs execute sequentially. Output includes per-frame raster time, CGImage construction/compositing time, total wall time, tick time, RNG count, draw counts, and allocated raster pixel count. Pixel count includes transparent pixels inside raster bounds and overlapping draws; it is not unique screen coverage. Texture decode totals are separate and affected by filesystem/OS caching, so they are not optimization comparisons.

The CG bridge includes row reversal, CFData copying, provider/color-space/CGImage creation, normal/additive compositing, image release and CGContextFlush into a real480×320 bitmap. Total also includes texture lookup, temporary destruction and bitmap clearing, so it can exceed raster plus composite subtotals. HMMedia calls UIImage draw; this CLI uses CGContextDrawImage and does not measure UIKit final enlargement, HUD/menu draws, display scheduling or device thermal behavior. Camera shake is not supplied. Other system processes are uncontrolled.

The title stress case draws actual Main_BG.png at3×scale around(240,160), rotation0..1.18rad, clipped to480×320. It is a large transformed-background stress sample, not a claim that this exact sequence is the original title animation.

## Results

All times are milliseconds per measured frame. p95 is nearest-rank(57th of60); p50 in raw output is the upper-middle sample.

| Case | Build | Raster mean | CG mean | Total mean | Total p95 | Total max | Tick mean |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Stage01 | O2 | 3.688 | 0.308 | 4.003 | 4.061 | 4.085 | 0.003 |
| Stage28 | O2 | 3.907 | 0.364 | 4.277 | 4.382 | 4.413 | 0.008 |
| Stage35 | O2 | 5.080 | 0.502 | 5.592 | 5.823 | 6.079 | 0.007 |
| title_large_rotated | O2 | 2.412 | 0.152 | 2.567 | 2.596 | 2.609 | 0.000 |
| Stage01 | O0 | 50.550 | 1.589 | 54.180 | 55.699 | 56.199 | 0.087 |
| Stage28 | O0 | 53.092 | 1.937 | 57.560 | 60.135 | 62.438 | 0.131 |
| Stage35 | O0 | 68.739 | 2.469 | 74.359 | 77.587 | 79.694 | 0.175 |
| title_large_rotated | O0 | 32.072 | 0.951 | 34.287 | 34.597 | 34.808 | 0.000 |

| Case | Sprite draws/frame | Trail meshes/frame | Raster pixels/frame |
| --- | ---: | ---: | ---: |
| Stage01 | 41.15 | 0.00 | 247259 |
| Stage28 | 28.68 | 6.03 | 306804 |
| Stage35 | 64.80 | 3.67 | 382430 |
| title_large_rotated | 1.00 | 0.00 | 153600 |

All240 O0/O2 snapshots have identical case/frame index, cumulative RNG calls, sprite/trail counts and pixel counts. This verifies matching workload metadata; no pixel-by-pixel image equivalence test was performed.

## Reproduce and evidence

```sh
clang++ -std=c++20 -O2 -ffp-contract=off -DHM_VALIDATION=1 \
  HeavyMach/Tools/benchmark_renderer.mm HeavyMach/Sources/Core/*.cpp \
  -framework CoreGraphics -framework ImageIO -framework CoreFoundation \
  -o HeavyMach/build/render-performance/benchmark-O2
HeavyMach/build/render-performance/benchmark-O2 HeavyMach/Resources \
  HeavyMach/build/render-performance/O2 O2
```

Use-O0 and O0 output names for the other build. Evidence: `build/render-performance/O{0,2}.{csv,jsonl}`, `run-O{0,2}.log`, successful `build-O{0,2}-fixed.log`, and `manifest.json` with source hashes/environment. Initial builds omitted CoreFoundation and failed linking; their original build logs remain preserved. Successful builds have two SDK enum-combination deprecation warnings; no runtime errors were reported.

O0 is the concrete performance failure found here. Production/device optimization and full native frame timings must be checked before translating O2 Mac results into an application frame-rate claim.

## Follow-up build correction

The measured O0 failure justified `-O2` for **SpriteRaster.cpp and TrailRaster.cpp only**, in CMake and both Xcode application targets. Debug assertions and the existing floating-point flags remain enabled. After this configuration change, CTest14/14, real simulator UIKit4/4, and the unsigned iphoneos build passed. See [native integration](sprite-native-integration.md) for exact evidence. These results do not measure the whole native application frame rate on a physical iPhone.
