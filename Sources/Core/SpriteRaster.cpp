#include "SpriteRaster.hpp"
#include "TextureFormat.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
namespace heavymach {
namespace {
using Color=std::array<double,4>;
struct Point {double x,y;};
int pot(int n){int p=1;while(p<n)p*=2;return p;}
int wrap(int n,int size){int r=n%size;return r<0?r+size:r;}
Color texel(SpriteTextureView t,int x,int y,int pw,int ph){
 x=wrap(x,pw);y=wrap(y,ph);
 const bool rgb=t.format==SpriteTextureFormat::RGB565;
 if(x>=t.width||y>=t.height)return {0,0,0,rgb?1.:0.};
 size_t o=size_t(y)*t.stride+size_t(x)*4;
 if(rgb){auto packed=packOriginalRGB565(t.rgba[o],t.rgba[o+1],t.rgba[o+2]);return {double((packed>>11)&31)/31,double((packed>>5)&63)/63,double(packed&31)/31,1};}
 return {t.rgba[o]/255.,t.rgba[o+1]/255.,t.rgba[o+2]/255.,t.rgba[o+3]/255.};
}
Color sample(SpriteTextureView t,double x,double y,int pw,int ph){
 // x/y are unnormalized full-texture coordinates. GL centers are n+.5.
 x-=.5;y-=.5;int ix=int(std::floor(x)),iy=int(std::floor(y));double fx=x-ix,fy=y-iy;Color c{};
 for(int j=0;j<2;++j)for(int i=0;i<2;++i){auto p=texel(t,ix+i,iy+j,pw,ph);double w=(i?fx:1-fx)*(j?fy:1-fy);for(int k=0;k<4;++k)c[k]+=p[k]*w;}
 return c;
}
double edge(Point a,Point b,Point p){return (b.x-a.x)*(p.y-a.y)-(b.y-a.y)*(p.x-a.x);}
// Observed GL framebuffer edge convention: bottom/left included in y-up.
bool bottomLeft(Point a,Point b){return b.y<a.y||(b.y==a.y&&b.x>a.x);}
}
OriginalTextureSampler validateOriginalTexture(SpriteTextureView t){
 if(t.width<1||t.height<1||t.width>1024||t.height>1024||t.stride<size_t(t.width)*4)throw std::invalid_argument("invalid sprite content dimensions; host must apply original 1024 downscale");
 if(t.format!=SpriteTextureFormat::PremultipliedRGBA8&&t.format!=SpriteTextureFormat::RGB565)throw std::invalid_argument("invalid sprite texture format");
 size_t rowBytes=size_t(t.width)*4;
 if(size_t(t.height-1)>(std::numeric_limits<size_t>::max()-rowBytes)/t.stride||t.rgba.size()<size_t(t.height-1)*t.stride+rowBytes)throw std::invalid_argument("short sprite texture buffer");
 int pw=t.paddedWidth?t.paddedWidth:pot(t.width),ph=t.paddedHeight?t.paddedHeight:pot(t.height);
 if(pw<t.width||ph<t.height||pw>1024||ph>1024||(pw&(pw-1))||(ph&(ph-1)))throw std::invalid_argument("invalid sprite padded texture dimensions");
 return {t,pw,ph};
}
std::array<double,4> sampleOriginalTexture(const OriginalTextureSampler& sampler,double u,double v){
 if(!std::isfinite(u)||!std::isfinite(v))return {};
 // GL_REPEAT reduces huge finite coordinates before integer texel conversion.
 u-=std::floor(u);v-=std::floor(v);
 return sample(sampler.texture,u*sampler.paddedWidth,v*sampler.paddedHeight,sampler.paddedWidth,sampler.paddedHeight);
}
SpriteRaster rasterizeSprite(const SpriteRasterInput& s,SpriteTextureView t,SpriteRasterBounds clip){
 SpriteRaster out;out.blend=s.blend;
 if(s.blend!=0&&s.blend!=2&&s.blend!=3)throw std::invalid_argument("sprite blend requires supported explicit factors; SRC_COLOR/inherited state unsupported");
 auto sampler=validateOriginalTexture(t);int pw=sampler.paddedWidth,ph=sampler.paddedHeight;
 if(s.columns<1||s.rows<1||s.tile<0)throw std::invalid_argument("invalid sprite atlas");
 if(!std::isfinite(s.position.x)||!std::isfinite(s.position.y)||!std::isfinite(s.scale.x)||!std::isfinite(s.scale.y)||!std::isfinite(s.rotation)||!std::isfinite(s.alpha)||s.scale.x==0||s.scale.y==0)return out;
 int left=std::clamp(clip.x,0,480),bottom=std::clamp(clip.y,0,320);
 int right=int(std::clamp(int64_t(clip.x)+clip.width,int64_t(0),int64_t(480))),top=int(std::clamp(int64_t(clip.y)+clip.height,int64_t(0),int64_t(320)));
 if(left>=right||bottom>=top)return out;
 int64_t tile=std::min(int64_t(s.tile),int64_t(s.columns)*s.rows-1);
 double w=double(t.width)/s.columns,h=double(t.height)/s.rows;
 double cs=std::cos(double(s.rotation)),sn=std::sin(double(s.rotation));
 auto transform=[&](double x,double y){return Point{s.position.x+s.scale.x*(cs*x-sn*y),s.position.y+s.scale.y*(sn*x+cs*y)};};
 std::array<Point,4> corners={transform(-w/2,-h/2),transform(w/2,-h/2),transform(w/2,h/2),transform(-w/2,h/2)};
 if(double(s.scale.x)*s.scale.y<0)std::reverse(corners.begin(),corners.end());
 double minX=right,minY=top,maxX=left,maxY=bottom;
 for(auto p:corners){if(!std::isfinite(p.x)||!std::isfinite(p.y))return out;minX=std::min(minX,p.x);minY=std::min(minY,p.y);maxX=std::max(maxX,p.x);maxY=std::max(maxY,p.y);}
 int x0=int(std::floor(std::clamp(minX,double(left),double(right)))),y0=int(std::floor(std::clamp(minY,double(bottom),double(top))));
 int x1=int(std::ceil(std::clamp(maxX,double(left),double(right)))),y1=int(std::ceil(std::clamp(maxY,double(bottom),double(top))));
 if(x1<=x0||y1<=y0)return out;
 out.bounds={x0,y0,x1-x0,y1-y0};out.rgba.resize(size_t(x1-x0)*(y1-y0)*4);
 double originX=double(tile%s.columns)*w,originY=double(tile/s.columns)*h;
 double vertexAlpha=std::clamp(double(s.alpha),0.,1.);
 for(int y=y0;y<y1;++y)for(int x=x0;x<x1;++x){
  Point p{x+.5,y+.5};bool covered=true;
  for(int k=0;k<4;++k){auto a=corners[k],b=corners[(k+1)%4];double e=edge(a,b,p);if(e<0||(e==0&&!bottomLeft(a,b))){covered=false;break;}}
  if(!covered)continue;
  double rx=(p.x-s.position.x)/s.scale.x,ry=(p.y-s.position.y)/s.scale.y;
  double localX=cs*rx+sn*ry,localY=-sn*rx+cs*ry;
  auto color=sampleOriginalTexture(sampler,(originX+localX+w/2)/pw,(originY+h/2-localY)/ph);
  // App init0x2120 sets GL_REPLACE. RGBA replaces incoming alpha;
  // GL_RGB replaces only RGB and retains vertex alpha.
  double alpha=t.format==SpriteTextureFormat::RGB565?vertexAlpha:color[3];size_t o=(size_t(y-y0)*(x1-x0)+size_t(x-x0))*4;
  for(int k=0;k<3;++k)out.rgba[o+k]=uint8_t(std::lround(std::clamp(color[k]*alpha,0.,1.)*255));
  out.rgba[o+3]=uint8_t(std::lround(std::clamp(alpha,0.,1.)*255));
 }
 return out;
}
}
