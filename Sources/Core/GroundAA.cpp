#include "GroundAA.hpp"
#include "GlobalInfo.hpp"
#include "World.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace heavymach {
const std::vector<GroundAAImage>& groundAAImages(){static const std::vector<GroundAAImage> images{
#include "GroundAAArt.inc"
};return images;}
const GroundAAImage* groundAAImage(std::string_view name){for(const auto& a:groundAAImages())if(a.name==name)return &a;return nullptr;}
bool isGroundAA(std::string_view name){return name.starts_with("GAA_Shilka_M")||name.starts_with("GAA_Tunguska_M");}
std::string groundAAPartTile(std::string_view part,int frame){
 if(!isGroundAA(part))return {};
 auto start=part.find("_M"),end=part.find('_',start+2);if(end==std::string_view::npos)return {};
 std::string kind(part.substr(end+1));
 if(kind=="Body"&&frame)kind="Body2";
 if(kind=="Radar")kind+=std::to_string(frame%8);
 return "Extensions/"+std::string(part.substr(0,start))+"_"+kind+".png";
}
std::optional<Vector2> groundAAPartSize(std::string_view part){if(auto a=groundAAImage(groundAAPartTile(part)))return a->size;return {};}
void configureGroundAADraw(DrawSprite& d,const PartsState& p,double time){
 if(!p.info||!isGroundAA(p.info->name))return;
 bool radar=p.info->name.ends_with("_Radar");int frame=radar?int(std::floor(std::max(0.,time)*4))%8:p.tileFrame;
 d.tile=groundAAPartTile(p.info->name,frame);d.cols=d.rows=1;d.frame=0;
 if(radar){const auto* a=groundAAImage(d.tile);float dx=a?-a->radarAnchorX:0;
  d.position.x+=dx*std::cos(p.rotation);d.position.y+=dx*std::sin(p.rotation);}
}
namespace {
struct GroundHost:WorldHost{Vector2 tileSize(std::string_view)override{return {480,320};}void spawnMapObject(const MapObjectDefinition&,int)override{}void spawnEnemy(const EnemyDefinition&)override{}void spawnPlayer(Vector2)override{}void event(const WorldEvent&)override{}};
std::vector<Vector2> safePositions(const GameData& data,const MapDefinition& map){
 GroundHost host;WorldState terrain;terrain.openMap(map,0,host);std::vector<Vector2> points;
 for(float x=map.left+300;x<map.right-100;x+=10){float y=terrain.mapHeight(x),left=terrain.mapHeight(x-20),right=terrain.mapHeight(x+20);
  if(y==10000||left==10000||right==10000||y<map.bottom+20||std::abs(left-right)>40)continue;
  if(!points.empty()&&x-points.back().x<115)continue;
  bool blocked=false;for(const auto& e:map.enemies){if(isGroundAA(e.name))continue;auto m=data.machines.find(e.name);if(m!=data.machines.end()&&m->second.machType==0&&std::abs(e.position.x-x)<(e.isBoss?140:60)){blocked=true;break;}}
  if(blocked)continue;
  // A long height ray can select a different overlapping segment than the
  // original short per-tick fall. Require actual contact at this spawn.
  GobHvMach probe;probe.machType=0;probe.machState=2;probe.position={x,y+4};bool stable=true;
  for(int n=0;n<120;++n){terrain.gameTime=(n+1)/60.;for(auto& event:probe.tick(terrain.motionWorld(probe.position.x,terrain.gameTime,[]{return 0;})))if(event.kind==MotionEventKind::Damage)stable=false;}
  if(stable&&probe.machState==1&&std::abs(probe.position.x-x)<30)points.push_back({x,y+4});
 }return points;
}
}
std::vector<GroundAAPlacement> groundAAPlan(const GameData& data){
 std::vector<GroundAAPlacement> result;
 // Seven released mission packages. The original eighth package has empty maps
 // and no menu entry; it is not unlocked by this expansion.
 for(int mission=0;mission<7;++mission){int total=0;std::vector<GroundAAPlacement> rows;
  for(int stage=0;stage<GlobalInfo::stageCount(mission);++stage){char key[32];snprintf(key,sizeof(key),"Stage%02d",GlobalInfo::mapNumber({mission,stage}));int count=0;
   for(const auto& e:data.maps.at(key).enemies)if(e.name.starts_with("Tank")&&!e.isBoss)++count;
   rows.push_back({mission,stage,count,0,0,key});total+=count;
  }
  int target=std::max(int(rows.size()),int(std::lround(total*1.3)));std::vector<int> counts(rows.size(),1);
  std::vector<size_t> capacities;for(auto& row:rows){auto capacity=safePositions(data,data.maps.at(row.map)).size();if(!capacity)throw std::runtime_error("No safe ground AA position: "+row.map);capacities.push_back(capacity);}
  for(int n=int(rows.size());n<target;++n){int best=-1;double score=-1e9;
   for(size_t j=0;j<rows.size();++j){if(size_t(counts[j])>=capacities[j])continue;double need=double(target)*rows[j].originalTanks/std::max(total,1)-counts[j];if(need>score){score=need;best=int(j);}}
   if(best<0)throw std::runtime_error("Insufficient ground AA capacity in mission "+std::to_string(mission+1));++counts[size_t(best)];}

  int alternate=mission%2;for(size_t j=0;j<rows.size();++j){for(int n=0;n<counts[j];++n){if(alternate++%2)++rows[j].tunguska;else ++rows[j].shilka;}result.push_back(rows[j]);}
 }return result;
}
void addGroundAAExpansion(GameData& data){
 if(data.machines.contains("GAA_Shilka_M1"))return;
 auto plan=groundAAPlan(data);
 for(int mission=0;mission<7;++mission){std::vector<const MachDefinition*> tanks;
  for(const auto& row:plan)if(row.mission==mission)for(const auto& e:data.maps.at(row.map).enemies)if(e.name.starts_with("Tank")&&!e.isBoss)tanks.push_back(&data.machines.at(e.name));
  if(tanks.empty())throw std::runtime_error("Ground AA baseline missing");
  std::stable_sort(tanks.begin(),tanks.end(),[](auto a,auto b){return a->hpMax<b->hpMax;});auto source=*tanks[tanks.size()/2];
  for(bool tung:{false,true}){
   std::string id=std::string(tung?"GAA_Tunguska_M":"GAA_Shilka_M")+std::to_string(mission+1);
   auto part=[&](const char* suffix,int type,int count,std::vector<Vector2> sockets){PartsInfo p;p.name=id+suffix;p.partsType=type;p.tileCnt=count;p.sockets=std::move(sockets);data.parts.emplace(p.name,p);return p.name;};
   auto body=part("_Body",0,2,{{0,tung?-11.f:-9.f},{0,tung?9.f:5.f}});
   auto top=part("_Turret",1,1,{{0,tung?-11.5f:-7.f},{tung?-18.f:-19.f,-1.f},{14,2.f},{12,0}});
   auto gun=part("_Gun",2,1,{{tung?14.f:11.f,-2.f},{tung?-18.5f:-16.f,1.f}});
   auto radar=part("_Radar",1,1,{{0,tung?-6.5f:-8.f}});
   char original[40];snprintf(original,sizeof(original),"E_Tank_Wpn_%02d",mission+1);auto w=data.weapons.at("E_Tank_Wpn_01");w.dmg=data.weapons.at(original).dmg;w.spd=data.weapons.at(original).spd;w.name=gun;w.shotCnt=tung?4:6;w.shotDly=data.weapons.at("E_0505_Wpn").shotDly;data.weapons.emplace(gun,w);
   MachPartDefinition turret{top,1,1,{{radar,2,0,{}},{gun,1,2,{}}}};
   if(tung){auto missile=part("_Missile",2,1,{{9,-3},{-15,3}});snprintf(original,sizeof(original),"E_ArmoCar_Msl_%02d",std::min(mission+1,5));w=data.weapons.at(original);w.name=missile;w.shotCnt=2;w.shotDly=.5f;data.weapons.emplace(missile,w);turret.children.push_back({missile,3,3,{}});}
   MachDefinition unit;unit.name=id;unit.hpMax=source.hpMax;unit.giveExp=source.giveExp;unit.machType=0;unit.parts={{body,0,0,{turret}}};data.machines.emplace(id,std::move(unit));
  }
 }
 for(const auto& row:plan){auto& map=data.maps.at(row.map);auto positions=safePositions(data,map);int count=row.shilka+row.tunguska,s=row.shilka,t=row.tunguska;
  for(int n=0;n<count;++n){size_t index=size_t((n+.5)*positions.size()/count);auto pos=positions.at(index);bool tung=t>0&&(s==0||n%2);if(tung)--t;else --s;
   map.enemies.push_back({std::string(tung?"GAA_Tunguska_M":"GAA_Shilka_M")+std::to_string(row.mission+1),pos,false});}
 }
}
}
