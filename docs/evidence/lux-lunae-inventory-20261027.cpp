#include "GameData.hpp"
#include "GlobalInfo.hpp"
#include <iostream>
using namespace heavymach;
void parts(const std::vector<MachPartDefinition>& ps){for(auto&p:ps){std::cout<<p.name<<",";parts(p.children);}}
int main(){auto d=GameData::loadDirectory("Resources/RawData");
 for(auto& [n,m]:d.machines)if(n.starts_with("Heli")){std::cout<<"HELI "<<n<<" hp="<<m.hpMax<<" exp="<<m.giveExp<<" parts=";parts(m.parts);std::cout<<'\n';}
 for(auto& [n,w]:d.weapons)if(n=="E_Wpn_ASM"||n=="E_0604_Wpn"||n.find("Msl")!=std::string::npos||n.find("Missile")!=std::string::npos)std::cout<<"WEAPON "<<n<<" type="<<w.wpnType<<" damage="<<w.dmg<<" speed="<<w.spd<<" delay="<<w.delay<<" shotDelay="<<w.shotDly<<" count="<<int(w.shotCnt)<<" lifetime="<<w.liveTm<<'\n';
 for(int m=0;m<8;++m)for(int s=0;s<GlobalInfo::stageCount(m);++s){auto index=GlobalInfo::mapNumber({m,s});char key[32];snprintf(key,sizeof(key),"Stage%02d",index);if(!d.maps.contains(key))continue;auto& map=d.maps.at(key);bool boss=false;for(auto&e:map.enemies)boss|=e.isBoss;if(boss){std::cout<<"BOSS mission="<<m+1<<" stage="<<s+1<<" map="<<key<<" bounds="<<map.left<<','<<map.right<<" enemies=";for(auto&e:map.enemies)if(e.isBoss)std::cout<<e.name<<'@'<<e.position.x<<',';std::cout<<'\n';}}
}
