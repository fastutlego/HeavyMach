#pragma once
#include "Geometry.hpp"
#include <cstddef>
#include <array>
#include <cstdint>
#include <span>
#include <vector>
namespace heavymach {
enum class SpriteTextureFormat {PremultipliedRGBA8, RGB565};
// Content rows start at texture v=0 (image top). Uncropped source only.
// Zero padded dimensions choose next POT; explicit dimensions must be POT.
// RGB565 quantizes input RGB before sampling; input alpha is ignored. Its
// black padding has alpha=1, unlike transparent RGBA padding.
struct SpriteTextureView {
 int width=0,height=0;size_t stride=0;std::span<const uint8_t> rgba;
 SpriteTextureFormat format=SpriteTextureFormat::PremultipliedRGBA8;
 int paddedWidth=0,paddedHeight=0;
};
// Obtain once per raster/cache entry with validateOriginalTexture. Sampling
// accepts normalized coordinates relative to the WHOLE padded texture, not
// content. No buffer validation/allocation occurs per sample.
struct OriginalTextureSampler {SpriteTextureView texture;int paddedWidth=0,paddedHeight=0;};
OriginalTextureSampler validateOriginalTexture(SpriteTextureView);
std::array<double,4> sampleOriginalTexture(const OriginalTextureSampler&,double u,double v);
struct SpriteRasterInput {
 int columns=1,rows=1,tile=0;
 Vector2 position{},scale{1,1};float rotation=0,alpha=1;int blend=0;
};
struct SpriteRasterBounds {int x=0,y=0,width=0,height=0;};
struct SpriteRaster {
 SpriteRasterBounds bounds; // screen y-up; output row0 is minimum screen y.
 std::vector<uint8_t> rgba; // source RGB contribution and effective coverage
 int blend=0; // 0/3 normal; 2 additive. Already multiplied by source factor.
 bool empty()const{return rgba.empty();}
};
// T*S*R (rotation radians, positive CCW). GL_REPLACE: vertex alpha affects
// RGB565 only; RGBA uses sampled alpha. Whole POT texture GL_REPEAT/LINEAR;
// no atlas crop. Bounded to 480x320. Nonfinite/degenerate geometry yields empty.
// Invalid texture/atlas or unsupported blend1/inherited state throws.
SpriteRaster rasterizeSprite(const SpriteRasterInput&,SpriteTextureView,
                            SpriteRasterBounds clip={0,0,480,320});
}
