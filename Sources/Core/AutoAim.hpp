#pragma once
#include "Geometry.hpp"
#include "ItemBox.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace heavymach {
enum class AimMode { off, manual, automatic };
enum class AimStatus { none, tracking, ready, blocked };
struct AimSolution { Vector2 velocity{};int ticks=0;float angle=.1f;explicit operator bool()const{return ticks>0;} };
struct AimSnapshot {
 AimMode mode=AimMode::manual;AimStatus status=AimStatus::none;
 uintptr_t target=0;Vector2 position{},screenPosition{};ObjectBounds bounds{};
 float angle=.1f;int flightTicks=0;uint64_t assistedShots=0,targetHits=0;
};
inline constexpr float aimTick=0x1.111112p-6f;
// Combat.cpp subtracts g before applying dt. Target displacement is per tick.
inline Vector2 interceptionVelocity(Vector2 origin,Vector2 target,Vector2 step,float g,int ticks){
 const float n=float(std::max(1,ticks));
 return {(target.x+step.x*(n-1)-origin.x)/(n*aimTick),
         (target.y+step.y*(n-1)-origin.y)/(n*aimTick)+g*(n+1)*.5f};
}
inline AimSolution solveAim(Vector2 origin,Vector2 target,Vector2 step,float speed,float g,float lifetime,bool highArc=false){
 AimSolution result;int limit=std::clamp(int(lifetime*.9f/aimTick),1,600);
 if(speed<=0){
  // Zero-speed support bombs start above their target and fall under gravity.
  if(g<=0||origin.y<=target.y)return result;
  for(int n=1;n<=limit;++n){float y=origin.y-aimTick*g*n*(n+1)*.5f;
   if(y<=target.y+step.y*(n-1)){result.ticks=n;result.velocity=interceptionVelocity(origin,target,step,g,n);result.angle=-1.57079632679f;return result;}}
  return result;
 }
 // Earliest discrete interception at nominal speed (at most 2% rounding margin).
 for(int candidate=1;candidate<=limit;++candidate){int n=highArc?limit-candidate+1:candidate;auto v=interceptionVelocity(origin,target,step,g,n);
  if(v.x*v.x+v.y*v.y<=speed*speed*1.0404f){result.velocity=v;result.ticks=n;result.angle=std::atan2(v.y,v.x);return result;}}
 return result;
}
}
