// Original player HP images are QobBase children, independently camera-tested.
#include "World.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "ItemBox.hpp"
#include "Trail.hpp"
#include <variant>
#include <memory>
#define private public
#include "GameSession.hpp"
#undef private
#include "../Sources/Core/GameSession.cpp"
#include <cassert>
#include <iostream>
using namespace heavymach;
int main(){
 auto data=GameData::loadDirectory(std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources/RawData");
 SaveSlot slot;GlobalInfo(slot).selectSlot(1);
 GameSession session(data,slot,[](std::string_view name){return name=="HPGauge.png"?Vector2{42,10}:Vector2{128,128};},[]{return int32_t(0);});
 session.start(0,0,0);auto& impl=*session.impl_;impl.world.camera={240,160};impl.player->position={100.25f,120.25f};
 auto gauges=[&]{std::vector<DrawSprite> out;for(auto& d:session.sprites())if(d.tile=="HPGauge.png")out.push_back(d);return out;};
 // Registration uses the old fraction; later damage moves the fill before draw.
 impl.world.paused=true;impl.player->position={575,100};impl.playerGraph.gauge=1;
 impl.motionEvents(*impl.player,impl.playerGraph,{{MotionEventKind::TickChildren}});impl.collect();assert(gauges().empty());
 impl.player->setHPGauge(0,impl);impl.collect();assert(gauges().empty());
 impl.motionEvents(*impl.player,impl.playerGraph,{{MotionEventKind::TickChildren}});impl.collect();assert(gauges().size()==1);
 impl.player->setHPGauge(1,impl);impl.collect();assert(gauges().size()==1&&gauges()[0].position.x==575);
 impl.player->position={100.25f,120.25f};
 auto refresh=[&]{impl.motionEvents(*impl.player,impl.playerGraph,{{MotionEventKind::TickChildren}});impl.collect();};
 for(float fraction:{1.f,.5f,0.f}){
  impl.player->setHPGauge(fraction,impl);refresh();auto d=gauges();assert(d.size()==2);
  assert(d[0].frame==1&&d[1].frame==0&&d[0].cols==1&&d[0].rows==2&&d[1].rows==2);
  assert(d[0].z==2&&d[1].z==2&&d[0].blend==0&&d[1].blend==0);
  assert(d[0].position.x==100&&d[0].position.y==180&&d[1].position.y==181);
  assert(d[1].position.x==float(int(100.25f-(21.f-21.f*fraction)+.5f)));
  assert(d[0].scale.x==1&&d[1].scale.x==fraction);
 }
 session.setPaused(true);refresh();assert(gauges().size()==2);
 impl.player->visible=false;refresh();assert(gauges().empty());impl.player->visible=true;
 impl.player->hasHPGauge=false;refresh();assert(gauges().empty());impl.player->hasHPGauge=true;
 // Parent outside the camera does not suppress its independently visible child.
 impl.player->position={100,-55};refresh();assert(!impl.playerGraph.inScreen&&gauges().size()==2);
 // Conversely, the player's body can be visible while both images are above view.
 impl.player->position={100,300};refresh();assert(impl.playerGraph.inScreen&&gauges().empty());
 impl.player->position={100,-63.6f};refresh();auto edge=gauges();
 assert(edge.size()==1&&edge[0].frame==0); // fixedPos trunc(x+.5), each 5px-high child tested separately.
 // Camera offset is applied before fixedPos rounding and culling. Rendering
 // compensates the host's existing -offset world transform without a new collect.
 impl.player->position={100.25f,120.25f};session.setCameraOffset({.75f,.75f});refresh();
 auto shaken=gauges();assert(shaken.size()==2);
 assert(shaken[0].position.x-.75f==100&&shaken[0].position.y-.75f==180);
 // Shift both images just outside the top; pre-quake culling would keep them.
 impl.player->position={100,260};session.setCameraOffset({0,-3.25f});refresh();assert(gauges().empty());
 // Shift a previously invisible image into view before rounding/culling.
 impl.player->position={100,264};session.setCameraOffset({0,4.25f});refresh();assert(gauges().size()==2);
 // Recursive QobBase.posY sums the background before the fill's local Y=1.
 // Reassociating owner+(60+1) changes fixedPos by one pixel here.
 impl.player->position={100,3.499049186706543f};session.setCameraOffset({0,-.000946044921875f});refresh();
 auto recursive=gauges();assert(recursive.size()==2&&recursive[1].position.y+.000946044921875f==65);
 std::cout<<"Player gauge full/half/zero, pause, visibility and child camera bounds passed\n";
}
