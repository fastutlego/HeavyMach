#include "VTOL.hpp"
#include "World.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace heavymach {
const std::array<VTOLSpec,3>& vtolSpecs(){
 static const std::array<VTOLSpec,3> specs{{
  {"VTOL_Yak141","Extensions/VTOL_Yak141.png","Heli004","E_Wpn_ASM",112,724.f/2172,{-52.8f,-9.6f,52,13.6f},{-24,-8},{{{28,-8}}}},
  {"VTOL_Harrier","Extensions/VTOL_Harrier.png","Heli008","E_Boss_Wpn_01",108,773.f/2034,{-60*108.f/132,-13*108.f/132,61*108.f/132,17*108.f/132},{-39*108.f/132,-10*108.f/132},{{{12*108.f/132,-10*108.f/132}}}},
  {"VTOL_F35B","Extensions/VTOL_F35B.png","E0604","E_0604_Wpn",116,773.f/2033,{-68*116.f/148,-14*116.f/148,67*116.f/148,17*116.f/148},{-20*116.f/148,-11*116.f/148},{{{55*116.f/148,-8*116.f/148}}}}
 }};return specs;
}
const VTOLSpec* vtolSpec(std::string_view name){for(const auto& s:vtolSpecs())if(name==s.name)return &s;return nullptr;}
Vector2 vtolRotate(Vector2 p,float a){return {p.x*std::cos(a)-p.y*std::sin(a),p.x*std::sin(a)+p.y*std::cos(a)};}
float vtolPitch(float previous,float x,float y){
 // Nose faces left: positive rotation lowers the nose. Limit pitch to 8° + 3°.
 constexpr float rad=3.14159265358979323846f/180;
 float target=std::clamp(-x/.5f,-1.f,1.f)*8*rad-std::clamp(y/20.f,-1.f,1.f)*3*rad;
 return previous+(target-previous)/12.f;
}
void addVTOLExpansion(GameData& data){
 if(data.machines.contains("VTOL_Yak141"))return; // Install once per loaded data set.
 for(const auto& s:vtolSpecs()){
  const auto& source=data.machines.at(s.statsSource);
  (void)data.weapons.at(s.weapon);(void)data.parts.at(s.weapon);
  PartsInfo anchor;anchor.name=std::string(s.name)+"_Anchor";anchor.sockets={{0,0},s.muzzle};
  data.parts.emplace(anchor.name,anchor);
  MachDefinition unit;unit.name=s.name;unit.hpMax=source.hpMax;unit.giveExp=source.giveExp;unit.machType=1;
  unit.parts={{anchor.name,0,0,{{s.weapon,1,1,{}}}}};data.machines.emplace(unit.name,std::move(unit));
 }
 struct Plan {const char* map;int spec,count;};
 constexpr Plan plan[]={{"Stage06",0,5},{"Stage08",0,5},{"Stage11",0,7},{"Stage26",0,4},{"Stage16",0,9},
 {"Stage12",1,7},{"Stage27",1,4},{"Stage17",1,9},{"Stage22",1,10},{"Stage31",1,11},
 {"Stage21",2,10},{"Stage23",2,10},{"Stage32",2,11},{"Stage33",2,11}};
 struct GroundHost:WorldHost {Vector2 tileSize(std::string_view)override{return {480,320};}void spawnMapObject(const MapObjectDefinition&,int)override{}void spawnEnemy(const EnemyDefinition&)override{}void spawnPlayer(Vector2)override{}void event(const WorldEvent&)override{}} host;
 for(const auto& p:plan){
  auto& map=data.maps.at(p.map);auto ordered=map.enemies;
  std::stable_sort(ordered.begin(),ordered.end(),[](auto& a,auto& b){return a.position.x<b.position.x;});
  WorldState terrain;terrain.openMap(map,0,host);
  struct Candidate{float x,y,gap;};std::vector<Candidate> choices;
  for(size_t n=1;n<ordered.size();++n){
   float gap=ordered[n].position.x-ordered[n-1].position.x;
   int divisions=std::max(1,int(gap/60));
   for(int j=1;j<=divisions;++j){float x=ordered[n-1].position.x+gap*float(j)/float(divisions+1);
   if(gap<35||x<map.left+300||x>map.right-300)continue;
   if(std::any_of(ordered.begin(),ordered.end(),[&](const auto& e){return e.isBoss&&std::abs(e.position.x-x)<600;}))continue;
   float ground=terrain.mapHeight(x);if(ground==10000||ground<map.bottom)continue;
   float y=ground+140;if(y>map.top-30)continue;
   choices.push_back({x,y,gap});
   }
  }
  // Choose roomy gaps spread across the route. No RNG consumption at load time.
  std::stable_sort(choices.begin(),choices.end(),[](auto& a,auto& b){return a.gap>b.gap;});
  std::vector<Candidate> selected;
  // Farthest-point placement spreads the requested count over the whole route,
  // instead of filling several subdivisions of the same large gap first.
  while(int(selected.size())<p.count&&!choices.empty()){
   auto best=choices.begin();float bestDistance=-1;
   for(auto c=choices.begin();c!=choices.end();++c){float distance=c->gap;
    if(!selected.empty()){distance=1e9f;for(auto& a:selected)distance=std::min(distance,std::abs(a.x-c->x));}
    if(distance>bestDistance){bestDistance=distance;best=c;}
   }
   if(!selected.empty()&&bestDistance<100)break;
   selected.push_back(*best);choices.erase(best);
  }
  if(int(selected.size())!=p.count)throw std::runtime_error(std::string("Insufficient VTOL spawn gaps: ")+p.map+" "+std::to_string(selected.size()));
  std::sort(selected.begin(),selected.end(),[](auto& a,auto& b){return a.x<b.x;});
  // Append without reordering or replacing any authored entry (including bosses).
  for(auto& c:selected)map.enemies.push_back({vtolSpecs()[p.spec].name,{c.x,c.y},false});
 }
}
}
