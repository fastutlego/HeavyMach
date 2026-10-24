#pragma once
#include "Geometry.hpp"
#include <cstdint>
namespace heavymach {
struct CameraShake {
 Vector2 offset{};
 float startTime=0,delay=0,size=0,pitch=0;
 void setQuake(float amplitude,int32_t count,float duration,double globalTime);
 // Original global g_time, not paused World.time. Base child tick is host-owned.
 void tick(double globalTime);
};
// worldPosition includes parent translation. A null offset means no camera.
// fixedPosition implements original trunc(value+.5), including negative values.
Vector2 cameraScreenPosition(Vector2 worldPosition,Vector2 cameraPosition,
                            Vector2 halfScreen,const Vector2* offset,bool fixedPosition=false);
}
