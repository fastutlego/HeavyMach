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
#include "GameSession.cpp"
#include <cassert>
#include <fstream>
#include <iostream>
using namespace heavymach;

int main(int argc,char**argv){
 const auto root=std::filesystem::current_path();auto data=GameData::loadDirectory(root/"Resources/RawData");addVTOLExpansion(data);addFighterExpansion(data);
 auto dims=[&](std::string_view name){auto path=name.starts_with("Extensions/")?root/"Art"/name:root/"Resources/Images"/name;std::ifstream f(path,std::ios::binary);unsigned char b[24]{};f.read((char*)b,24);if(f.gcount()!=24)return Vector2{40,12};auto be=[&](int n){return uint32_t(b[n])<<24|uint32_t(b[n+1])<<16|uint32_t(b[n+2])<<8|b[n+3];};return Vector2{float(be(16)),float(be(20))};};

 std::cout<<"variant,map,width,height,index,name,phase,tick,x,y,endx,playerx,playery,top,ground,widthBody,heightBody\n";
 for(int variant=0;variant<5;++variant)for(auto canvas: {Vector2{480,320},Vector2{480,360},Vector2{695,320},Vector2{569,320}})
 for(int mission=0;mission<7;++mission)for(int stage=0;stage<GlobalInfo::stageCount(mission);++stage){
  char key[32];snprintf(key,sizeof(key),"Stage%02d",GlobalInfo::mapNumber({mission,stage}));auto wave=fighterWave(key);if(!wave)continue;
  SaveSlot save;GlobalInfo(save).selectSlot(0);uint32_t seed=9;GameSession game(data,save,dims,[&]{seed=seed*1664525+1013904223;return int32_t(seed>>1);});game.setCanvasWidth(canvas.x);game.start(0,mission,stage);auto& i=*game.impl_;i.world.canvasHeight=canvas.y;
  float trigger=i.world.map.right-800;if(wave->boss[0]){auto boss=std::find_if(i.world.map.enemies.begin(),i.world.map.enemies.end(),[&](const auto&e){return e.name==wave->boss;});trigger=boss->position.x-490;}
  i.player->position.x=trigger-40;i.world.processRegen(trigger-40,i);
  for(size_t n=0;n<i.world.map.enemies.size();++n)if(i.world.bossEncounterActivated[n]&&!i.world.mapEnemiesDefeated[n]&&i.world.map.enemies[n].name!=wave->boss)i.world.killMapBoss(n);
  i.player->position.x=trigger;float ground=i.world.mapHeight(trigger);if(ground!=10000)i.player->position.y=ground+30;
  i.world.processRegen(trigger,i);for(int c=0;c<60;++c)i.world.processCamera(i.player->position);i.spawnReinforcements();
  std::vector<GameSession::Impl::Machine*> jets;
  for(auto& e:i.enemies)if(e.definition->name.starts_with("Lux_"))jets.push_back(&e);
  float first=jets.at(0)->enemy->position.x;float end=jets.at(0)->aircraftEntryEndX;
  for(size_t n=0;n<jets.size();++n){auto&m=*jets[n];if(variant>=1&&variant<=2)m.enemy->position.x=first+140*n;
   if(variant==2){float low=std::max(i.world.backLine+70,i.player->position.x+120);float span=std::max(0.f,end-low);m.aircraftEntryEndX=low+span*float(n)/float(std::max(size_t(1),jets.size()-1));}
  }

  if(variant>=3){
   float maxHeight=0,terrain=-1e9;
   for(auto*m:jets){maxHeight=std::max(maxHeight,m->vtol->width*m->vtol->aspect);float g=i.world.mapHeight(m->aircraftEntryEndX);if(g!=10000)terrain=std::max(terrain,g);}
   if(terrain==-1e9)terrain=i.player->position.y-30;
   float low=std::max(terrain+maxHeight*.5f+20,i.world.camera.y-160+maxHeight*.5f+20);
   float high=std::min(i.world.map.top,i.world.camera.y+canvas.y-160)-maxHeight*.5f-20;
   int lanes=std::max(1,int(std::floor((high-low)/(maxHeight+12)))+1);
   lanes=std::min(lanes,variant==3?2:4);

   std::vector<int> usage(lanes,0);
   for(size_t n=0;n<jets.size();++n){
    int best=0;float bestScore=1e20f;
    for(int lane=0;lane<lanes;++lane){float y=low+(lanes>1?(high-low)*lane/(lanes-1):0),score=usage[lane]*.001f;
     for(size_t prior=0;prior<n;++prior){auto&a=*jets[n];auto&b=*jets[prior];float dx=(a.vtol->width+b.vtol->width)*.5f-std::abs(a.enemy->position.x-b.enemy->position.x);float dy=(a.vtol->width*a.vtol->aspect+b.vtol->width*b.vtol->aspect)*.5f-std::abs(y-b.enemy->position.y);if(dx>0&&dy>0)score+=dx*dy;}
     if(score<bestScore){bestScore=score;best=lane;}
    }
    ++usage[best];jets[n]->enemy->position.y=low+(lanes>1?(high-low)*best/(lanes-1):0);
   }

  }
  auto output=[&](const char*phase,int tick){for(size_t n=0;n<jets.size();++n){auto&m=*jets[n];std::cout<<variant<<','<<key<<','<<canvas.x<<','<<canvas.y<<','<<n<<','<<m.definition->name<<','<<phase<<','<<tick<<','<<m.enemy->position.x<<','<<m.enemy->position.y<<','<<m.aircraftEntryEndX<<','<<i.player->position.x<<','<<i.player->position.y<<','<<i.world.map.top<<','<<i.world.mapHeight(m.enemy->position.x)<<','<<m.vtol->width<<','<<m.vtol->width*m.vtol->aspect<<'\n';}};
  output("spawn",0);int finalEntry=-1;
  for(int tick=1;tick<=2400;++tick){i.world.gameTime=double(tick)/60;i.globalTime=i.world.gameTime;bool entering=false;
   for(auto*m:jets){i.current=m;if(m->aircraftEntering)i.advanceAircraftEntry(*m);else m->enemy->tick(i.enemyFrame(*m),i);entering|=m->aircraftEntering;}i.current=nullptr;
   if(!entering&&finalEntry<0){finalEntry=tick;output("last_entry",tick);}if(tick==finalEntry+300&&finalEntry>=0)output("after_5s",tick);if(tick==finalEntry+1200&&finalEntry>=0){output("after_20s",tick);break;}
  }
 }
}
