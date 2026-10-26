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
#include <fstream>
#include <iostream>
#include <chrono>
using namespace heavymach;
int main(int argc,char** argv){
 const std::filesystem::path root=argc>1?argv[1]:".";
 GameData data=GameData::loadDirectory(root/"Resources/RawData");
 auto dims=[&](std::string_view name){std::ifstream f(root/"Resources/Images"/name,std::ios::binary);unsigned char b[24]{};f.read((char*)b,24);if(f.gcount()!=24)return Vector2{40,12};auto be=[&](int n){return uint32_t(b[n])<<24|uint32_t(b[n+1])<<16|uint32_t(b[n+2])<<8|b[n+3];};return Vector2{float(be(16)),float(be(20))};};
 SaveSlot slot;GlobalInfo(slot).selectSlot(0);slot.bytes[0x2f]=4;uint32_t seed=1;GameSession game(data,slot,dims,[&]{seed=seed*1664525+1013904223;return int32_t(seed>>1);});game.start(0,0,0);auto& i=*game.impl_;
 i.enemies.clear();i.world.enemiesSpawned.assign(i.world.map.enemies.size(),true);i.world.ground.clear();i.world.bins.clear();i.world.paused=false;i.world.gameState=0;i.player->position={200,120};
 i.spawnEnemy({"Heli001",{400,180},false});auto first=i.enemies.back().id;
 i.spawnEnemy({"Heli001",{600,220},false});auto second=i.enemies.back().id;
 i.spawnEnemy({"Heli001",{100,160},false});
 assert(game.aimMode()==AimMode::manual);game.setAimMode(AimMode::automatic);i.updateAutoAim();assert(game.aimSnapshot().target==first);
 auto rng=seed;for(int n=0;n<120;++n)i.updateAutoAim();assert(seed==rng); // prediction is read-only, no RNG consumption
 game.cycleAimTarget();i.updateAutoAim();assert(game.aimSnapshot().target==second);
 i.findAimTarget(second)->enemy->position.x=3000;i.updateAutoAim();assert(game.aimSnapshot().target==second); // offscreen lock
 i.findAimTarget(second)->enemy->position.x=199;i.updateAutoAim();assert(game.aimSnapshot().target==first);
 i.findAimTarget(first)->enemy->hp=0;i.updateAutoAim();assert(game.aimSnapshot().target==0);
 i.enemies.clear();i.spawnEnemy({"Heli001",{400,180},false});game.cycleAimTarget();i.updateAutoAim();assert(game.aimSnapshot().target);game.cycleAimTarget();i.updateAutoAim();assert(!game.aimSnapshot().target);game.cycleAimTarget();i.updateAutoAim();assert(game.aimSnapshot().target);
 game.setAutoFire(true);game.setAimMode(AimMode::manual);assert(!i.autoFire&&!i.playerGraph.parts[2]->weapon.isFire);
 game.setAimMode(AimMode::off);game.touch({470,270},0);assert(!i.playerGraph.parts[2]->weapon.isFire);game.setAutoFire(true);assert(!i.autoFire);
 // Three targets cycle without alternating forever between the nearest two.
 i.enemies.clear();i.spawnEnemy({"Heli001",{400,180},false});auto a=i.enemies.back().id;
 i.spawnEnemy({"Heli001",{500,180},false});auto b=i.enemies.back().id;
 i.spawnEnemy({"Heli001",{600,180},false});auto c=i.enemies.back().id;
 game.setAimMode(AimMode::automatic);i.updateAutoAim();assert(game.aimSnapshot().target==a);
 game.cycleAimTarget();i.updateAutoAim();assert(game.aimSnapshot().target==b);
 game.cycleAimTarget();i.updateAutoAim();assert(game.aimSnapshot().target==c);
 game.cycleAimTarget();i.updateAutoAim();assert(game.aimSnapshot().target==a);
 // Try the high ballistic arc when the low arc crosses terrain. A tall wall
 // blocks both, without spawning a shot or bypassing collision.
 i.world.ground.resize(1);i.world.ground[0].collision=ColVector({350,180},{350,-1000});i.world.bins={{0}};
 auto cannon=data.weapons.at("P_Wpn_Canon01");auto low=solveAim({200,100},{500,100},{},cannon.spd,cannon.g,cannon.liveTm);
 assert(low&&!i.clearAimPath({200,100},low,cannon));auto high=i.aimPath({200,100},{500,100},{},cannon);
 assert(high.ticks>low.ticks&&i.clearAimPath({200,100},high,cannon));
 i.world.ground[0].collision=ColVector({350,1000},{350,-1000});assert(!i.clearAimPath({200,100},i.aimPath({200,100},{500,100},{},cannon),cannon));
 i.world.ground.clear();i.world.bins.clear();
 // Each original weapon fires from its actual muzzle/support origin and travels
 // to an actual target. No spawn-at-target or forced targetAt hit is used.
 for(bool correction:{false,true}){game.setAimCorrectionEnabled(correction);int cases=0,blocked=0,missed=0;
 for(auto& [name,info]:data.weapons)if(name.starts_with("P_")&&data.parts.contains(name))for(int moving=0;moving<2;++moving)for(int height=0;height<3;++height)for(float distance:{160.f,800.f}){
  i.enemies.clear();i.dynamic.clear();i.spawnEnemy({"Heli001",{200+distance,160},false});auto& target=i.enemies.back();target.enemy->hp=target.enemy->hpMax=100000;target.inScreen=distance<480;target.aimStep={moving?.8f:0,moving?.15f:0};
  i.world.gameTime=0;i.player->position={200,float(60+height*60)};i.playerPart(2,name);auto& w=i.playerGraph.parts[2]->weapon;game.setAimMode(AimMode::off);game.setAimMode(AimMode::automatic);i.updateAutoAim();
  if(game.aimSnapshot().flightTicks==0){game.setAutoFire(true);i.updateAutoAim();assert(!w.isFire);++blocked;continue;}w.aimDir=w.aimDirDest;game.setAutoFire(true);
  int hp=target.enemy->hp;bool shot=false;int saves=0;
  for(int n=1;n<=400&&target.enemy->hp==hp;++n){i.world.gameTime=double(n)/60;i.globalTime=i.world.gameTime;
   target.enemy->position.x+=target.aimStep.x;target.enemy->position.y+=target.aimStep.y;
   i.updateAutoAim();w.aimDir=w.aimDirDest;tickWeapon(w,i.frame(*i.player,i.playerGraph,*i.playerGraph.parts[2]),i);
   for(auto& node:i.dynamic)if(auto b=std::get_if<BulletState>(&node.value)){if(b->removed)continue;shot=true;i.guideBullet(node,*b);i.assistedBullet=&node;CombatFrame f;f.time=i.world.gameTime;f.playerPos=i.player->position;tickBullet(*b,f,i);i.assistedBullet=nullptr;if(node.aimTicks==0)node.aimTarget=0;}
   for(auto& e:game.takeEvents())if(e.kind==SessionEventKind::PersistSave)++saves;
  }
  if(!(shot&&target.enemy->hp<hp)){if(!correction){++missed;continue;}std::cerr<<name<<" moving="<<moving<<" shots="<<game.aimSnapshot().assistedShots<<" status="<<int(game.aimSnapshot().status)<<" FAIL\n";return 1;}
  assert(saves==0);++cases;std::cout<<name<<" moving="<<moving<<" damage="<<hp-target.enemy->hp<<" PASS\n";
 }
 assert(cases+blocked+missed==240);assert(blocked>0);std::cout<<"AIM_COMPARE correction="<<correction<<" hit_conditions="<<cases<<" unreachable="<<blocked<<" miss_conditions="<<missed<<" total=240\n";
 }
 // Compare every tracked projectile, including full burst completion and
 // flight drain, against a live hitbox whose direction changes after firing.
 for(bool correction:{false,true})for(auto& [name,info]:data.weapons)if(name.starts_with("P_")&&data.parts.contains(name)){
  i.enemies.clear();i.dynamic.clear();i.world.ground.clear();i.world.bins.clear();i.world.gameTime=0;
  i.spawnEnemy({"Heli001",{400,180},false});auto& target=i.enemies.back();target.enemy->hp=target.enemy->hpMax=1000000;
  i.player->position={200,120};i.playerPart(2,name);auto& w=i.playerGraph.parts[2]->weapon;
  game.setAimMode(AimMode::off);game.setAimMode(AimMode::automatic);game.setAimCorrectionEnabled(correction);
  for(int n=1;n<=720;++n){i.world.gameTime=n/60.;i.globalTime=i.world.gameTime;
   target.aimStep={n%60<30?2.f:-2.f,n%80<40?.8f:-.8f};target.enemy->position.x+=target.aimStep.x;target.enemy->position.y+=target.aimStep.y;
   game.setAutoFire(n<120);i.updateAutoAim();w.aimDir=w.aimDirDest;tickWeapon(w,i.frame(*i.player,i.playerGraph,*i.playerGraph.parts[2]),i);
   for(auto& node:i.dynamic)if(auto bullet=std::get_if<BulletState>(&node.value)){if(bullet->removed)continue;i.guideBullet(node,*bullet);i.assistedBullet=&node;CombatFrame f;f.time=i.world.gameTime;f.playerPos=i.player->position;tickBullet(*bullet,f,i);i.assistedBullet=nullptr;if(node.aimTicks==0)node.aimTarget=0;}
   game.takeEvents();
  }
  auto result=game.aimSnapshot();assert(result.assistedShots>0);if(correction)assert(result.targetHits==result.assistedShots);
  std::cout<<"AIM_PROJECTILES weapon="<<name<<" correction="<<correction<<" tracked="<<result.assistedShots<<" hit="<<result.targetHits<<"\n";
 }
 i.enemies.clear();i.dynamic.clear();i.spawnEnemy({"Heli001",{600,180},false});i.player->position={200,120};i.playerPart(2,"P_Wpn_Canon01");game.setAimMode(AimMode::automatic);
 std::vector<double> times;times.reserve(10000);for(int n=0;n<10000;++n){auto begin=std::chrono::steady_clock::now();i.updateAutoAim();times.push_back(std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-begin).count());}
 std::sort(times.begin(),times.end());std::cout<<"AIM_SOLVER host_debug p95_us="<<times[9499]<<" max_us="<<times.back()<<" samples=10000\n";
 game.start(0,0,0);assert(game.aimSnapshot().target==0&&!i.autoFire);
}
