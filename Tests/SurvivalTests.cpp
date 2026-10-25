#include "../Sources/Core/GameSession.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace heavymach;
struct WorldRecorder:WorldHost {
 std::vector<EnemyDefinition> spawned;std::vector<WorldEvent> events;std::function<void()> constructor;
 Vector2 tileSize(std::string_view)override{return {100,100};}
 void spawnMapObject(const MapObjectDefinition&,int)override{}
 void spawnPlayer(Vector2)override{}
 void spawnEnemy(const EnemyDefinition& e)override{spawned.push_back(e);if(constructor)constructor();}
 void event(const WorldEvent& e)override{events.push_back(e);}
};
struct Sequence {
 std::vector<int32_t> values;size_t calls=0;
 int32_t next(){assert(calls<values.size());return values[calls++];}
};
struct EnemyRecorder:EnemyHost {
 int32_t experience=-1;uint32_t seed=13;
 int32_t random31()override{seed=seed*1664525u+1013904223u;return int32_t(seed>>1);}
 bool acquireParticle()override{return false;}
 void spawnItemBox(Vector2)override{random31();random31();}
 void event(const EnemyEvent& e)override{if(e.kind==EnemyEventKind::AddExperience)experience=e.amount;}
};
int main(){
 auto resources=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";
 auto data=GameData::loadDirectory(resources/"RawData");const auto& map=data.maps.at("Survival");
 WorldRecorder h;WorldState world;world.openMap(map,1,h);world.configureSurvival(0);h.spawned.clear();
 assert(world.playerSpawn.x==map.right-480&&world.survivalRegenSeed==400);
 Sequence noSpawn{{1}};world.processSurvival([&]{return noSpawn.next();},h);assert(noSpawn.calls==1&&h.spawned.empty());
 world.bossEncounter=1;Sequence bossGate{{400}};world.processSurvival([&]{return bossGate.next();},h);assert(bossGate.calls==1&&h.spawned.empty());world.bossEncounter=0;
 // Constructor draws occur between category/name and the following boss decision.
 Sequence sync{{0,0,314,41,42,0,9,43,44,0}};std::vector<int> constructorValues;
 h.constructor=[&]{constructorValues.push_back(sync.next());constructorValues.push_back(sync.next());};
 world.processSurvival([&]{return sync.next();},h);
 assert(sync.calls==10&&constructorValues==std::vector<int>({41,42,43,44}));
 assert(h.spawned.size()==2&&h.spawned[0].name=="ArmoredCar315"&&!h.spawned[0].isBoss);
 assert(h.spawned[1].name=="MBoss10"&&h.spawned[1].isBoss&&world.bossEncounter==1&&world.survivalRegenSeed==399);
 h.constructor={};
 const char* names[]={"ArmoredCar315","Tank200","Truck010","Heli010"};int last[]={314,199,9,9};
 for(int category=0;category<4;++category){
  world.bossEncounter=0;h.spawned.clear();Sequence q{{0,category,last[category],1,1,1,1}};
  world.processSurvival([&]{return q.next();},h);assert(q.calls==7&&h.spawned.size()==1&&h.spawned[0].name==names[category]);
  float ground=world.mapHeight(map.right+30);float y=ground==10000?50:ground+1;
  assert(h.spawned[0].position.x==map.right+30&&h.spawned[0].position.y==y+(category==3?200:0));
 }
 h.spawned.clear();Sequence fullBoss{{0,1,0,1,0,5,1}};world.processSurvival([&]{return fullBoss.next();},h);
 assert(fullBoss.calls==7&&h.spawned.size()==2&&h.spawned[0].name=="Tank001"&&h.spawned[1].name=="Boss06"&&h.spawned[1].isBoss);
 world.bossEncounter=0;world.survivalRegenSeed=51;h.spawned.clear();Sequence heli{{0,2,0,1,1,0,0}};world.processSurvival([&]{return heli.next();},h);
 assert(heli.calls==7&&h.spawned.size()==2&&h.spawned[0].name=="Truck001"&&h.spawned[1].name=="Heli001"&&!h.spawned[1].isBoss&&world.survivalRegenSeed==50);
 Sequence floor{{0,0,0,1,1,1}};world.processSurvival([&]{return floor.next();},h);assert(floor.calls==6&&world.survivalRegenSeed==50&&h.spawned.back().name=="ArmoredCar001");
 int draws=0;world.paused=true;double before=world.gameTime;world.tick(world.playerSpawn,{4,4},h,[&]{++draws;return 1;});assert(draws==0&&world.gameTime==before);
 world.paused=false;world.show=false;world.tick(world.playerSpawn,{4,4},h,[&]{++draws;return 1;});assert(draws==0&&world.gameTime==before);
 // Original enemy definition, ordinary damage callback. No definition/stat mutation.
 EnemyRecorder eh;GobHvM_Enemy enemy;const auto& definition=data.machines.at("ArmoredCar001");enemy.makeMachFromDefinition(definition,eh);
 EnemyFrame frame;frame.motion.gameMode=1;enemy.onDamage(enemy.hp,frame,eh);assert(eh.experience==definition.giveExp/2);
 SaveSlot slot;GlobalInfo(slot).selectSlot(1);auto mission=slot.i32(0xc),stage=slot.i32(0x10),farthest=slot.i32(0x14);
 auto dimensions=[&](std::string_view name){auto p=resources/"Images"/name;if(!std::filesystem::exists(p))p+=".png";std::ifstream f(p,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);assert(f.gcount()==24);auto be=[&](int i){return uint32_t(b[i])<<24|uint32_t(b[i+1])<<16|uint32_t(b[i+2])<<8|b[i+3];};return Vector2{float(be(16)),float(be(20))};};
 uint32_t seed=17;size_t calls=0;auto random=[&]{++calls;seed=seed*1664525u+1013904223u;return int32_t(seed>>1);};
 GameSession game(data,slot,dimensions,random);game.start(1,4,4);assert(game.world().gameMode==1&&game.world().playerSpawn.x==map.right-480);assert(game.world().survivalRegenSeed==400);
 game.takeEvents();game.setPaused(true);size_t pausedCalls=calls;game.tick(1,0);assert(calls==pausedCalls&&game.world().gameTime==0);game.setPaused(false);
 bool spawned=false,died=false,returned=false;int deathTick=0;
 // Actual unchanged Survival enemies attack a stationary original grade0 player.
 for(int tick=1;tick<=36000&&!returned;++tick){game.tick(double(tick)/60,0);spawned|=game.enemyCount()>0;
  if(!died&&game.player()->hp==0){died=true;deathTick=tick;assert(game.world().gameState==3&&game.world().paused&&game.world().presentation.typoFrame==3);}
  for(const auto& e:game.takeEvents())if(e.kind==SessionEventKind::WorldUI&&e.code==int(WorldEventKind::ChangeScreen)){assert(e.index==3);returned=true;}
 }
 assert(spawned&&died&&returned);assert(slot.i32(0xc)==mission&&slot.i32(0x10)==stage&&slot.i32(0x14)==farthest);
 std::cout<<"Survival branches passed; actual mode1 spawn/death/return tick="<<deathTick<<" halfEXP="<<eh.experience<<"\n";
}
