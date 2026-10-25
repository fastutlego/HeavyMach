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
#include <fstream>
using namespace heavymach;
int main(){
 auto data=GameData::loadDirectory(std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources/RawData");
 SaveSlot slot;GlobalInfo(slot).selectSlot(1);int calls=0;
 GameSession game(data,slot,[](std::string_view){return Vector2{128,128};},[&]{++calls;return int32_t(1);});
 game.start(0,0,0);auto& i=*game.impl_;i.world.camera={240,160};
 BulletState bullet;bullet.pos={100,.25f};game.setCameraOffset({0,1});assert(!i.inCamera(bullet));
 game.setCameraOffset({0,0});assert(i.inCamera(bullet));
 bullet.pos={-.25f,100};game.setCameraOffset({-1,0});assert(i.inCamera(bullet));
 bullet.pos={540.25f,100};game.setCameraOffset({1,0});assert(i.inCamera(bullet));
 // Quake changes cached enemy screen status before child weapon AI and targeting.
 i.enemies.clear();EnemyDefinition enemy;enemy.name="ArmoredCar015";enemy.position={100,.25f};i.spawnEnemy(enemy);
 auto& m=i.enemies.front();m.bounds={};i.player->position={0,100};
 for(auto& p:m.parts)if(p->armed){p->weapon.aiStep=1;p->weapon.resetTime=-100;p->weapon.delayTime=0;}
 game.setCameraOffset({0,1});int before=calls;i.motionEvents(*m.enemy,m,{{MotionEventKind::TickChildren}});
 assert(!m.inScreen&&calls==before&&!i.targetAt(false,enemy.position));
 game.setCameraOffset({0,0});i.motionEvents(*m.enemy,m,{{MotionEventKind::TickChildren}});
 assert(m.inScreen&&calls>before&&i.targetAt(false,enemy.position)==m.id);
 // Player target containment remains a world-space test with no screen gate.
 i.player->position={100,100};game.setCameraOffset({0,10000});assert(i.targetAt(true,{100,100})==1);
 // Touch deltas use the quake-adjusted screen anchor, without altering world positions.
 i.world.camera={240,160};game.setCameraOffset({3,7});auto& weapon=*i.playerGraph.parts[2];weapon.visual.rotatedPosition={0,0};
 game.touch({297,93},0);assert(weapon.weapon.aimDirDest==0); // dx200,dy0 after quake.
 // Fixed map images subtract quake before integer conversion and add it back
 // solely to compensate the existing host-wide camera translation.
 i.mapSprites.clear();DrawSprite map;map.tile="CameraFixture.png";map.position={100.25f,120.25f};i.mapSprites.push_back(map);
 game.setCameraOffset({.75f,.75f});i.collect();bool seen=false;
 for(auto& d:game.sprites())if(d.tile==map.tile){seen=true;assert(d.position.x-.75f==100&&d.position.y-.75f==120);}assert(seen);
 i.mapSprites[0].position={100,384.25f};game.setCameraOffset({0,1});i.collect();seen=false;
 for(auto& d:game.sprites())seen|=d.tile==map.tile;assert(seen); // Rounded383, bottom319.
 // Items consume cached visibility BEFORE base tick refresh, not the new quake gate.
 i.enemies.clear();i.dynamic.clear();i.world.gameMode=1;i.spawnItemBox({200,100});
 auto& box=std::get<GobItemBox>(i.dynamic.back().value);box.landed=true;box.inScreen=true;
 game.setCameraOffset({0,10000});float x=box.position.x;game.tick(1,0);assert(box.position.x<x&&!box.inScreen);
 x=box.position.x;game.tick(1+1./60,0);assert(box.position.x==x);
 // processStageClear changes camObject before processCamera in0xb658.
 game.start(0,0,0);i.world.camera={1000,500};i.world.paused=true;
 i.world.gameState=1;i.world.presentation.step=3;
 game.tick(2,0);assert(!game.player());
 assert(i.world.camera.x==1000&&i.world.camera.y==500); // Cleared target is not followed.
 i.world.presentation.step=5;
 game.tick(2+1./60,0);assert(game.player());
 WorldState expected;expected.map=i.world.map;expected.backLine=i.world.backLine;expected.frontLine=i.world.frontLine;
 expected.camera={1000,500};expected.processCamera(i.world.playerSpawn);
 assert(i.world.camera.x==expected.camera.x&&i.world.camera.y==expected.camera.y);
 // Trail removal precedes QobBase visual registration in0x1b5c0/0x7830.
 i.dynamic.clear();auto fading=std::make_shared<GobTrailEffect>();
 fading->configure(4,10,4,.1f,.8f);fading->wind=fading->diffuse=false;fading->tile="TrailFixture.png";
 fading->addTrailPos({0,0},0,i);fading->addTrailPos({25,0},0,i);
 i.dynamic.emplace_back(fading);i.collect();
 auto meshes=[&]{return std::count_if(game.renderItems().begin(),game.renderItems().end(),[](const auto& item){return std::holds_alternative<TrailMesh>(item);});};
 assert(meshes()==1);fading->tick(1,false,i);assert(fading->removed);
 i.collect();assert(meshes()==0);
 // Only background/map object dimensions and sprites use World ownership.
 auto scopedData=data;auto& scopedMap=scopedData.maps.at("Stage01");
 scopedMap.background="ScopedBackground.png";scopedMap.objects.clear();
 MapObjectDefinition scopedObject;scopedObject.name="ScopedObject";scopedObject.position={scopedMap.left+150,150};scopedObject.layer=0;
 scopedMap.objects.push_back(scopedObject);
 SaveSlot scopedSlot;GlobalInfo(scopedSlot).selectSlot(1);int sharedQueries=0,worldQueries=0;
 GameSession scoped(scopedData,scopedSlot,[&](std::string_view name){assert(!name.starts_with("Scoped"));++sharedQueries;return Vector2{128,128};},[]{return 1;},nullptr,
  [&](std::string_view name){assert(name=="ScopedBackground.png"||name=="ScopedObject.png");++worldQueries;return Vector2{128,128};});
 scoped.restoreWorldCamera({scopedMap.left+240,160}); // Place scope fixture inside the view.
 scoped.start(0,0,0);scoped.tick(1./60,0);
 bool worldBackground=false,worldObject=false,sharedSprite=false;
 for(const auto& sprite:scoped.sprites()){
  if(sprite.tile=="ScopedBackground.png"){worldBackground=true;assert(sprite.worldTexture);}
  else if(sprite.tile=="ScopedObject.png"){worldObject=true;assert(sprite.worldTexture);}
  else {sharedSprite=true;assert(!sprite.worldTexture);}
 }
 assert(sharedQueries>0&&worldQueries>=3&&worldBackground&&worldObject&&sharedSprite);

 // Render, weapon AI and collision share the expanded visibility gate.
 game.start(0,0,0);i.enemies.clear();i.dynamic.clear();i.world.camera={240,160};
 game.setCameraOffset({0,0});enemy.position={580,100};i.spawnEnemy(enemy);
 auto& edge=i.enemies.front();edge.bounds=imageBounds(20,20);i.player->position={100,100};
 for(auto& p:edge.parts)if(p->armed){p->weapon.aiStep=1;p->weapon.resetTime=-100;p->weapon.delayTime=0;}
 i.motionEvents(*edge.enemy,edge,{{MotionEventKind::TickChildren}});
 assert(!edge.inScreen&&!i.targetAt(false,enemy.position));
 game.setCanvasWidth(696);assert(game.canvasWidth()==696);
 before=calls;i.motionEvents(*edge.enemy,edge,{{MotionEventKind::TickChildren}});
 assert(edge.inScreen&&calls>before&&i.targetAt(false,enemy.position)==edge.id);
 i.collect();bool edgeBody=false;
 for(auto& sprite:game.sprites())if(!sprite.worldTexture&&sprite.position.x>696-80&&sprite.position.y<160)edgeBody=true;
 assert(edgeBody);
 // A real projectile at that newly visible position follows the normal damage path.
 BulletState edgeBullet;edgeBullet.info=&data.weapons.at("P_Wpn_Gun01");edgeBullet.pos=enemy.position;
 CombatFrame edgeFrame;edgeFrame.playerPos=i.player->position;int edgeHP=edge.enemy->hp;
 tickBullet(edgeBullet,edgeFrame,i);assert(edge.enemy->hp<edgeHP);

 // Extended touches retain the original 100/60 aiming thresholds and world deltas.
 auto& wideWeapon=*i.playerGraph.parts[2];wideWeapon.visual.rotatedPosition={0,0};
 auto anchor=i.cameraPoint(i.player->position);game.touch({anchor.x+300,anchor.y},0);
 assert(wideWeapon.weapon.aimDirDest==0);game.touch({800,anchor.y},2);assert(!wideWeapon.weapon.isFire);
 game.start(0,0,1);assert(game.canvasWidth()==696); // Stage/menu replacement keeps the chosen extent.
 // Rear map cleanup keeps all terrain intersecting the wider left edge,
 // while still advancing the original physical backLine.
 i.world.gameMode=0;i.world.paused=true;i.world.camera={1000,160};i.player->position={1000,100};
 i.safeBackLine=0;i.removeBgLine=700;i.mapSprites.clear();map.position={560,100};i.mapSprites.push_back(map);
 i.tick(3,0); // safeBackLine 760 has not crossed the original 320-unit cleanup quantum.
 assert(i.mapSprites.size()==1);
 i.player->position.x=1400;i.world.camera={600,160};i.world.backLine=0;i.world.frontLine=5000;i.tick(3.1,0);
 assert(i.world.backLine==700&&i.mapSprites.size()==1); // Original progression is independent of retention.
 game.setCanvasWidth(480);assert(game.canvasWidth()==480);

 // Visible prefetched bosses fight normally, but do not unlock another boss
 // if destroyed before their ORIGINAL map placement reaches the 500 trigger.
 auto encounterData=data;auto& encounterMap=encounterData.maps.at("Stage01");
 encounterMap.left=0;encounterMap.right=3000;encounterMap.bottom=-160;encounterMap.top=640;
 encounterMap.objects.clear();encounterMap.enemies={{"MBoss01",{430,100},true},{"Boss07",{600,100},true}};
 SaveSlot encounterSlot;GlobalInfo(encounterSlot).selectSlot(1);
 GameSession encounters(encounterData,encounterSlot,[](std::string_view){return Vector2{128,128};},[]{return 1;});
 encounters.setCanvasWidth(696);encounters.start(0,0,0);auto& ei=*encounters.impl_;
 assert(ei.enemies.size()==2&&ei.world.bossEncounter==1&&ei.world.frontLine==580);
 auto pending=std::find_if(ei.enemies.begin(),ei.enemies.end(),[](auto& e){return e.mapEnemyIndex==1;});
 assert(pending!=ei.enemies.end()&&!ei.world.bossEncounterActivated[1]);
 pending->enemy->position.x=1000; // A moving boss does not move the encounter threshold.
 ei.world.processRegen(100,ei);assert(!ei.world.bossEncounterActivated[1]); // Strictly less than 500.
 ei.damage(pending->id,pending->enemy->hp);
 assert(ei.world.mapEnemiesDefeated[1]&&ei.world.bossEncounter==1&&ei.world.frontLine==580);
 ei.world.processRegen(111,ei);assert(!ei.world.bossEncounterActivated[1]&&ei.world.bossEncounter==1);
 // The surviving active boss still owns the lock, and releases it on death.
 auto active=std::find_if(ei.enemies.begin(),ei.enemies.end(),[](auto& e){return e.mapEnemyIndex==0;});
 ei.damage(active->id,active->enemy->hp);assert(ei.world.bossEncounter==0&&ei.world.frontLine==3010);
 encounters.start(0,0,0);pending=std::find_if(ei.enemies.begin(),ei.enemies.end(),[](auto& e){return e.mapEnemyIndex==1;});
 pending->enemy->position.x=2000;ei.world.processRegen(100,ei);assert(ei.world.bossEncounter==1);
 ei.world.processRegen(111,ei);
 assert(ei.world.bossEncounter==2&&ei.world.bossEncounterActivated[1]&&ei.world.frontLine==750);
 // Mode changes retain pending encounter ownership instead of losing its lock.
 encounters.start(0,0,0);encounters.setCanvasWidth(480);ei.world.processRegen(111,ei);
 assert(ei.world.bossEncounter==2&&ei.world.frontLine==750);


 // Original 480 spawns keep the original unmapped death callback, including
 // two projectiles finding the same removed boss during one original frame.
 encounters.setCanvasWidth(480);encounters.start(0,0,0);
 assert(ei.enemies.size()==1&&ei.enemies.front().mapEnemyIndex==SIZE_MAX);
 auto& originalBoss=ei.enemies.front();originalBoss.enemy->hp=1;
 ei.world.bins.clear(); // Isolate target collision from the fixture's old terrain.
 originalBoss.bounds=imageBounds(20,20);ei.world.camera={240,160};
 BulletState firstHit;firstHit.info=&data.weapons.at("P_Wpn_Gun01");firstHit.pos=originalBoss.enemy->position;
 auto secondHit=firstHit;CombatFrame hits;hits.playerPos=ei.player->position;
 tickBullet(firstHit,hits,ei);assert(ei.world.bossCount==-1);
 tickBullet(secondHit,hits,ei);assert(ei.world.bossCount==-2);
 // Audit the actual registered bounds of all 155 referenced enemy models,
 // including all 48 boss placements across the 42 referenced maps.
 auto actualDimensions=[](std::string_view name){
  auto path=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources/Images"/name;
  std::ifstream file(path,std::ios::binary);unsigned char bytes[24]{};file.read(reinterpret_cast<char*>(bytes),24);assert(file.gcount()==24);
  auto be=[&](int p){return uint32_t(bytes[p])<<24|uint32_t(bytes[p+1])<<16|uint32_t(bytes[p+2])<<8|bytes[p+3];};
  return Vector2{float(be(16)),float(be(20))};
 };
 SaveSlot auditSlot;GlobalInfo(auditSlot).selectSlot(1);GameSession models(data,auditSlot,actualDimensions,[]{return 1;});
 std::unordered_map<std::string,ObjectBounds> actualBounds;std::set<std::string> maps{"Survival"};
 for(int mission=0;mission<8;++mission)for(int stage=0;stage<GlobalInfo::stageCount(mission);++stage){char key[32];std::snprintf(key,sizeof(key),"Stage%02d",GlobalInfo::mapNumber({mission,stage}));maps.insert(key);}
 int bossPlacements=0;
 for(auto& key:maps)for(auto& enemy:data.maps.at(key).enemies){
  bossPlacements+=enemy.isBoss;if(actualBounds.contains(enemy.name))continue;
  models.impl_->spawnEnemy(enemy);auto bounds=models.impl_->enemies.back().bounds;
  assert(bounds.left>=-128);actualBounds.emplace(enemy.name,bounds);
 }
 assert(actualBounds.size()==155&&bossPlacements==48&&maps.size()==42);
 struct AuditHost:WorldHost {
  Vector2 tileSize(std::string_view)override{return {900,400};}
  void spawnMapObject(const MapObjectDefinition&,int)override{}
  void spawnEnemy(const EnemyDefinition&)override{}
  void spawnPlayer(Vector2)override{}
  void event(const WorldEvent&)override{}
 };
 for(float width:{695.f,696.f})for(auto& key:maps){
  const auto& map=data.maps.at(key);AuditHost host;WorldState audit;audit.setCanvasWidth(width);
  audit.camera={10000,160};audit.openMap(map,0,host);float safe=-32000,removed=-32000;
  for(float x=audit.playerSpawn.x;x<=map.right;++x){
   float y=audit.mapHeight(x);if(y==10000)y=100;
   audit.tick(Vector2{x,y+10},{0,0},host,[]{return 1;});
   for(size_t index=0;index<map.enemies.size();++index){const auto& enemy=map.enemies[index];if(!enemy.isBoss||audit.enemiesSpawned[index])continue;
    auto bounds=actualBounds.at(enemy.name);
    bool horizontal=enemy.position.x+bounds.left<=audit.camera.x+width*.5f&&enemy.position.x+bounds.right>=audit.camera.x-width*.5f;
    bool vertical=enemy.position.y+bounds.top>=audit.camera.y-160&&enemy.position.y+bounds.bottom<=audit.camera.y+160;
    assert(!(horizontal&&vertical));
   }
   if(safe<x-240){safe=x-240;if(safe>removed+320){audit.backLine=std::max(audit.backLine,removed);removed=safe;}}
   // Exercise each successive encounter's approach/end/narrow camera interval.
   if(audit.bossEncounter&&x>=audit.frontLine-60)while(audit.bossEncounter)audit.killBoss();
  }
 }
 std::cout<<"Camera collision/AI/target/touch/fixed-map/cached-item integration passed\n";
}
