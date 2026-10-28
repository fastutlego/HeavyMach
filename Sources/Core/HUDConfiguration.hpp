#pragma once
#include "HUDLayout.hpp"
#include <array>
namespace heavymach::hud {
enum Group { menu, progress, jump, weapon, gauges, aim, passives, preview, cheat, upperTrim, groupCount };
inline constexpr std::array<Rect,groupCount> bounds{{{0,256,76,39},{0,293,102,27},{0,0,72,68},{67,0,93,64},{160,0,174,64},{334,0,146,64},{0,190,50,68},{170,270,224,40},{0,0,150,32},{76,256,404,64}}};
struct Configuration {
 std::array<std::optional<Placement>,groupCount> positions{};
 Transform legacy{};Rect safe{0,0,480,320};Rect cheatBounds=bounds[cheat];
 Rect localBounds(int i)const{return i==cheat?cheatBounds:bounds.at(i);}
 bool customized()const {for(auto& p:positions)if(p)return true;return false;}
 ResolvedGroup group(int i)const{return resolve(localBounds(i),legacy,safe,positions.at(i));}
 bool samePositions(const Configuration& b)const {for(int i=0;i<groupCount;++i){if(bool(positions[i])!=bool(b.positions[i]))return false;if(positions[i]&&(positions[i]->x!=b.positions[i]->x||positions[i]->y!=b.positions[i]->y))return false;}return true;}
};
inline constexpr std::array<Rect,6> hits{{{6,263,64,32},{0,0,68,68},{67,0,72,62},{334,10,46,46},{382,10,46,46},{430,10,46,46}}};
inline constexpr std::array<int,6> hitGroups{{menu,jump,weapon,aim,aim,aim}};
inline bool valid(const Configuration& c,bool includeCheat=false) {
 for(int i=0;i<groupCount;++i)if((i!=cheat||includeCheat)&&!c.group(i).fits)return false;
 for(size_t i=0;i<hits.size();++i)for(size_t j=i+1;j<hits.size();++j){
  if(hitGroups[i]==hitGroups[j])continue;
  if(!c.positions[hitGroups[i]]&&!c.positions[hitGroups[j]])continue; // original one-pixel overlap
  if(overlaps(c.group(hitGroups[i]).hitRect(hits[i]),c.group(hitGroups[j]).hitRect(hits[j])))return false;
 }
 if(includeCheat)for(size_t i=0;i<hits.size();++i)if((c.positions[cheat]||c.positions[hitGroups[i]])&&overlaps(c.group(cheat).visualBounds,c.group(hitGroups[i]).hitRect(hits[i])))return false;
 return true;
}
}
