#include "GameData.hpp"
#include "GlobalInfo.hpp"
#include "World.hpp"
#include <iostream>
#include <set>
using namespace heavymach;
struct Host:WorldHost{Vector2 tileSize(std::string_view)override{return {480,320};}void spawnMapObject(const MapObjectDefinition&,int)override{}void spawnEnemy(const EnemyDefinition&)override{}void spawnPlayer(Vector2)override{}void event(const WorldEvent&)override{}};
int main(){auto data=GameData::loadDirectory("Resources/RawData");std::set<std::string> used;for(int m=0;m<7;++m)for(int s=0;s<GlobalInfo::stageCount(m);++s){char name[32];snprintf(name,sizeof(name),"Stage%02d",GlobalInfo::mapNumber({m,s}));used.insert(name);}std::cout<<"map,campaign,index,name,boss,x,y,ground,aboveGround,top\n";for(auto&[name,map]:data.maps){Host host;WorldState world;world.openMap(map,0,host);for(size_t n=0;n<map.enemies.size();++n){auto&e=map.enemies[n];auto it=data.machines.find(e.name);if(it==data.machines.end()||it->second.machType!=1)continue;float g=world.mapHeight(e.position.x);std::cout<<name<<','<<used.contains(name)<<','<<n<<','<<e.name<<','<<e.isBoss<<','<<e.position.x<<','<<e.position.y<<','<<g<<','<<(g==10000?10000:e.position.y-g)<<','<<map.top<<'\n';}}}
