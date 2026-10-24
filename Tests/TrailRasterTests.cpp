#include "../Sources/Core/TrailRaster.hpp"
#include <cassert>
#include <limits>
#include <stdexcept>
using namespace heavymach;
namespace {
TrailVertex vertex(float x,float y){TrailVertex v;v.position={x,y};v.uv={.5f,.5f};return v;}
TrailMesh quad(float w,float h){TrailMesh m;m.vertices={vertex(0,0),vertex(w,0),vertex(0,h),vertex(w,h)};return m;}
uint8_t at(const TrailRaster& r,int x,int y,int k){return r.rgba[(size_t(y-r.bounds.y)*r.bounds.width+x-r.bounds.x)*4+k];}
}
void runTrailRasterTests(){
 const uint8_t half[]={128,128,128,128};TrailTextureView texture{1,1,4,half};
 auto m=quad(4,4);auto r=rasterizeTrail(m,texture);assert(r.bounds.width==4&&r.bounds.height==4);
 // Shared diagonal crosses pixel centers; every pixel is covered exactly once.
 for(int y=0;y<4;++y)for(int x=0;x<4;++x){assert(at(r,x,y,0)==64&&at(r,x,y,3)==128);}
 // Reversed winding remains visible, no culling.
 std::swap(m.vertices[0],m.vertices[1]);std::swap(m.vertices[2],m.vertices[3]);auto reversed=rasterizeTrail(m,texture);assert(reversed.rgba==r.rgba);
 auto overlap=quad(4,4);auto v=overlap.vertices;overlap.vertices.insert(overlap.vertices.end(),{v[3],v[0],v[0],v[1],v[2],v[3]});
 auto normal=rasterizeTrail(overlap,texture);assert(at(normal,1,1,0)==96&&at(normal,1,1,3)==192);
 overlap.blend=2;auto add=rasterizeTrail(overlap,texture);assert(at(add,1,1,0)==129&&at(add,1,1,3)==255);
 overlap.blend=3;auto add3=rasterizeTrail(overlap,texture);assert(add3.rgba==add.rgba&&add3.blend==2);
 const uint8_t texels[]={255,0,0,255,0,255,0,255,0,0,255,255,255,255,255,255};TrailTextureView four{2,2,8,texels};
 auto bilinear=rasterizeTrail(quad(1,1),four);assert(at(bilinear,0,0,0)==128&&at(bilinear,0,0,1)==128&&at(bilinear,0,0,2)==128);
 // App never changes default GL_REPEAT. Integral UV wraps to the texture edge,
 // whose bilinear footprint spans opposite sides, rather than clamping.
 auto repeat=quad(1,1);for(auto& p:repeat.vertices)p.uv={-100,100};auto repeated=rasterizeTrail(repeat,four);assert(at(repeated,0,0,0)==128&&at(repeated,0,0,1)==128&&at(repeated,0,0,2)==128);
 const uint8_t white[]={255,255,255,255};TrailTextureView solid{1,1,4,white};TrailMesh triangle;triangle.vertices={vertex(0,0),vertex(2,0),vertex(0,2)};
 // Original app init 0x2120 sets GL_REPLACE: RGBA texture replaces incoming
 // color AND alpha, even when the trail's lifecycle writes a fading alpha.
 triangle.vertices[0].color={1,0,0,0};triangle.vertices[1].color={0,1,0,.2f};triangle.vertices[2].color={0,0,1,.4f};auto colors=rasterizeTrail(triangle,solid);assert(at(colors,0,0,0)==255&&at(colors,0,0,1)==255&&at(colors,0,0,2)==255&&at(colors,0,0,3)==255);
 // Content3x1 pads to4x1. Its right edge blends with transparent padding;
 // content-normalized sampling would incorrectly return full white.
 const uint8_t npotPixels[]={255,255,255,255,255,255,255,255,255,255,255,255};
 TrailTextureView npot{3,1,12,npotPixels};auto padding=quad(1,1);for(auto& p:padding.vertices)p.uv={.75f,.5f};
 auto padded=rasterizeTrail(padding,npot);assert(at(padded,0,0,0)==64&&at(padded,0,0,3)==128);
 auto rgb=solid;rgb.format=SpriteTextureFormat::RGB565;auto fading=quad(1,1);for(auto& p:fading.vertices)p.color={0,0,0,.5f};
 auto rgbResult=rasterizeTrail(fading,rgb);assert(at(rgbResult,0,0,0)==128&&at(rgbResult,0,0,3)==128);
 auto moved=quad(4,4);moved.translation={478,319};auto clipped=rasterizeTrail(moved,solid);assert(clipped.bounds.x==478&&clipped.bounds.y==319&&clipped.bounds.width==2&&clipped.bounds.height==1);
 auto enormous=quad(1e30f,1e30f);enormous.translation={-1,-1};auto bounded=rasterizeTrail(enormous,solid);assert(bounded.rgba.size()<=480*320*4);
 auto bad=quad(4,4);for(auto& p:bad.vertices)p.position.x=std::numeric_limits<float>::quiet_NaN();assert(rasterizeTrail(bad,solid).empty());
 auto flat=quad(0,4);assert(rasterizeTrail(flat,solid).empty());assert(rasterizeTrail(moved,solid,{0,0,0,0}).empty());
 auto unsupported=quad(1,1);unsupported.blend=1;bool unsupportedRejected=false;try{rasterizeTrail(unsupported,solid);}catch(const std::invalid_argument&){unsupportedRejected=true;}assert(unsupportedRejected);
 bool rejected=false;try{rasterizeTrail(quad(1,1),{2,2,8,half});}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
}
