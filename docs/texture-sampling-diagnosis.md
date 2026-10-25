# Texture sampling diagnosis

The three real-asset discrepancies are present before CoreGraphics. Disabling GL blending shows that source samples already differ. This diagnostic does not establish a precise GPU fixed-point format and does not justify changing product sampling or tolerances.

## Procedure

`Tools/diagnose_texture_sampling.mm` copies the independent CGL path from `compare_scene_renderer.mm` and reuses the benchmark image decoder/snapshot rules. It regenerates the same Stage28/35 tick180 lists and selects indices4/35/31. No original resource or product source is modified.

For each selected sprite:

1. Render the original GL_REPLACE/SRC_ALPHA draw over black.
2. Disable GL_BLEND and draw the same sprite to read raw sampled RGBA8, before alpha multiplication.
3. Compare those bytes against the production CPU sampler at current inverse T*S*R coordinates, retaining unrounded CPU samples in the log.
4. Replace only the diagnostic texture with opaque white to compare primitive coverage independently of transparent source pixels.
5. At every pixel where the original contribution differs by more than1, supply the CPU-calculated UV (converted to GLfloat) as a constant UV quad. Read the **same framebuffer pixel**, preserving the original dithering position. This removes varying-UV interpolation across the transformed sprite while retaining actual GL texture filtering and RGBA8 output conversion. CPU samples at those GLfloat UVs are logged separately to expose double-to-float input loss.

Dithering remains enabled. GL_LINEAR is used for both minification and magnification. Whole POT texture content/padding and GL_REPEAT are unchanged. CPU and tool use `-ffp-contract=off -fno-fast-math`, as the app does.

## Observed results

| Sprite | Position | Scale / radians | White coverage mismatch | Raw RGBA maximum difference | Raw channels >1 | Final RGB maximum / channels >1 |
| --- | --- | --- | ---: | ---: | ---: | --- |
| Stage28 draw4 P_Body_07 | 98.1984863,108.9408493 | 1,1 / .0422283858 | 1 | 1 | 0 | 2 /2 |
| Stage28 draw35 Bullet | 192.8769531,140.6091003 | 1,1 / .0701780394 | 0 | 1 | 0 | 2 /1 |
| Stage35 draw31 HPGaugeBoss | 156.9761047,178.2076263 | .3120000064,1 /0 | 0 | 2 | 4 | 3 /6 |

Coverage for Body07 differs at(75,128): CPU includes the pixel and GL excludes it. Both actual textured RGB contributions there are0, so this is not the observed >1 error. The two affected Body/Bullet sample points and all six HP gauge points are covered in both paths. A general coverage equivalence claim would nevertheless be wrong.

Body/Bullet raw sample differences of at most1 become contribution differences of2 after source-alpha multiplication. HP gauge has raw differences of2 before blending and contribution differences up to3. Thus neither CoreGraphics composition nor alpha multiplication alone explains the HP gauge result.

## Constant-UV control

At the eight problematic pixels, constant-UV GL samples differ by at most1 from rounded CPU samples, and by at most1 from their original transformed/varying-UV GL samples. This separates a small varying-coordinate/raster stage effect from remaining texture-sample/output quantization. It does not identify the GPU's interpolation bit count or internal rounding mode.

Examples at HP gauge pixel(141,178):

- CPU unrounded raw RGBA×255: `[228.2020,124.7769,123.5073,228.5746]`.
- Actual GL, constant CPU UV at the same pixel: `[229,125,124,229]`.
- Actual GL, original varying UV: `[230,126,124,230]`.
- CPU contribution RGB: `[205,112,111]`; original GL contribution: `[207,113,112]`.

At(141,177), CPU raw `[223.0893,66.3475,64.0287,224.5110]`, constant-UV GL `[223,67,64,225]`, varying-UV GL `[224,67,64,226]`; final CPU/GL red is196/199.

Multiplying the readback RGBA8 bytes is not always identical to actual GL blended output. For example Body07 pixel(87,114) has raw GL `[163,170,152,243]`; rounded byte multiplication gives red155 while actual blended red is156. This shows that RGBA8 raw readback is not a lossless observation of the precision retained during filtering/blending. It does not establish which internal stage rounds. The tool records that diagnostic calculation but does not use it as a replacement oracle.

No fitting parameter, sampler change, tolerance change or product patch was introduced. GPU transform/UV interpolation and sample/output precision remain the narrowed unresolved boundary. The old iPhone GPU could differ from this Apple M4 Pro CGL implementation.

## Reproduce and evidence

```sh
clang++ -std=c++20 -O2 -DHM_VALIDATION=1 -ffp-contract=off -fno-fast-math -Wno-deprecated-declarations -Wno-deprecated-anon-enum-enum-conversion Tools/diagnose_texture_sampling.mm Sources/Core/SpriteRaster.cpp Sources/Core/TrailRaster.cpp build/core/libHeavyMachValidationCore.a -framework CoreFoundation -framework CoreGraphics -framework ImageIO -framework OpenGL -o /tmp/hm-diagnose-sampling
/tmp/hm-diagnose-sampling Resources build/render-reference/sampling-diagnosis
```

Compile and execution completed with exit0. Exit0 means collection completed, not equivalence. No app/window/device operation was used. Existing scene outputs are preserved.

`build/render-reference/sampling-diagnosis/diagnosis.jsonl` records transforms, dimensions and all summary counts. Per-draw `*-pixels.jsonl` records UV, original raw/blended GL, CPU unrounded/float-UV samples, constant-UV GL, and geometry mismatches. `*-gl-raw.rgba`, `*-gl-blended.rgba`, `*-cpu-contribution.rgba` retain full480x320 bottom-up arrays. `execution.json` records commands, source hashes and artifact hashes.
