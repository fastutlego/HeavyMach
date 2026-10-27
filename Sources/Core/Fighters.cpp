#include "Fighters.hpp"
#include <algorithm>
#include <stdexcept>
namespace heavymach {
const std::array<VTOLSpec,3>& fighterSpecs(){
 static const std::array<VTOLSpec,3> specs{{
  {"Lux_F22","Extensions/Lux_F22.png","Heli010","Lux_F22_Missile",112,827.f/1902,{-53,-11,53,14},{-18,-10},{{{53,-7}}},true},
  {"Lux_Su57","Extensions/Lux_Su57.png","Heli010","Lux_Su57_Missile",116,756.f/2079,{-55,-10,55,13},{-16,-9},{{{53,-4}}},true},
  {"Lux_MiG31","Extensions/Lux_MiG31.png","Heli010","Lux_MiG31_Missile",120,724.f/2172,{-56,-10,56,14},{-12,-9},{{{55,-3}}},true}
 }};return specs;
}
const VTOLSpec* aircraftSpec(std::string_view name){for(const auto& s:fighterSpecs())if(name==s.name)return &s;return vtolSpec(name);}
const std::array<FighterWave,14>& fighterWaves(){
 static const std::array<FighterWave,14> waves{{
  {"Stage04","Boss02",{1,1,0}},{"Stage09","Boss04",{1,1,2}},
  {"Stage14","Boss05",{2,2,2}},{"Stage29","Boss03",{3,3,2}},
  {"Stage19","Boss01",{3,2,3}},{"Stage24","Boss06",{2,3,3}},
  {"Stage35","Boss07",{3,3,2}},
  {"Stage03","",{2,1,1}},{"Stage08","",{1,2,1}},
  {"Stage13","",{1,1,2}},{"Stage28","",{2,1,1}},
  {"Stage18","",{1,2,1}},{"Stage23","",{1,1,2}},
  {"Stage34","",{2,1,1}}
 }};return waves;
}
const FighterWave* fighterWave(std::string_view map){for(const auto& w:fighterWaves())if(map==w.map)return &w;return nullptr;}
void addFighterExpansion(GameData& data){
 if(data.machines.contains("Lux_F22"))return;
 int maxHP=0;for(const auto& [name,m]:data.machines)if(name.starts_with("Heli"))maxHP=std::max(maxHP,m.hpMax);
 if(maxHP<=0)throw std::runtime_error("Missing original helicopter health");
 for(size_t n=0;n<fighterSpecs().size();++n){const auto& s=fighterSpecs()[n];
  const char* original=n==2?"E_0604_Wpn":"E_Wpn_ASM";
  auto weapon=data.weapons.at(original);weapon.name=s.weapon;weapon.spd*=1.25f;weapon.delay/=1.1f;
  weapon.shotCnt=n==2?6:3;weapon.shotDly=.5f/1.1f;weapon.aiTimeScale=1.f/1.1f;
  weapon.burstGroupSize=n==2?2:0;weapon.burstGroupDelay=n==2?1.f/1.1f:0;
  data.weapons.emplace(weapon.name,weapon);
  auto gun=data.parts.at(original);gun.name=s.weapon;gun.tileCnt=0;data.parts.emplace(gun.name,gun);
  PartsInfo anchor;anchor.name=std::string(s.name)+"_Anchor";anchor.sockets={{0,0},s.muzzle};data.parts.emplace(anchor.name,anchor);
  MachDefinition unit;unit.name=s.name;unit.hpMax=maxHP*17/10;unit.giveExp=data.machines.at(s.statsSource).giveExp;unit.machType=1;
  unit.parts={{anchor.name,0,0,{{s.weapon,1,1,{}}}}};data.machines.emplace(unit.name,std::move(unit));
 }
}
}
