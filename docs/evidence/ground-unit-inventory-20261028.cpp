#include "GameData.hpp"
#include <iostream>
#include <map>
#include <functional>
using namespace heavymach;
int main(){auto d=GameData::loadDirectory("Resources/RawData");std::map<std::string,MachDefinition> machines(d.machines.begin(),d.machines.end());
for(auto& [name,m]:machines){if(m.machType!=0)continue;std::cout<<name<<" type="<<int(m.machType)<<" hp="<<m.hpMax<<" exp="<<m.giveExp<<"\n";
std::function<void(const MachPartDefinition&)> visit=[&](const auto& p){auto a=d.parts.find(p.name);std::cout<<"  part="<<p.name;if(a!=d.parts.end())std::cout<<" type="<<int(a->second.partsType)<<" tile="<<a->second.tileName()<<" frames="<<int(a->second.tileCnt)<<" sockets="<<a->second.sockets.size();auto w=d.weapons.find(p.name);if(w!=d.weapons.end())std::cout<<" weaponType="<<w->second.wpnType<<" damage="<<w->second.dmg<<" shotCnt="<<int(w->second.shotCnt)<<" shotDly="<<w->second.shotDly<<" delay="<<w->second.delay<<" bullet="<<w->second.bulletTile;std::cout<<"\n";for(auto& c:p.children)visit(c);};for(auto& p:m.parts)visit(p);}
std::map<std::string,MapDefinition> maps(d.maps.begin(),d.maps.end());for(auto& [name,m]:maps){std::map<std::string,int> counts;for(auto& e:m.enemies)if(d.machines.contains(e.name)&&d.machines.at(e.name).machType==0)++counts[e.name];if(counts.empty())continue;std::cout<<name;for(auto& [n,c]:counts)std::cout<<" "<<n<<":"<<c;std::cout<<"\n";}}
