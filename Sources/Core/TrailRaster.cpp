#include "TrailRaster.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
namespace heavymach {
namespace {
struct Vertex {double x,y,u,v;std::array<double,4> color;};
bool finite(const Vertex& v){if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v))return false;for(auto c:v.color)if(!std::isfinite(c))return false;return true;}
double edge(const Vertex& a,const Vertex& b,double x,double y){return (b.x-a.x)*(y-a.y)-(b.y-a.y)*(x-a.x);}
bool bottomLeft(const Vertex& a,const Vertex& b){return b.y<a.y||(b.y==a.y&&b.x>a.x);}
bool inside(double e,const Vertex& a,const Vertex& b){return e>0||(e==0&&bottomLeft(a,b));}
using Color=std::array<double,4>;
int lower(double n,int lo,int hi){return int(std::floor(std::clamp(n,double(lo),double(hi))));}
int upper(double n,int lo,int hi){return int(std::ceil(std::clamp(n,double(lo),double(hi))));}
}
TrailRaster rasterizeTrail(const TrailMesh& mesh,TrailTextureView texture,TrailRasterBounds clip){
 TrailRaster out;out.blend=mesh.blend==3?2:mesh.blend;
 auto sampler=validateOriginalTexture(texture);
 if(!mesh.setBlend||mesh.blend<0||mesh.blend>3)throw std::invalid_argument("trail blend requires resolved GL state");
 if(mesh.blend==1)throw std::invalid_argument("SRC_COLOR trail requires separate coverage/contribution host support");
 int left=std::clamp(clip.x,0,480),bottom=std::clamp(clip.y,0,320);
 int right=int(std::clamp(int64_t(clip.x)+clip.width,int64_t(0),int64_t(480))),top=int(std::clamp(int64_t(clip.y)+clip.height,int64_t(0),int64_t(320)));
 if(left>=right||bottom>=top||mesh.vertices.size()<3)return out;
 std::vector<std::array<Vertex,3>> triangles;
 double minX=right,minY=top,maxX=left,maxY=bottom;
 for(size_t i=2;i<mesh.vertices.size();++i){std::array<Vertex,3> tri;size_t ids[]={i-2,i-1,i};if(i&1)std::swap(ids[0],ids[1]);
  bool valid=true;for(int j=0;j<3;++j){auto& s=mesh.vertices[ids[j]];auto& d=tri[j];d={double(s.position.x)+mesh.translation.x,double(s.position.y)+mesh.translation.y,s.uv.x,s.uv.y,{s.color[0],s.color[1],s.color[2],s.color[3]}};valid=valid&&finite(d);}
  if(!valid)continue;double area=edge(tri[0],tri[1],tri[2].x,tri[2].y);if(!std::isfinite(area)||area==0)continue;if(area<0)std::swap(tri[0],tri[1]);
  double x0=std::min({tri[0].x,tri[1].x,tri[2].x}),x1=std::max({tri[0].x,tri[1].x,tri[2].x}),y0=std::min({tri[0].y,tri[1].y,tri[2].y}),y1=std::max({tri[0].y,tri[1].y,tri[2].y});
  if(x1<=left||x0>=right||y1<=bottom||y0>=top)continue;
  minX=std::min(minX,x0);maxX=std::max(maxX,x1);minY=std::min(minY,y0);maxY=std::max(maxY,y1);triangles.push_back(tri);
 }
 if(triangles.empty())return out;
 out.bounds={lower(minX,left,right),lower(minY,bottom,top),0,0};out.bounds.width=upper(maxX,left,right)-out.bounds.x;out.bounds.height=upper(maxY,bottom,top)-out.bounds.y;
 if(out.bounds.width<=0||out.bounds.height<=0)return out;
 std::vector<Color> pixels(size_t(out.bounds.width)*out.bounds.height);
 for(const auto& tri:triangles){auto& a=tri[0];auto& b=tri[1];auto& c=tri[2];double area=edge(a,b,c.x,c.y);
  int x0=lower(std::min({a.x,b.x,c.x}),out.bounds.x,out.bounds.x+out.bounds.width),x1=upper(std::max({a.x,b.x,c.x}),out.bounds.x,out.bounds.x+out.bounds.width);
  int y0=lower(std::min({a.y,b.y,c.y}),out.bounds.y,out.bounds.y+out.bounds.height),y1=upper(std::max({a.y,b.y,c.y}),out.bounds.y,out.bounds.y+out.bounds.height);
  for(int y=y0;y<y1;++y)for(int x=x0;x<x1;++x){double px=x+.5,py=y+.5,e0=edge(b,c,px,py),e1=edge(c,a,px,py),e2=edge(a,b,px,py);if(!inside(e0,b,c)||!inside(e1,c,a)||!inside(e2,a,b))continue;
   double w0=e0/area,w1=e1/area,w2=e2/area;Color source=sampleOriginalTexture(sampler,w0*a.u+w1*b.u+w2*c.u,w0*a.v+w1*b.v+w2*c.v);
   // App init 0x2120: GL_REPLACE. RGBA ignores vertex color/alpha;
   // GL_RGB replaces RGB but retains interpolated incoming alpha.
   if(texture.format==SpriteTextureFormat::RGB565)source[3]=std::clamp(w0*a.color[3]+w1*b.color[3]+w2*c.color[3],0.,1.);
   auto& dest=pixels[size_t(y-out.bounds.y)*out.bounds.width+size_t(x-out.bounds.x)];double alpha=source[3];double retain=mesh.blend==0?1-alpha:1;
   for(int k=0;k<3;++k)dest[k]=std::min(source[k]*alpha+dest[k]*retain,1.);
   dest[3]=std::min(alpha+dest[3]*retain,1.);
  }
 }
 out.rgba.resize(pixels.size()*4);for(size_t i=0;i<pixels.size();++i)for(int k=0;k<4;++k)out.rgba[i*4+k]=uint8_t(std::lround(std::clamp(pixels[i][k],0.,1.)*255.));return out;
}
}
