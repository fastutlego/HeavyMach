#pragma once
#include <cstdint>
namespace heavymach {
// Texture2D initWithImage 0x69b0: nil color space => A8; alphaInfo 1..4
// => RGBA8888; otherwise RGB565. Values are CGImageAlphaInfo ABI values.
constexpr bool originalTextureUsesRGB565(bool hasColorSpace,unsigned alphaInfo) {
    return hasColorSpace && !(alphaInfo>=1 && alphaInfo<=4);
}
// Original ARM 0x6d24..0x6d42, little-endian RGBA word after bitmap decode.
constexpr uint16_t packOriginalRGB565(uint8_t r,uint8_t g,uint8_t b) {
    return uint16_t((uint16_t(r>>3)<<11)|(uint16_t(g>>2)<<5)|(b>>3));
}
// GL normalizes the packed channels by 31/63. The CG bridge stores the nearest
// 8-bit value; at most half an 8-bit step differs from the exact normalized GL
// sample, before later interpolation. Do not use bit replication as a substitute.
struct TextureRGB8 {uint8_t r,g,b;};
constexpr TextureRGB8 originalRGB565ToRGB8(uint16_t value) {
    return {uint8_t((((value>>11)&31)*255+15)/31),
            uint8_t((((value>>5)&63)*255+31)/63),
            uint8_t(((value&31)*255+15)/31)};
}
}
