#include "../Sources/Core/SpriteRaster.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
using namespace heavymach;
using Pixel=std::array<uint8_t,4>;
static Pixel pixel(const SpriteRaster& r,int x,int y){assert(x>=r.bounds.x&&y>=r.bounds.y&&x<r.bounds.x+r.bounds.width&&y<r.bounds.y+r.bounds.height);size_t o=(size_t(y-r.bounds.y)*r.bounds.width+x-r.bounds.x)*4;return {r.rgba[o],r.rgba[o+1],r.rgba[o+2],r.rgba[o+3]};}
static void equal(Pixel a,Pixel b,int tolerance=0){for(int i=0;i<4;++i)assert(std::abs(int(a[i])-b[i])<=tolerance);}
static void compareGL(const std::filesystem::path& directory){
 struct Case {const char* name;const char* texture;int w,h,pw,ph,cols,rows,tile;float x,y,sx,sy,angle,alpha;int blend;bool rgb;};
 const Case cases[]={
  {"alpha_midpoint","alpha_edge",2,1,2,1,1,1,0,240.5f,160.5f,8,8,0,1,0,false},
  {"alpha_vertex_additive","alpha_edge",2,1,2,1,1,1,0,240.5f,160.5f,8,8,0,.5f,2,false},
  {"atlas_neighbor_padding","atlas",3,3,4,4,2,2,0,240.25f,160.75f,12,9,0,1,0,false},
  {"atlas_flip_rotate","atlas",3,3,4,4,2,2,3,240.25f,160.75f,-9,6,.37f,.65f,0,false},
  {"atlas_minification","atlas",3,3,4,4,1,1,0,240.5f,160.5f,.7f,.7f,.2f,1,0,false},
  {"rgb565_padding","opaque",3,1,4,1,1,1,0,240.5f,160.5f,8,8,0,1,0,true},
  {"rgb565_vertex_alpha","opaque",3,1,4,1,1,1,0,240.5f,160.5f,8,8,0,.5f,0,true}
 };
 auto load=[&](std::string name){std::ifstream file(directory/name,std::ios::binary);if(!file)throw std::runtime_error("missing independent GL fixture: "+name);return std::vector<uint8_t>(std::istreambuf_iterator<char>(file),{});};
 for(const auto& c:cases){
  auto bytes=load(std::string(c.texture)+".rgba"),gl=load(std::string(c.name)+".rgba");assert(gl.size()==480*320*4);
  SpriteTextureView texture{c.w,c.h,size_t(c.pw)*4,bytes,c.rgb?SpriteTextureFormat::RGB565:SpriteTextureFormat::PremultipliedRGBA8,c.pw,c.ph};
  SpriteRasterInput input;input.columns=c.cols;input.rows=c.rows;input.tile=c.tile;input.position={c.x,c.y};input.scale={c.sx,c.sy};input.rotation=c.angle;input.alpha=c.alpha;input.blend=c.blend;
  auto raster=rasterizeSprite(input,texture);int maximum=0,overOne=0;
  // CGL framebuffer alpha is NOT the effective coverage returned by this API.
  // Compare final RGB over the reference's black framebuffer only.
  for(int y=0;y<320;++y)for(int x=0;x<480;++x)for(int k=0;k<3;++k){int cpu=0;
   if(x>=raster.bounds.x&&x<raster.bounds.x+raster.bounds.width&&y>=raster.bounds.y&&y<raster.bounds.y+raster.bounds.height)cpu=pixel(raster,x,y)[k];
   int delta=std::abs(cpu-int(gl[(y*480+x)*4+k]));maximum=std::max(maximum,delta);overOne+=delta>1;
  }
  std::cout<<c.name<<": max RGB byte difference "<<maximum<<", channels >1 "<<overOne<<"\n";
  assert(overOne==0);
 }
}
int main(int argc,char** argv){
 std::array<uint8_t,8> alphaEdge{255,255,255,255,0,0,0,0};
 SpriteTextureView texture{2,1,8,alphaEdge};SpriteRasterInput s;s.position={4,4};s.scale={2,2};
 auto a=rasterizeSprite(s,texture);equal(pixel(a,4,4),{16,16,16,64});equal(pixel(a,3,4),{143,143,143,191});
 // Source factor follows filtering. Pre-multiplying again before filtering
 // would return RGB64 here, reproducing the prior HMMedia defect.
 s.alpha=.5;auto faded=rasterizeSprite(s,texture);equal(pixel(faded,4,4),{16,16,16,64}); // GL_REPLACE ignores RGBA vertex alpha
 s.alpha=1;s.blend=2;auto add=rasterizeSprite(s,texture);assert(add.rgba==a.rgba&&add.blend==2);
 s.blend=3;auto mode3=rasterizeSprite(s,texture);assert(mode3.rgba==a.rgba&&mode3.blend==3);s.blend=0;
 s.position={4.5f,4.5f};auto boundary=rasterizeSprite(s,texture);equal(pixel(boundary,4,3),{64,64,64,128});equal(pixel(boundary,4,5),{0,0,0,0});s.position={4,4};
 // Adjacent atlas texels remain visible to filtering; cropping would yield red.
 std::array<uint8_t,8> atlas{255,0,0,255,0,0,255,255};texture.rgba=atlas;s.columns=2;
 auto neighbor=rasterizeSprite(s,texture);equal(pixel(neighbor,4,4),{191,0,64,255});
 // Texture seam repeats the whole POT texture, as the original default wrap.
 equal(pixel(neighbor,3,4),{191,0,64,255});
 std::array<uint8_t,12> three{255,255,255,255,255,255,255,255,255,255,255,255};texture={3,1,12,three};s.columns=3;s.tile=2;
 auto padded=rasterizeSprite(s,texture);equal(pixel(padded,4,4),{143,143,143,191});
 texture.format=SpriteTextureFormat::RGB565;auto opaquePadding=rasterizeSprite(s,texture);equal(pixel(opaquePadding,4,4),{191,191,191,255});
 s.alpha=.5;auto rgbFade=rasterizeSprite(s,texture);equal(pixel(rgbFade,4,4),{96,96,96,128});s.alpha=1;
 texture.format=SpriteTextureFormat::PremultipliedRGBA8;s.columns=2;s.tile=1;s.position={6,6};s.scale={4,2};
 auto fraction=rasterizeSprite(s,texture);equal(pixel(fraction,7,6),{195,195,195,223});
 s.tile=999;assert(rasterizeSprite(s,texture).rgba==fraction.rgba);
 // Exact texel centers after nonuniform T*S*R; a reversed matrix order fails.
 std::array<uint8_t,16> quad{255,0,0,255,0,255,0,255,0,0,255,255,255,255,255,255};texture={2,2,8,quad};s={};s.position={10.5f,10};s.scale={2,3};s.rotation=1.5707963267948966f;
 auto rotated=rasterizeSprite(s,texture);equal(pixel(rotated,9,8),{255,0,0,255},1);
 s={};s.position={4,4};s.scale={-1,1};auto flip=rasterizeSprite(s,texture);equal(pixel(flip,4,4),{255,0,0,255});equal(pixel(flip,3,4),{0,255,0,255});
 s.scale={1,-1};auto vflip=rasterizeSprite(s,texture);equal(pixel(vflip,3,4),{0,0,255,255});
 // Fullscreen/huge transforms stay bounded and output storage is <=480*320*4.
 s.scale={1e30f,1e30f};s.position={240,160};auto huge=rasterizeSprite(s,texture);assert(huge.rgba.size()<=480*320*4);
 s.scale={0,1};assert(rasterizeSprite(s,texture).empty());s.scale={1,1};s.rotation=std::numeric_limits<float>::infinity();assert(rasterizeSprite(s,texture).empty());
 s={};s.position={-1e30f,-1e30f};assert(rasterizeSprite(s,texture).empty());
 s.position={4,4};assert(rasterizeSprite(s,texture,{0,0,-1,320}).empty());
 auto rejects=[&](SpriteRasterInput input,SpriteTextureView view){bool threw=false;try{(void)rasterizeSprite(input,view);}catch(const std::invalid_argument&){threw=true;}assert(threw);};
 s.blend=1;rejects(s,texture);s.blend=4;rejects(s,texture);s.blend=0;s.columns=0;rejects(s,texture);s.columns=1;s.tile=-1;rejects(s,texture);s.tile=0;
 auto invalid=texture;invalid.paddedWidth=3;rejects(s,invalid);invalid=texture;invalid.rgba=std::span<const uint8_t>(quad.data(),3);rejects(s,invalid);
 invalid=texture;invalid.stride=std::numeric_limits<size_t>::max();rejects(s,invalid);
 auto sampler=validateOriginalTexture(texture);auto center=sampleOriginalTexture(sampler,.25,.25);auto repeated=sampleOriginalTexture(sampler,2.25,-3.75);assert(center==repeated);assert(center[0]==1&&center[3]==1);
 assert((sampleOriginalTexture(sampler,std::numeric_limits<double>::infinity(),0)==std::array<double,4>{}));
 if(argc>1)compareGL(argv[1]);
 std::cout<<"SpriteRaster filtering/atlas/padding/565/transform/clipping/invalid inputs passed\n";
}
