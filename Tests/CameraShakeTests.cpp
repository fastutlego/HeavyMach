#include "../Sources/Core/CameraShake.hpp"
#include <cassert>
#include <cmath>
#include <limits>
using namespace heavymach;
void runCameraShakeTests(){
 CameraShake q;q.setQuake(10,4,.3f,0);q.tick(.0375);assert(q.offset.y==0&&q.startTime==0); // original zero sentinel
 q.setQuake(10,4,.3f,1);q.tick(1);assert(q.offset.y==0);q.tick(1.0375);assert(std::fabs(q.offset.y-8.75f)<.00001f);
 auto first=q.offset.y;q.tick(1.1125);assert(q.offset.y>0&&q.offset.y<first); // half-sine humps, not alternating signs
 q.offset.x=7;q.tick(1.15);assert(q.offset.x==7); // active override writes Y only
 q.tick(1.30001);assert(q.offset.x==0&&q.offset.y==0&&q.startTime==0);
 q.offset={2,3};q.setQuake(6,4,.3f,2);assert(q.offset.x==2&&q.offset.y==3); // trigger does not clear offset
 q.tick(2.0375);assert(std::fabs(q.offset.y-5.25f)<.00001f);
 q.setQuake(10,4,.3f,16777217.);assert(q.startTime==16777216.f);q.tick(16777217.);assert(q.startTime==0); // float start precision
 q.setQuake(10,4,0,1);q.tick(1);assert(q.startTime==0&&q.offset.y==0);
 q.setQuake(10,0,.3f,1);q.tick(1.1);assert(q.offset.y==0); // pitch inf -> phase0
 q.setQuake(10,4,.3f,1);q.tick(std::numeric_limits<double>::quiet_NaN());assert(q.startTime==0);
 Vector2 offset{2,5};auto p=cameraScreenPosition({100,50},{60,40},{240,160},&offset);assert(p.x==278&&p.y==165);
 auto hud=cameraScreenPosition({100,50},{60,40},{240,160},nullptr);assert(hud.x==100&&hud.y==50);
 auto negative=cameraScreenPosition({-1.7f,-.7f},{},{},nullptr,true);assert(negative.x==-1&&negative.y==0);
}
