#include "CameraShake.hpp"
#include <bit>
#include <cmath>
namespace heavymach {
namespace {float fixed(float x){float v=x+.5f;int32_t i;if(std::isnan(v))i=0;else if(double(v)>=2147483647.)i=INT32_MAX;else if(double(v)<=-2147483648.)i=INT32_MIN;else i=int32_t(v);return float(i);}}
void CameraShake::setQuake(float amplitude,int32_t count,float duration,double time){startTime=float(time);delay=duration;size=amplitude;pitch=duration/float(count);}
void CameraShake::tick(double time){
 if(startTime!=0){float elapsed=float(time-double(startTime));
  if(!(elapsed<delay)){startTime=0;offset={0,0};}
  else {double phase=std::fmod(double(elapsed),double(pitch))/double(pitch);double sine=std::sin(phase*std::bit_cast<double>(uint64_t(0x400921fb54442d18)));float decay=1.f-elapsed/delay;offset.y=float((sine*double(size))*double(decay));}
 }
}
Vector2 cameraScreenPosition(Vector2 p,Vector2 cam,Vector2 half,const Vector2* offset,bool snap){
 if(offset){p.x=((p.x-cam.x)+half.x)-offset->x;p.y=((p.y-cam.y)+half.y)-offset->y;}
 if(snap){p.x=fixed(p.x);p.y=fixed(p.y);}return p;
}
}
