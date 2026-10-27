#include "GroundAA.hpp"
#include "Fighters.hpp"
#include "GlobalInfo.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
using namespace heavymach;
struct Host:CombatHost{
 std::vector<double> shots;int32_t random31()override{return 0;}bool sweepTerrain(Vector2&,const Vector2&)override{return false;}bool inCamera(const BulletState&)override{return true;}std::optional<CombatTarget> targetAt(bool,Vector2)override{return {};}void damage(CombatTarget,int32_t)override{}bool acquireParticle()override{return false;}void event(const CombatEvent&)override{}void spawnBullet(const WeaponInfo&,bool,float,Vector2,double t)override{shots.push_back(t);}
};
int main(){auto root=std::filesystem::path(__FILE__).parent_path().parent_path();auto data=GameData::loadDirectory(root/"Resources/RawData");addVTOLExpansion(data);addFighterExpansion(data);auto before=data;auto plan=groundAAPlan(data);addGroundAAExpansion(data);int total=0;
 for(const auto& [name,map]:before.maps){auto& after=data.maps.at(name);assert(after.enemies.size()>=map.enemies.size());for(size_t n=0;n<map.enemies.size();++n){const auto&a=map.enemies[n];const auto&b=after.enemies[n];assert(a.name==b.name&&a.position.x==b.position.x&&a.position.y==b.position.y&&a.isBoss==b.isBoss);}}
 for(const auto& row:plan){auto& m=data.maps.at(row.map);int s=0,t=0;for(auto& e:m.enemies){if(e.name.starts_with("GAA_Shilka"))++s;if(e.name.starts_with("GAA_Tunguska"))++t;}assert(s==row.shilka&&t==row.tunguska&&s+t>=1);total+=s+t;std::cout<<row.mission+1<<','<<row.stage+1<<','<<row.map<<','<<row.originalTanks<<','<<s<<','<<t<<'\n';}
 auto size=data.machines.size();addGroundAAExpansion(data);assert(data.machines.size()==size);
 for(int mission=0;mission<7;++mission){int tanks=0,added=0;for(auto& r:plan)if(r.mission==mission){tanks+=r.originalTanks;added+=r.shilka+r.tunguska;}assert(added==std::lround(tanks*1.3));}
 for(const auto& [name,w]:data.weapons)if(isGroundAA(name)){
  const int expected=name.ends_with("_Missile")?2:name.starts_with("GAA_Shilka")?6:4;assert(w.shotCnt==expected);
  if(!name.ends_with("_Missile")){assert(w.effectTile=="BulletEffect.png"&&w.effectType=="Normal"&&w.bulletTile=="none");}
  Host host;WeaponState state;state.weaponInfo=&w;CombatFrame frame;frame.machType=3;frame.time=1;assert(fireWeapon(state,frame,host));
  for(int n=61;n<180;++n){frame.time=double(n)/60;tickWeapon(state,frame,host);}assert(int(host.shots.size())==expected);
  for(size_t n=1;n<host.shots.size();++n)assert(std::abs((host.shots[n]-host.shots[n-1])-w.shotDly)<.018);
  state={};state.weaponInfo=&w;frame.time=4;fireWeapon(state,frame,host);int remaining=state.fireCount;frame.paused=true;frame.time=30;tickWeapon(state,frame,host);assert(state.fireCount==remaining);
 }
 for(const auto& [name,w]:before.weapons){auto& n=data.weapons.at(name);assert(w.shotCnt==n.shotCnt&&w.shotDly==n.shotDly&&w.spd==n.spd&&w.dmg==n.dmg);}
 for(const auto& art:groundAAImages())assert(std::filesystem::exists(root/"Art"/art.source));
 for(auto name:{"GAA_Shilka_M1_Radar","GAA_Tunguska_M7_Radar"}){PartsState p;p.info=&data.parts.at(name);p.rotation=.15;for(int n=0;n<8;++n){DrawSprite d;d.rotation=.15;configureGroundAADraw(d,p,n*.25);assert(d.tile==groundAAPartTile(name,n));assert(d.rotation==.15f);}}
 std::cout<<"Ground AA definitions, original spawns, all 35 stages, bursts, pause and radar passed; added="<<total<<'\n';
}
