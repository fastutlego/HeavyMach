#pragma once
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
namespace heavymach {
// touchHLE src/image.rs gamma_decode/gamma_encode and
// frameworks/core_graphics/cg_bitmap_context.rs draw_image/put_pixel.
// Texture2D 0x69b0 first clears its bitmap, then draws the decoded image once.
// RGBA's premultiplied Normal blend against (0,0,0,0) yields the source.
// RGB565's NoneSkipLast branch returns the source without blending (bg alpha=1).
// Both therefore apply the same float gamma round-trip, truncating to u8.
// Runtime powf intentionally matches the host libm used by the compatibility
// binary; do not substitute double pow, sRGB transfer curves, or rounded bytes.
struct CompatibilityPixelTables {
    std::array<uint8_t,256> rgb{},alpha{};
    CompatibilityPixelTables() {
        for(size_t i=0;i<256;++i) {
            float component=float(i)/255.f;
            rgb[i]=uint8_t(::powf(::powf(component,2.2f),1.f/2.2f)*255.f);
            alpha[i]=uint8_t(component*255.f);
        }
    }
};
inline const CompatibilityPixelTables& compatibilityPixelTables() {
    static const CompatibilityPixelTables tables;return tables;
}
// Apply only to copied source pixels; leave zero POT padding unchanged.
// RGB565 ignores alpha when packing; opaque source alpha remains 255 here.
inline void compatibilityBitmapPixels(uint8_t *rgba,size_t pixelCount) {
    const auto& t=compatibilityPixelTables();
    for(size_t i=0;i<pixelCount;++i,rgba+=4) {
        rgba[0]=t.rgb[rgba[0]];rgba[1]=t.rgb[rgba[1]];rgba[2]=t.rgb[rgba[2]];
        rgba[3]=t.alpha[rgba[3]];
    }
}
}
