#include "ValidationScenario.hpp"
#if defined(HM_VALIDATION) && HM_VALIDATION
#include "GlobalInfo.hpp"
#include "World.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>
namespace heavymach {
std::vector<ValidationStage> validationStages(const GameData& data){
 std::vector<ValidationStage> result;
 for(int mission=0;mission<8;++mission){for(int stage=0;stage<GlobalInfo::stageCount(mission);++stage){
  char key[32];std::snprintf(key,sizeof(key),"Stage%02d",GlobalInfo::mapNumber({mission,stage}));auto it=data.maps.find(key);if(it==data.maps.end())continue;
  ValidationStage d;d.mission=mission;d.stage=stage;d.mapName=key;d.naturalProgression=mission<=6;for(const auto& e:it->second.enemies)if(e.isBoss)d.bosses.push_back(e);result.push_back(std::move(d));
 }}
 return result;
}
std::vector<std::string> validationWeapons(const GameData& data){std::vector<std::string> names;for(const auto& [name,w]:data.weapons)if(name.starts_with("P_")&&data.parts.contains(name))names.push_back(name);std::sort(names.begin(),names.end());return names;}
std::optional<Vector2> validationSpawnPosition(const GameData& data,const ValidationScenario& s){
 auto stages=validationStages(data);auto entry=std::find_if(stages.begin(),stages.end(),[&](const auto& v){return v.mission==s.mission&&v.stage==s.stage;});if(entry==stages.end())throw std::invalid_argument("validation stage");
 const auto& map=data.maps.at(entry->mapName);
 if(s.playerPosition){auto p=*s.playerPosition;if(!std::isfinite(p.x)||!std::isfinite(p.y)||p.x<map.left||p.x>map.right||p.y<map.bottom||p.y>map.top)throw std::invalid_argument("validation position outside original map");return p;}
 if(!s.bossIndex)return std::nullopt;
 if(*s.bossIndex>=entry->bosses.size())throw std::invalid_argument("validation boss index");
 // Construct the real world collision bins. Asset dimensions affect only
 // presentation and object preload, which this read-only geometry host discards.
 struct GeometryHost final:WorldHost {
  Vector2 tileSize(std::string_view)override{return {480,320};}
  void spawnMapObject(const MapObjectDefinition&,int)override{}
  void spawnEnemy(const EnemyDefinition&)override{}
  void spawnPlayer(Vector2)override{}
  void event(const WorldEvent&)override{}
 } host;
 WorldState terrain;terrain.openMap(map,0,host);
 const float bossX=entry->bosses[*s.bossIndex].position.x;
 auto stablePosition=[&](float x)->std::optional<Vector2>{
  float height=terrain.mapHeight(x);
  if(height==10000||height<map.bottom||height+40.f>map.top||x>=bossX)return {};
  Vector2 start{x,height+40.f};GobHvMach probe;probe.player=true;probe.position=start;probe.machState=2;probe.hp=1;
  for(int tick=0;tick<120;++tick){auto frame=terrain.motionWorld(probe.position.x,double(tick+1)/60);auto events=probe.tick(frame);
   for(const auto& event:events)if(event.kind==MotionEventKind::Damage||event.kind==MotionEventKind::NextStage)return {};
  }
  return probe.machState==1?std::optional<Vector2>(start):std::nullopt;
 };
 float x=std::clamp(bossX-180.f,map.left+20.f,map.right-20.f);
 for(int n=0;n<=24;++n){if(auto point=stablePosition(x))return point;x=std::max(map.left+20.f,x-20.f);}
 // If the local approach has no runtime-supported landing, search the whole
 // authored map to the boss's left, closest first on a one-unit grid. A distant
 // result is only a starting position: the runner must approach via real input.
 for(x=std::min(bossX-20.f,map.right-20.f);x>=map.left+20.f;x-=1.f)if(auto point=stablePosition(x))return point;

 throw std::runtime_error("no stable runtime collision ground found before validation boss");
}
void prepareValidationSlot(SaveSlot& target,const GameData& data,const ValidationScenario& s){
 auto stages=validationStages(data);auto stage=std::find_if(stages.begin(),stages.end(),[&](const auto& v){return v.mission==s.mission&&v.stage==s.stage;});if(stage==stages.end())throw std::invalid_argument("validation stage is absent from original data");
 if(s.internalLevel<0||s.internalLevel>99)throw std::invalid_argument("validation level outside 0..99");
 for(auto grade:s.grades)if(grade>6)throw std::invalid_argument("validation grade outside 0..6");
 if(s.bossIndex&&*s.bossIndex>=stage->bosses.size())throw std::invalid_argument("validation boss index");
 if(s.playerPosition){auto p=*s.playerPosition;const auto& map=data.maps.at(stage->mapName);if(!std::isfinite(p.x)||!std::isfinite(p.y)||p.x<map.left||p.x>map.right||p.y<map.bottom||p.y>map.top)throw std::invalid_argument("validation position outside original map");}
 auto names=validationWeapons(data);for(const auto& name:s.weapons)if(!std::binary_search(names.begin(),names.end(),name))throw std::invalid_argument("validation weapon is not original player weapon");
 (void)validationSpawnPosition(data,s); // Terrain errors cannot partially commit the supplied slot.
 SaveSlot prepared;GlobalInfo info(prepared);info.selectSlot(0);prepared.setInt(0x28,s.internalLevel);prepared.bytes[0x2f]=s.grades[0];prepared.bytes[0x2e]=s.grades[1];prepared.bytes[0x2d]=s.grades[2];
 auto threshold=GlobalInfo::thresholds(s.internalLevel);prepared.setInt(0x20,s.nearLevelUp?threshold.next-1:threshold.previous);prepared.setFloat(0x24,0);prepared.setInt(0xc,s.mission);prepared.setInt(0x10,s.stage);prepared.setInt(0x14,GlobalInfo::currentStage({s.mission,s.stage}));
 auto grant=[&](const std::string& name){for(int i=0;i<prepared.weaponCount();++i)if(prepared.weaponName(size_t(i))==name)return;info.addWeaponItem(name,data.weapons.at(name).charge);};
 if(s.allWeapons)for(const auto& name:names)grant(name);for(const auto& name:s.weapons)grant(name);
 if(s.selectedWeapon<0||s.selectedWeapon>=prepared.weaponCount())throw std::invalid_argument("validation selected weapon index");
 prepared.validate();target=prepared;
}
}
#endif
