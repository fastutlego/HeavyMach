// Compile this unchanged against the pre-auto-aim revision and current sources.
// Output is deterministic and contains no pointers or wall-clock measurements.
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
using namespace heavymach;
int main(int argc,char** argv){
 const std::filesystem::path root=argc>1?argv[1]:".";
 auto data=GameData::loadDirectory(root/"Resources/RawData");
 auto dims=[&](std::string_view name){std::ifstream f(root/"Resources/Images"/name,std::ios::binary);unsigned char b[24]{};f.read((char*)b,24);if(f.gcount()!=24)return Vector2{40,12};auto be=[&](int n){return uint32_t(b[n])<<24|uint32_t(b[n+1])<<16|uint32_t(b[n+2])<<8|b[n+3];};return Vector2{float(be(16)),float(be(20))};};
 std::cout<<std::hexfloat;
 for(auto& [name,info]:data.weapons)if(name.starts_with("P_")&&data.parts.contains(name))for(uint32_t initial:{1u,12345u,987654u}){
  SaveSlot slot;GlobalInfo(slot).selectSlot(0);slot.bytes[0x2f]=4;GlobalInfo(slot).addWeaponItem(name,100);
  uint32_t rng=initial;GameSession game(data,slot,dims,[&]{rng=rng*1664525+1013904223;return int32_t(rng>>1);});game.start(0,0,0);
  auto& i=*game.impl_;i.enemies.clear();i.world.enemiesSpawned.assign(i.world.map.enemies.size(),true);i.world.paused=false;i.world.gameState=0;
  i.player->hp=100000;i.player->position={350,100};game.selectWeapon(int(slot.weaponCount())-1);i.playerPart(2,name);
  i.spawnEnemy({"Heli001",{650,200},false});i.enemies.back().enemy->hp=i.enemies.back().enemy->hpMax=100000;
  std::cout<<"WEAPON "<<name<<" SEED "<<initial<<'\n';
  size_t observed=0;
  for(int tick=1;tick<=420;++tick){
   if(tick==5||tick==140||tick==280)game.touch({460,float(tick==140?250:180)},0);
   if(tick==120||tick==260||tick==400)game.touch({460,180},2);
   if(tick==70)game.booster();
   game.tick(tick/60.,tick<180?.02f:-.01f);
   auto& w=i.playerGraph.parts[2]->weapon;
   std::cout<<tick<<' '<<rng<<' '<<i.player->position.x<<' '<<i.player->position.y<<' '<<i.player->hp<<' '<<i.charge<<' '<<w.aimDir<<' '<<w.aimDirDest<<' '<<w.fireTime<<' '<<w.fireCount<<' '<<w.isFire<<' '<<w.isReload;
   for(auto& n:i.dynamic)if(auto b=std::get_if<BulletState>(&n.value)){
    ++observed;std::cout<<" B "<<b->toPlayer<<' '<<b->pos.x<<' '<<b->pos.y<<' '<<b->vel.x<<' '<<b->vel.y<<' '<<b->rotate<<' '<<b->alpha<<' '<<b->removed;
   }
   for(auto& m:i.enemies)std::cout<<" E "<<m.id<<' '<<m.enemy->position.x<<' '<<m.enemy->position.y<<' '<<m.enemy->hp;
   for(auto& e:game.takeEvents())std::cout<<" V "<<int(e.kind)<<' '<<e.code<<' '<<e.index<<' '<<e.amount<<' '<<e.value<<' '<<e.text;
   std::cout<<'\n';
  }
  assert(observed>0);std::cout<<"SAVE ";for(auto byte:slot.bytes)std::cout<<int(byte)<<',';std::cout<<'\n';
 }
}
