#include "../Sources/Core/GameSession.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
using namespace heavymach;
int main(int argc,char** argv){
 auto resources=argc>1?std::filesystem::path(argv[1]):std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";
 GameData data=GameData::loadDirectory(resources/"RawData");SaveSlot slot;GlobalInfo(slot).selectSlot(1);
 auto dimensions=[&](std::string_view name){auto file=resources/"Images"/name;if(!std::filesystem::exists(file))file+=".png";
  std::ifstream f(file,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);if(f.gcount()!=24)throw std::runtime_error("Missing test image "+file.string());
  auto be=[&](int p){return uint32_t(b[p])<<24|uint32_t(b[p+1])<<16|uint32_t(b[p+2])<<8|b[p+3];};return Vector2{float(be(16)),float(be(20))};};
 uint32_t rng=17;auto random=[&](){rng=rng*1664525u+1013904223u;return int32_t(rng>>1);}; // Deterministic test stream, not a production RNG replacement.
 // Original World survives game -> menu -> survival; hidden/paused ticks do not advance it.
 {SaveSlot clockSlot=slot;GameSession clock(data,clockSlot,dimensions,random);
  clock.start(0,0,0);assert(clock.world().gameTime==0);
  for(int n=0;n<12;++n)clock.tick(double(n+1)/60,0);
  double retained=clock.world().gameTime;assert(retained>0);
  clock.setPaused(true);clock.tick(5,0);assert(clock.world().gameTime==retained);
  auto retainedCamera=clock.world().camera;
  clock.start(1,4,4);assert(clock.world().camera.x==retainedCamera.x&&clock.world().camera.y==retainedCamera.y);assert(clock.world().gameTime==retained); // Previously reset to zero.
  assert(clock.world().gameMode==1&&!clock.world().paused);
  GameSession replacement(data,clockSlot,dimensions,random);
  // Menu lifetime does not tick a hidden World; a later wall clock is not added.
  replacement.restoreWorldCamera(retainedCamera);
  replacement.start(1,4,4,retained);assert(replacement.world().gameTime==retained);
  assert(replacement.world().camera.x==retainedCamera.x&&replacement.world().camera.y==retainedCamera.y);
  replacement.tick(500,0);assert(replacement.world().gameTime==retained+double(1.f/60.f));
  GameSession cold(data,clockSlot,dimensions,random);cold.start(1,4,4);assert(cold.world().gameTime==0);

 }
 // Explicit original mode follows exactly the default mode's state, draw order and RNG.
 {SaveSlot aSlot=slot,bSlot=slot;uint32_t ar=11,br=11;
  GameSession a(data,aSlot,dimensions,[&]{ar=ar*1664525u+1013904223u;return int32_t(ar>>1);});
  GameSession b(data,bSlot,dimensions,[&]{br=br*1664525u+1013904223u;return int32_t(br>>1);});
  b.setCanvasWidth(480);a.start(0,0,0);b.start(0,0,0);
  for(int frame=0;frame<120;++frame){
   a.touch({470,160},frame?1:0);b.touch({470,160},frame?1:0);a.tick((frame+1)/60.,.08f);b.tick((frame+1)/60.,.08f);
   assert(ar==br&&aSlot.bytes==bSlot.bytes&&a.world().camera.x==b.world().camera.x&&a.world().camera.y==b.world().camera.y);
   assert(a.sprites().size()==b.sprites().size());
   for(size_t n=0;n<a.sprites().size();++n){auto& x=a.sprites()[n];auto& y=b.sprites()[n];
    assert(x.tile==y.tile&&x.frame==y.frame&&x.cols==y.cols&&x.rows==y.rows&&x.z==y.z&&x.blend==y.blend);
    assert(x.position.x==y.position.x&&x.position.y==y.position.y&&x.scale.x==y.scale.x&&x.scale.y==y.scale.y&&x.rotation==y.rotation&&x.alpha==y.alpha);
   }
  }
 }
 // Every shipped map background covers the full wide canvas at both map ends.
 {size_t checked=0;std::set<std::string> playable{"Survival"};
  for(int mission=0;mission<8;++mission)for(int stage=0;stage<GlobalInfo::stageCount(mission);++stage){char key[32];std::snprintf(key,sizeof(key),"Stage%02d",GlobalInfo::mapNumber({mission,stage}));playable.insert(key);}
  // Stage42 is an unreferenced original data file with no supplied BG_map15a asset.
  for(const auto& name:playable){const auto& map=data.maps.at(name);if(map.background.empty())continue;
   WorldState background;background.map=map;background.center={(map.left+map.right)*.5f,(map.bottom+map.top)*.5f};
   background.backLine=map.left;background.frontLine=map.right+10;background.hasBackground=true;background.backgroundSize=dimensions(map.background);
   for(float width:{568.8889f,695.f,696.f}){
    background.setCanvasWidth(width);
    for(auto position:{Vector2{map.left,map.bottom},Vector2{map.right,map.top}}){background.processCamera(position);
     float x=background.backgroundPosition.x-background.camera.x+width*.5f,y=background.backgroundPosition.y-background.camera.y+160;
     float halfW=background.backgroundSize.x*background.backgroundDrawScale*.5f,halfH=background.backgroundSize.y*background.backgroundDrawScale*.5f;
     assert(x-halfW<.001f&&x+halfW>=width-.001f&&y-halfH<.001f&&y+halfH>=320-.001f);
    }
   }++checked;
  }assert(checked==42);
 }
 GameSession game(data,slot,dimensions,random);game.start(0,0,0);assert(game.player()&&game.player()->hp==256);assert(game.sprites().empty()); // First draw follows the first tick.
 auto events=game.takeEvents();assert(!events.empty());
 double time=0;bool fired=false;int soundCount=0;
 for(int i=0;i<150;++i){time+=1./60;game.touch({460,160},i==0?0:1);game.tick(time,0);fired|=game.bulletCount()>0;for(const auto& e:game.takeEvents())soundCount+=e.kind==SessionEventKind::AudioName;}
 assert(fired&&soundCount>0);
 game.setPaused(true);game.touch({460,160},2);game.setPaused(false);
 int resumedFire=0;
 for(int i=0;i<120;++i){time+=1./60;game.tick(time,0);for(const auto& e:game.takeEvents())if(i>30&&e.kind==SessionEventKind::AudioName&&e.text.starts_with("Wpn_"))++resumedFire;}
 assert(resumedFire==0); // Release while paused cannot leave isFire latched.
 game.touch({460,160},2);
 float x=game.player()->position.x;for(int i=0;i<120;++i){time+=1./60;game.tick(time,.1f);}assert(game.player()->position.x>x);
 double worldTime=game.world().gameTime;game.setPaused(true);game.tick(time+1,1);assert(game.world().gameTime==worldTime);game.setPaused(false);
 game.selectWeapon(1);assert(game.selectedWeapon()==1);game.selectWeapon(0);assert(game.selectedWeapon()==0);
 float oldVy=game.player()->velocity.y;game.booster();assert(game.player()->velocity.y>=oldVy);
 game.selectWeapon(3);bool sawTrail=false;
 game.touch({470,200},0);
 for(int i=0;i<180;++i){time+=1./60;game.tick(time,0);
  for(const auto& draw:game.renderItems())if(auto trail=std::get_if<TrailMesh>(&draw)){
   sawTrail=true;for(const auto& v:trail->vertices){assert(v.uv.x==0||v.uv.x==.625f);assert(v.uv.y==0||v.uv.y==1);}
  }game.takeEvents();}
 game.touch({470,200},2);assert(sawTrail); // Original starting RaderLsr weapon feeds draw-time trail geometry.
 game.start(0,0,1);assert(game.stage().stage==1&&game.player()->hp==256);
 for(int i=0;i<300;++i){time+=1./60;game.tick(time,.1f);game.takeEvents();}
 assert(game.enemyCount()>0);

 // Isolated fixture moves one ORIGINAL enemy definition near the original spawn;
 // weapon stats, machine parts, HP, damage, RNG and all gameplay code stay intact.
 auto fixture=data;auto& testMap=fixture.maps.at("Stage01");assert(!testMap.enemies.empty());
 testMap.enemies.resize(1);testMap.enemies[0].position={testMap.left+250,130};testMap.enemies[0].isBoss=false;
 SaveSlot combatSlot;GlobalInfo(combatSlot).selectSlot(1);GameSession combat(fixture,combatSlot,dimensions,random);combat.start(0,0,0);
 bool damaged=false,earned=false,gameOver=false;
 for(int i=0;i<2400;++i){
  const auto* p=combat.player();if(!p)break;
  float screenY=p->position.y-combat.world().camera.y+160;
  combat.touch({470,screenY+20},i==0?0:1);combat.tick(double(i+1)/60,0);
  damaged|=combat.player()&&combat.player()->hp<256;earned|=combatSlot.experience()>0;
  for(const auto& e:combat.takeEvents())gameOver|=e.kind==SessionEventKind::PlayerUI&&e.code==int(PlayerEventKind::GameOver);
  if(earned||gameOver)break;
 }
 std::cout<<"combat fixture damage="<<damaged<<" exp="<<combatSlot.experience()<<" gameOver="<<gameOver<<"\n";
 assert(earned&&combatSlot.experience()>0); // Original gun killed a real enemy definition.
 auto defenseData=fixture;auto enemy=testMap.enemies[0];for(int n=0;n<7;++n){enemy.position.x=testMap.left+180+float(n)*35;defenseData.maps.at("Stage01").enemies.push_back(enemy);}
 SaveSlot defenseSlot;GlobalInfo(defenseSlot).selectSlot(1);GameSession defense(defenseData,defenseSlot,dimensions,random);defense.start(0,0,0);
 bool tookHit=false,died=false;
 for(int i=0;i<7200;++i){defense.tick(double(i+1)/60,0);tookHit|=defense.player()&&defense.player()->hp<256;
  for(const auto& e:defense.takeEvents())died|=e.kind==SessionEventKind::PlayerUI&&e.code==int(PlayerEventKind::GameOver);
  if(died)break;}
 std::cout<<"defense fixture hit="<<tookHit<<" gameOver="<<died<<"\n";
 assert(tookHit&&died&&defense.world().paused&&defense.world().gameState==3);
 // Original zero-tile weapon parts retain firing/socket logic but nil texture draws nothing.
 auto zeroTileData=data;auto& zeroWeapon=zeroTileData.parts.at("P_Wpn_Gun01");zeroWeapon.tileCnt=0;
 assert(data.parts.at("E_MBoss_01_Wpn").tileCnt==0);
 SaveSlot zeroSlot;GlobalInfo(zeroSlot).selectSlot(1);int dummyRequests=0;
 auto zeroDimensions=[&](std::string_view name){if(name=="PartsDummy.png"){++dummyRequests;return Vector2{};}return dimensions(name);};
 GameSession zero(zeroTileData,zeroSlot,zeroDimensions,random);zero.start(0,0,0);bool zeroFired=false;
 for(int i=0;i<30;++i){zero.touch({470,180},i==0?0:1);zero.tick(double(i+1)/60,0);zeroFired|=zero.bulletCount()>0;
  for(const auto& sprite:zero.sprites())assert(sprite.tile!="PartsDummy.png");}
 assert(dummyRequests==0&&zeroFired&&zero.player()->hp==256);
 // GobBullet::isInCam 0xab00 always returns true for visual registration,
 // while tick 0xa444 explicitly invokes SUPER isInCam for collision testing.
 // Send one long-lived bullet offscreen; no enemies/terrain impacts can consume RNG.
 struct OffscreenResult {int calls=0,effectFrames=0;bool body=false,trail=false;};
 auto offscreen=[&](const char* effect){
  auto isolated=data;isolated.maps.at("Stage01").enemies.clear();
  auto& weapon=isolated.weapons.at("P_Wpn_Gun01");
  weapon.effectType=effect;weapon.effectTile="SmokeEffect.png";weapon.bulletTile="Bullet.png";
  weapon.spd=30000;weapon.spdRand=0;weapon.aimRand=0;weapon.g=0;weapon.pierce=true;
  weapon.shotCnt=1;weapon.delay=100;weapon.liveTm=60;
  SaveSlot isolatedSlot;GlobalInfo(isolatedSlot).selectSlot(1);OffscreenResult result;
  GameSession session(isolated,isolatedSlot,dimensions,[&]{++result.calls;return int32_t(0);});
  session.start(0,0,0);double t=0;
  for(int n=0;n<90&&session.bulletCount()==0;++n){session.touch({470,160},n?1:0);session.tick(t+=1./60,0);}
  assert(session.bulletCount()==1);session.touch({470,160},2);
  for(int n=0;n<12;++n){session.tick(t+=1./60,0);
   for(const auto& sprite:session.sprites())if(sprite.tile=="Bullet.png"&&(sprite.position.x>480||sprite.position.x<0||sprite.position.y>320||sprite.position.y<0))result.body=true;
   bool effectDrawn=false;for(const auto& sprite:session.sprites())if(sprite.tile=="SmokeEffect.png"&&(sprite.position.x>480||sprite.position.x<0||sprite.position.y>320||sprite.position.y<0))effectDrawn=true;
   result.effectFrames+=effectDrawn;
   for(const auto& item:session.renderItems())if(auto mesh=std::get_if<TrailMesh>(&item))result.trail|=!mesh->vertices.empty();
  }
  assert(session.bulletCount()==1);return result;
 };
 auto normalOff=offscreen("Normal"),smokeOff=offscreen("Smoke");
 assert(normalOff.body&&smokeOff.body);
 assert(smokeOff.calls>normalOff.calls&&(smokeOff.calls-normalOff.calls)%4==0);
 assert(offscreen("Flame").effectFrames==6); // Every other draw, including offscreen.
 assert(offscreen("Trail").trail&&offscreen("Laser").trail);
 std::cout<<"GameSession actual resource stage loop passed\n";
}
