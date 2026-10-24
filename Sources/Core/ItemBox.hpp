#pragma once
#include "Motion.hpp"
#include "Visuals.hpp"
namespace heavymach {
// These are local min/max edges, despite the original CGRect field names.
struct ObjectBounds {float left=0,bottom=0,right=0,top=0;};
constexpr ObjectBounds playerBounds(){return {-20,0,20,50};}
constexpr ObjectBounds minimumMachBounds(){return {-20,0,20,40};}
bool pointInObject(Vector2 point,Vector2 worldPosition,ObjectBounds);
ObjectBounds imageBounds(float tileWidth,float tileHeight);
ObjectBounds partsBounds(float tileWidth,float tileHeight,Vector2 socket0);
// 0x428c accumulates from the BASE part argument, not the newly added child.
void accumulateMachBounds(ObjectBounds&,Vector2 baseRotatedPosition,ObjectBounds basePartBounds);
enum class ItemBoxEventKind {Pickup,Remove,TickChildren};
struct ItemBoxHost {
 virtual ~ItemBoxHost()=default;
 virtual int32_t random31()=0;
 // Pickup must synchronously call Player.pickupItem to retain RNG ordering.
 virtual void event(ItemBoxEventKind)=0;
};
struct ItemBoxFrame {
 MotionWorld motion;
 bool hasPlayer=true;
 Vector2 playerPosition{};
 ObjectBounds playerBound=playerBounds();
};
class GobItemBox {
public:
 Vector2 position{},velocity{},velocityTick{};
 bool landed=false,show=true,inScreen=true,removed=false;
 float alpha=1,rotation=0; Vector2 scale{1,1};
 explicit GobItemBox(ItemBoxHost&);
 void tick(const ItemBoxFrame&,ItemBoxHost&);
 DrawSprite draw() const;
};
}
