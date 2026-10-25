// QobImage children: register at tick, read position/scale at draw.
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
#include <fstream>
#include <iostream>
using namespace heavymach;
int main(){
 const auto resources=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";
 auto data=GameData::loadDirectory(resources/"RawData");SaveSlot slot;GlobalInfo(slot).selectSlot(1);
 auto dimensions=[&](std::string_view key){
  std::ifstream f(resources/"Images"/key,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);
  if(f.gcount()!=24||b[0]!=137||b[1]!='P')throw std::runtime_error("Missing PNG: "+std::string(key));
  auto be=[&](int n){return uint32_t(b[n])<<24|uint32_t(b[n+1])<<16|uint32_t(b[n+2])<<8|b[n+3];};return Vector2{float(be(16)),float(be(20))};
 };
 GameSession game(data,slot,dimensions,[]{return int32_t(1);});game.start(0,0,0);
 auto& i=*game.impl_;i.enemies.clear();i.world.camera={240,160};i.world.paused=true;
 int failures=0,checks=0;auto check=[&](bool ok,const char* label){++checks;if(!ok){++failures;std::cerr<<"FAIL "<<label<<'\n';}};
 auto gauges=[&]{std::vector<DrawSprite> out;for(const auto& d:game.sprites())if(d.tile=="HPGaugeEnemy.png"||d.tile=="HPGaugeBoss.png")out.push_back(d);return out;};
 for(const char* name:{"ArmoredCar015","MBoss01","Heli001"}){
  i.enemies.clear();i.spawnEnemy({name,{100.25f,120.25f},false});auto& m=i.enemies.front();auto& e=*m.enemy;
  auto refresh=[&]{i.motionEvents(e,m,{{MotionEventKind::TickChildren}});i.collect();};
  game.setCameraOffset({.75f,.75f});
  for(float fraction:{1.f,.5f,0.f}){
   m.gauge=fraction;refresh();auto d=gauges();check(d.size()==2,"two gauge children");if(d.size()!=2)continue;
   check(d[0].frame==1&&d[1].frame==0&&d[0].rows==2&&d[1].rows==2,"background then fill");
   check(d[0].z==2&&d[1].z==2&&d[0].blend==0&&d[1].blend==0,"layer/blend");
   check(d[0].position.x-.75f==100&&d[0].position.y-.75f==float(int(120.25f+e.gaugeY-.75f+.5f)),"fixed background after quake");
   check(d[1].position.x-.75f==float(int(100.25f-(e.gaugeSize-fraction*e.gaugeSize)-.75f+.5f))&&d[1].position.y-d[0].position.y==1,"fixed fill after quake");
   check(d[0].scale.x==1&&d[1].scale.x==fraction,"full half zero scale");
  }
  e.visible=false;refresh();check(gauges().empty(),"hidden parent");e.visible=true;
  e.hasHPGauge=false;refresh();check(gauges().empty(),"missing gauge");e.hasHPGauge=true;
 }
 i.enemies.clear();i.spawnEnemy({"MBoss01",{100,-60},false});auto& m=i.enemies.front();auto& e=*m.enemy;
 m.bounds={-20,0,20,40};game.setCameraOffset({0,0});
 auto refresh=[&]{i.motionEvents(e,m,{{MotionEventKind::TickChildren}});i.collect();};
 refresh();check(!m.inScreen&&gauges().size()==2,"body outside but gauges inside");
 e.position.y=-84.6f;refresh();auto edge=gauges();check(edge.size()==1&&edge[0].frame==0,"background outside but fill inside negative boundary");
 e.position={600,100};m.gauge=1;refresh();check(gauges().empty(),"both outside at registration");
 m.gauge=0;i.collect();check(gauges().empty(),"damage cannot register a previously culled fill");
 refresh();auto zero=gauges();check(zero.size()==1&&zero[0].frame==0&&zero[0].scale.x==0,"zero scale retains unscaled bounds");
 m.gauge=1;i.collect();auto moved=gauges();check(moved.size()==1&&moved[0].frame==0&&moved[0].position.x==600,"registered fill uses new position without reculling");
 e.position={100.25f,120.25f};refresh();i.damage(m.id,e.hp);i.collect();auto dead=gauges();
 check(m.removed&&dead.size()==2&&dead[1].scale.x==0,"same frame death retains registered gauge");
 game.tick(1,0);check(i.enemies.empty()&&gauges().empty(),"next tick removes dead gauge");
 // Exercise actual session tick registration/reset, including a hidden subtree.
 // Settle the camera at its clamped follow target before placing this fixture.
 i.world.camera={i.player->position.x+140,i.player->position.y+70};i.world.processCamera(i.player->position);
 i.spawnEnemy({"MBoss01",{i.world.camera.x,i.world.camera.y},false});auto& live=i.enemies.front();
 game.tick(2,0);check(gauges().size()==2,"session tick registers live children");
 live.enemy->visible=false;game.tick(3,0);check(gauges().empty(),"hidden subtree clears previous registration");
 live.enemy->visible=true;i.collect();check(gauges().empty(),"show without a new tick does not resurrect registration");
 game.tick(4,0);check(gauges().size()==2,"next visible tick registers again");
 // processFly emits fatal Damage before TickChildren. Recursive removal
 // suppresses new child registration, unlike a later bullet death above.
 i.enemies.clear();i.world.camera={240,160};i.spawnEnemy({"MBoss01",{100,100},false});auto& falling=i.enemies.front();
 i.motionEvents(*falling.enemy,falling,{{MotionEventKind::Damage,falling.enemy->hp},{MotionEventKind::TickChildren}});i.collect();
 check(falling.removed&&gauges().empty(),"fatal damage before child tick suppresses registration");
 std::cout<<checks<<" checks, "<<failures<<" failures\n";return failures?1:0;
}
