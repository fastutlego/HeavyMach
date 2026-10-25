#pragma once
#include "Trail.hpp"
#include "SpriteRaster.hpp"
namespace heavymach {
// Same uncropped original texture as sprites, including POT padding and format.
using TrailTextureView=SpriteTextureView;
struct TrailRasterBounds {int x=0,y=0,width=0,height=0;};
struct TrailRaster {
 TrailRasterBounds bounds; // y-up screen pixel bounds; rows start at minimum y.
 std::vector<uint8_t> rgba; // RGB contribution plus effective compositing alpha.
 int blend=0;
 bool empty()const{return rgba.empty();}
};
// Clip is always inside logical 480x320. Pixel center sampling, GL-style bilinear
// texel centers, GL_REPEAT and original GL_REPLACE. No texture resampling.
TrailRaster rasterizeTrail(const TrailMesh&,TrailTextureView,
                          TrailRasterBounds clip={0,0,480,320});
}
