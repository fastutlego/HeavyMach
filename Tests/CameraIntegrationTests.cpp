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
 std::cout<<"Camera collision/AI/target/touch/fixed-map/cached-item integration passed\n";
}
