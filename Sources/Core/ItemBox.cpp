#include "ItemBox.hpp"
#include <bit>
#include <stdexcept>
namespace heavymach {
bool pointInObject(Vector2 p,Vector2 pos,ObjectBounds b){
 // Preserve original ordered > rejection (including unordered NaN behavior).
 if(pos.x+b.left>p.x)return false;if(p.x>pos.x+b.right)return false;
 if(pos.y+b.bottom>p.y)return false;if(p.y>pos.y+b.top)return false;return true;
}
ObjectBounds imageBounds(float w,float h){return {w*-.5f,h*-.5f,w*.5f,h*.5f};}
ObjectBounds partsBounds(float w,float h,Vector2 socket){auto b=imageBounds(w,h);b.left-=socket.x;b.right-=socket.x;b.bottom-=socket.y;b.top-=socket.y;return b;}
void accumulateMachBounds(ObjectBounds& b,Vector2 p,ObjectBounds part){
 float left=p.x+.5f*part.left;left=b.left<left?b.left:left;b.left=left<=-20||left!=left?left:-20;
 b.bottom=0;
 float right=p.x+.5f*part.right;right=b.right>right?b.right:right;b.right=right>=20||right!=right?right:20;
 float top=p.y+.5f*part.top;top=b.top>top?b.top:top;b.top=top>=40||top!=top?top:40;
}
GobItemBox::GobItemBox(ItemBoxHost& h){
 auto random=[&](){int32_t n=h.random31();if(n<0)throw std::invalid_argument("random31 must be nonnegative");return float(n)*std::bit_cast<float>(0x30000000u);};
 velocity.x=random()*200.f-100.f;velocity.y=random()*100.f+150.f;
}
void GobItemBox::tick(const ItemBoxFrame& f,ItemBoxHost& h){
 if(!show)return;
 if(!f.motion.paused){
  if(!landed){
   velocity.y-=10.f;velocityTick=velocity;velocityTick.mul(std::bit_cast<float>(0x3c888889u));bool hit=false;
   for(int i:f.motion.collisionCandidates){
    if(i<0||size_t(i)>=f.motion.ground.size())continue;
    if(f.motion.ground[i].collision.checkCollideVel(position,velocityTick)){
     if(velocity.y<0){velocity.y*=std::bit_cast<float>(0xbe99999au);if(velocity.y<30)landed=true;}
     hit=true;break;
    }
   }
   if(!hit)position.add(velocityTick);
  }
  if(inScreen){
   if(f.hasPlayer&&pointInObject({position.x,position.y+12.f},f.playerPosition,f.playerBound)){
    h.event(ItemBoxEventKind::Pickup);removed=true;h.event(ItemBoxEventKind::Remove);
   }else if(landed&&f.motion.gameMode==1){
    // ObjC nil myMach/pos/x/y produces zero, including during attract motion.
    Vector2 player=f.hasPlayer?f.playerPosition:Vector2{};
    position.x=position.x+(player.x-position.x)/25.f;
    position.y=position.y+((player.y+24.f)-position.y)/25.f;
   }
  }
 }
 h.event(ItemBoxEventKind::TickChildren);
}
DrawSprite GobItemBox::draw()const{
 DrawSprite s;s.tile="ItemBox.png";s.position=position;s.scale=scale;s.rotation=rotation;s.alpha=alpha;s.z=2;return s;
}
}
