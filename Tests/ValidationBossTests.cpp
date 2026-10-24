#include "../Sources/Core/GameSession.hpp"
#if defined(HM_VALIDATION) && HM_VALIDATION
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <map>
#include <iomanip>
#include <set>
using namespace heavymach;
// Actual combat experiment: failures are retained and produce nonzero exit status.
// No enemy mutation, forced result, infinite ammo, healing, or invulnerability.
int main(int argc,char** argv){
 auto resources=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";
 auto data=GameData::loadDirectory(resources/"RawData");
 auto dims=[&](std::string_view name){auto p=resources/"Images"/name;if(!std::filesystem::exists(p))p+=".png";std::ifstream f(p,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);if(f.gcount()!=24)throw std::runtime_error("Missing image "+p.string());auto be=[&](int i){return uint32_t(b[i])<<24|uint32_t(b[i+1])<<16|uint32_t(b[i+2])<<8|b[i+3];};return Vector2{float(be(16)),float(be(20))};};
 std::ofstream output(argc>1?argv[1]:"validation-boss-results.jsonl");if(!output)throw std::runtime_error("Cannot write results");
 const std::string requestedStrategy=argc>3?argv[3]:"baseline";
 const std::set<std::string> v6Cases={"Stage16:0","Stage29:0","Stage17:0","Stage17:1","Stage18:0","Stage18:1","Stage19:0","Stage20:1","Stage21:0","Stage23:0","Stage24:0","Stage30:0","Stage32:0","Stage34:0","Stage35:0"};
 int total=0,passed=0;
 for(const auto& stage:validationStages(data))for(size_t bi=0;bi<stage.bosses.size();++bi){
  if(argc>2&&!std::string(argv[2]).empty()&&(","+std::string(argv[2])+",").find(","+stage.mapName+":"+std::to_string(bi)+",")==std::string::npos)continue;
  // Fixed, reviewable input scripts selected before combat. No result-based
  // retry, HP mutation, ammunition refill, or outcome override.
  const bool useV6=requestedStrategy=="v6-verified"||(requestedStrategy=="full-verified"&&v6Cases.contains(stage.mapName+":"+std::to_string(bi)));
  const std::string strategy=useV6?(stage.mapName=="Stage35"&&bi==0?"v6-radar-still-hop210-hold2":stage.mapName=="Stage23"&&bi==0?"v6-radar-still-hold":"v6-radar"):(requestedStrategy=="full-verified"?"baseline":requestedStrategy);
  const bool radarPriority=strategy.starts_with("v6-radar");
  const bool evasive=strategy=="v6-strafe"||radarPriority;
  ++total;SaveSlot slot;uint32_t seed=11;auto rng=[&](){seed=seed*1664525+1013904223;return int32_t(seed>>1);};
  GameSession game(data,slot,dims,rng);ValidationScenario scenario;scenario.mission=stage.mission;scenario.stage=stage.stage;scenario.bossIndex=bi;scenario.grades={6,6,6};scenario.allWeapons=true;
  bool sequential=(requestedStrategy=="sequential"||requestedStrategy=="full-verified")&&stage.mapName=="Stage25"&&bi==1;if(sequential)scenario.bossIndex=0;
  uintptr_t target=0;int initialHP=-1,lastHP=-1,frames=0,shots=0;bool dead=false;std::string reason="timeout";Vector2 lastPosition=stage.bosses[bi].position;
  int moveDirection=-1,clampedTouches=0,dashRequests=0,radarCycle=0;
  int approachFrames=0,approachShotSounds=0;std::set<uintptr_t> predecessorIDs;std::vector<GameSession::ValidationEnemySnapshot> predecessorDeaths;bool approachRequired=false;float initialBossDistance=0;
  int initialEnemies=0,initialBehind=0,initialPriorBosses=0,framesTargetBehind=0,framesNearAim=0,maxHPDrop=0,damageTicks=0;
  std::map<std::string,int> weaponFrames,ammoSpent,damageWhileSelected,approachWeaponFrames;Vector2 spawn{},finalPlayer{};
  try{
   game.startValidation(scenario);game.takeEvents();
   auto initial=game.validationEnemies();spawn=game.player()->position;initialEnemies=int(initial.size());
   for(const auto& e:initial){initialBehind+=e.position.x<spawn.x-240;initialPriorBosses+=e.boss&&e.position.x<spawn.x;}
   initialBossDistance=stage.bosses[bi].position.x-spawn.x;approachRequired=initialBossDistance>=500;
   auto identify=[&](){for(const auto& e:game.validationEnemies())if(e.boss&&e.name==stage.bosses[bi].name&&e.spawnPosition.x==stage.bosses[bi].position.x&&e.spawnPosition.y==stage.bosses[bi].position.y){if(!target){target=e.id;initialHP=e.hp;}lastHP=e.hp;lastPosition=e.position;return true;}return false;};
   identify();
   if(!target){
    for(;approachFrames<(sequential?7200:3600);++approachFrames){
     if(!game.player()||game.player()->hp<=0)break;
     if(sequential&&game.world().paused&&game.player()->upgradeStep==1){game.confirmUpgrade();game.finishUpgradeSlide();}
     if(game.world().paused)break;
     if(approachFrames%60==0)game.booster();
     Vector2 approachAim{479,160};float approachTilt=.1f;
     if(sequential){
      auto candidates=game.validationEnemies();
      for(const auto& e:candidates)if(e.boss&&e.hp==0&&e.removed&&(e.spawnPosition.x!=stage.bosses[bi].position.x||e.spawnPosition.y!=stage.bosses[bi].position.y)&&predecessorIDs.insert(e.id).second)predecessorDeaths.push_back(e);
      const GameSession::ValidationEnemySnapshot* enemy=nullptr;float nearest=500;
      for(const auto& e:candidates)if(!e.removed&&e.hp>0&&e.position.x>game.player()->position.x){float distance=e.position.x-game.player()->position.x;if(distance<nearest){nearest=distance;enemy=&e;}}
      int desired=0;
      if(enemy&&enemy->boss){bool chosen=false;for(const char* name:{"P_Wpn_Laser","P_Wpn_ElShock","P_Wpn_Missile03","P_Wpn_Missile02"}){for(int i=1;i<slot.weaponCount();++i)if(slot.weaponName(size_t(i))==name&&slot.weaponCharge(size_t(i))>0){desired=i;chosen=true;break;}if(chosen)break;}}
      if(game.selectedWeapon()!=desired){game.touch({},2);game.selectWeapon(desired);}
      if(enemy){
       Vector2 origin=game.validationAimOrigin();approachAim={enemy->position.x+(enemy->bounds.left+enemy->bounds.right)*.5f-game.world().camera.x+240,enemy->position.y+(enemy->bounds.bottom+enemy->bounds.top)*.5f-game.world().camera.y+160};
       Vector2 delta{approachAim.x-origin.x,approachAim.y-origin.y};float scale=1e6f;if(delta.x>0)scale=std::min(scale,(479-origin.x)/delta.x);if(delta.y>0)scale=std::min(scale,(319-origin.y)/delta.y);if(delta.y<0)scale=std::min(scale,(1-origin.y)/delta.y);if(scale>1&&scale<1e6f)approachAim={origin.x+delta.x*scale,origin.y+delta.y*scale};
       if(enemy->boss&&nearest<260)approachTilt=0;
      }
     }
     auto selectedName=slot.weaponName(size_t(game.selectedWeapon()));int beforeAmmo=slot.weaponCharge(size_t(game.selectedWeapon())),beforeHP=game.player()->hp;++approachWeaponFrames[selectedName];
     game.touch(approachAim,approachFrames==0?0:1);game.tick(double(approachFrames+1)/60,approachTilt);
     int afterAmmo=0;for(int i=0;i<slot.weaponCount();++i)if(slot.weaponName(size_t(i))==selectedName)afterAmmo=slot.weaponCharge(size_t(i));ammoSpent[selectedName]+=std::max(0,beforeAmmo-afterAmmo);
     int drop=beforeHP-game.player()->hp;if(drop>0){++damageTicks;maxHPDrop=std::max(maxHPDrop,drop);}
     finalPlayer=game.player()->position;for(const auto& e:game.takeEvents())if(e.kind==SessionEventKind::AudioName&&e.text==data.weapons.at(selectedName).fireSFX)++approachShotSounds;
     if(identify()&&(lastHP==0||lastPosition.x-game.player()->position.x<=240)){++approachFrames;break;}
    }
    game.touch({},2);
   }
   if(!target){reason=game.player()&&game.player()->hp<=0?"player_died_during_approach":"target_not_spawned_after_approach";}else{
    std::vector<std::string> weapons={"P_Wpn_Laser","P_Wpn_ElShock","P_Wpn_Missile03","P_Wpn_Missile02","P_Wpn_RaderLsr","P_Wpn_RaderMsl","P_Wpn_Canon03","P_Wpn_Canon02","P_Wpn_Canon01"};
    if(radarPriority)weapons={"P_Wpn_RaderLsr","P_Wpn_RaderMsl","P_Wpn_RaderBomb","P_Wpn_Laser","P_Wpn_ElShock","P_Wpn_Missile03","P_Wpn_Missile02","P_Wpn_Canon03","P_Wpn_Canon02"};
    for(const auto& name:validationWeapons(data))if(name!="P_Wpn_Gun01"&&std::find(weapons.begin(),weapons.end(),name)==weapons.end())weapons.push_back(name);
    weapons.push_back("P_Wpn_Gun01");
    for(frames=0;frames<60*120;++frames){
     auto enemies=game.validationEnemies();auto found=std::find_if(enemies.begin(),enemies.end(),[&](const auto& e){return e.id==target;});
     if(found==enemies.end()){reason="target_disappeared_without_death_observation";break;}
     lastHP=found->hp;lastPosition=found->position;if(found->hp==0&&found->removed){dead=true;reason="killed";break;}
     if(!game.player()||game.player()->hp<=0||game.world().gameState==3){reason="player_died";break;}
     if(game.world().paused){
      if(game.player()->upgradeStep==1){game.confirmUpgrade();game.finishUpgradeSlide();}
      if(game.world().paused){reason="paused_before_target_death";break;}
     }
     int desired=0;bool chosen=false;auto priority=weapons;
     if(strategy.find("-cycle")!=std::string::npos&&radarCycle%2==1){auto pos=std::find(priority.begin(),priority.end(),"P_Wpn_RaderMsl");if(pos!=priority.end())std::rotate(priority.begin(),pos,pos+1);}
     for(const auto& name:priority){for(int index=0;index<slot.weaponCount();++index)if(slot.weaponName(size_t(index))==name&&slot.weaponCharge(size_t(index))>0){desired=index;chosen=true;break;}if(chosen)break;}
     if(game.selectedWeapon()!=desired){game.touch({},2);game.selectWeapon(desired);}
     Vector2 aim{lastPosition.x+(found->bounds.left+found->bounds.right)*.5f-game.world().camera.x+240,lastPosition.y+(found->bounds.bottom+found->bounds.top)*.5f-game.world().camera.y+160};
     // Extend the same aim ray to the screen edge: original near-body touches
     // do not update firing aim. This changes only input, never target geometry.
     Vector2 origin=game.validationAimOrigin();
     const auto& activeWeapon=data.weapons.at(slot.weaponName(size_t(game.selectedWeapon())));
     if(evasive&&activeWeapon.wpnType==5){aim.x+=origin.x-(game.player()->position.x-game.world().camera.x+240);aim.y+=origin.y-(game.player()->position.y-game.world().camera.y+160);}
     Vector2 delta{aim.x-origin.x,aim.y-origin.y};
     // Analytic low ballistic arc is input aiming only; actual spread, muzzle
     // position, discrete gravity, collision and damage remain original code.
     if(activeWeapon.wpnType!=5&&activeWeapon.g>0&&activeWeapon.spd>0&&delta.x>0){
      double v2=double(activeWeapon.spd)*activeWeapon.spd,g=double(activeWeapon.g)*60;
      double discriminant=v2*v2-g*(g*delta.x*delta.x+2*delta.y*v2);
      if(discriminant>=0){delta.y=float(delta.x*(v2-std::sqrt(discriminant))/(g*delta.x));aim={origin.x+delta.x,origin.y+delta.y};}
     }
     float scale=1e6f;
     if(delta.x>0)scale=std::min(scale,(479-origin.x)/delta.x);
     if(delta.y>0)scale=std::min(scale,(319-origin.y)/delta.y);
     if(delta.y<0)scale=std::min(scale,(1-origin.y)/delta.y);
     if(activeWeapon.wpnType!=5&&scale>1&&scale<1e6f)aim={origin.x+delta.x*scale,origin.y+delta.y*scale};
     framesTargetBehind+=delta.x<=0;framesNearAim+=std::fabs(aim.x-origin.x)<=100&&std::fabs(aim.y-origin.y)<=100;
     std::string firedName=slot.weaponName(size_t(game.selectedWeapon()));int ammoBefore=slot.weaponCharge(size_t(game.selectedWeapon()));
     ++weaponFrames[firedName];int priorHP=game.player()->hp;int targetHPBefore=found->hp;
     float distance=lastPosition.x-game.player()->position.x;
     float tilt=distance>300?.1f:distance<140?-.1f:0;
     if(evasive){
      float nearDistance=strategy.starts_with("v6-radar-far")?380.f:160.f,farDistance=strategy.starts_with("v6-radar-far")?440.f:330.f;
      if(distance<nearDistance)moveDirection=-1;else if(distance>farDistance)moveDirection=1;
      tilt=float(moveDirection)*.1f;
      bool groundDistance=strategy.starts_with("v6-radar-close")||strategy.starts_with("v6-radar-mid")||strategy.starts_with("v6-radar-wide");
      if(groundDistance){float desired=strategy.starts_with("v6-radar-close")?220.f:strategy.starts_with("v6-radar-mid")?270.f:350.f;tilt=distance>desired+10?.1f:distance<desired-10?-.1f:0;}
      else if(strategy.starts_with("v6-radar-still"))tilt=0;
      else if(strategy.starts_with("v6-radar-jump"))tilt=distance>300?.1f:distance<140?-.1f:0;
      if(!groundDistance&&!strategy.starts_with("v6-radar-still")&&(game.player()->machState==1||(game.player()->machState==2&&game.player()->velocity.y<0&&game.player()->useBooster<2)))game.booster();
     }else{
      if(frames%70==0&&game.player()->machState==1)game.booster();
      else if(frames%100==0&&game.player()->machState==2&&game.player()->velocity.y<0)game.booster();
     }
     if(evasive){Vector2 clipped{std::clamp(aim.x,1.f,479.f),std::clamp(aim.y,1.f,319.f)};clampedTouches+=clipped.x!=aim.x||clipped.y!=aim.y;aim=clipped;}
     auto hopMarker=strategy.find("-hop");if(hopMarker!=std::string::npos&&frames==std::stoi(strategy.substr(hopMarker+4)))game.booster();
     bool dashed=false;if(strategy.find("-dash")!=std::string::npos&&frames%100==30&&game.player()->dashVel==0){auto pos=game.player()->position;float side=(frames/100)%2==0?-20.f:20.f;game.touch({std::clamp(pos.x-game.world().camera.x+240+side,1.f,479.f),std::clamp(pos.y-game.world().camera.y+180,1.f,319.f)},0);++dashRequests;dashed=true;}
     bool held=strategy.find("-hold")!=std::string::npos;int touchPeriod=strategy.ends_with("hold2")?2:6;
     if(!dashed&&(!held||frames%touchPeriod==0))game.touch(aim,frames==0?0:1);game.tick(double(approachFrames+frames+1)/60,tilt);
     int ammoAfter=0;for(int i=0;i<slot.weaponCount();++i)if(slot.weaponName(size_t(i))==firedName)ammoAfter=slot.weaponCharge(size_t(i));ammoSpent[firedName]+=std::max(0,ammoBefore-ammoAfter);if(ammoAfter<ammoBefore&&(firedName=="P_Wpn_RaderLsr"||firedName=="P_Wpn_RaderMsl"))++radarCycle;
     for(const auto& e:game.validationEnemies())if(e.id==target)damageWhileSelected[firedName]+=std::max(0,targetHPBefore-e.hp);
     finalPlayer=game.player()->position;int hpDrop=priorHP-game.player()->hp;if(hpDrop>0){++damageTicks;maxHPDrop=std::max(maxHPDrop,hpDrop);}
     for(const auto& event:game.takeEvents())if(event.kind==SessionEventKind::AudioName){auto weapon=data.weapons.find(slot.weaponName(size_t(game.selectedWeapon())));if(weapon!=data.weapons.end()&&event.text==weapon->second.fireSFX)++shots;}
    }
   }
  }catch(const std::exception& e){reason="exception: "+std::string(e.what());}
  if(dead)++passed;
  auto emit=[&](std::ostream& out){out<<"{\"strategy\":"<<std::quoted(strategy)<<",\"mission\":"<<stage.mission<<",\"stage\":"<<stage.stage<<",\"map\":"<<std::quoted(stage.mapName)<<",\"bossIndex\":"<<bi<<",\"name\":"<<std::quoted(stage.bosses[bi].name)<<",\"seed\":11,\"grades\":[6,6,6],\"hpInitial\":"<<initialHP<<",\"hpLast\":"<<lastHP<<",\"playerHP\":"<<(game.player()?game.player()->hp:0)<<",\"frames\":"<<frames<<",\"shotSounds\":"<<shots<<",\"passed\":"<<(dead?"true":"false")<<",\"reason\":"<<std::quoted(reason)
   <<",\"dashRequests\":"<<dashRequests<<",\"clampedTouches\":"<<clampedTouches<<",\"sequentialPredecessor\":"<<(sequential?"true":"false")<<",\"initialBossDistance\":"<<initialBossDistance<<",\"approachRequired\":"<<(approachRequired?"true":"false")<<",\"approachFrames\":"<<approachFrames<<",\"initialEnemies\":"<<initialEnemies<<",\"initialBehind240\":"<<initialBehind<<",\"initialPriorBosses\":"<<initialPriorBosses<<",\"targetBehindFrames\":"<<framesTargetBehind<<",\"nearAimFrames\":"<<framesNearAim<<",\"damageTicks\":"<<damageTicks<<",\"maxHPDrop\":"<<maxHPDrop<<",\"spawn\":["<<spawn.x<<","<<spawn.y<<"],\"finalPlayer\":["<<finalPlayer.x<<","<<finalPlayer.y<<"],\"mapBottom\":"<<game.world().map.bottom<<",\"weaponFrames\":{";
   bool first=true;for(const auto& [name,count]:weaponFrames){if(!first)out<<",";first=false;out<<std::quoted(name)<<":"<<count;}out<<"},\"ammoSpent\":{";first=true;for(const auto& [name,count]:ammoSpent){if(!first)out<<",";first=false;out<<std::quoted(name)<<":"<<count;}
   out<<"},\"damageWhileSelected\":{";first=true;for(const auto& [name,count]:damageWhileSelected){if(!first)out<<",";first=false;out<<std::quoted(name)<<":"<<count;}
   int remainingAmmo=0;for(int i=1;i<slot.weaponCount();++i)remainingAmmo+=slot.weaponCharge(size_t(i));out<<"},\"remainingFiniteAmmo\":"<<remainingAmmo<<",\"approachShotSounds\":"<<approachShotSounds<<",\"predecessorDeaths\":[";first=true;for(const auto& e:predecessorDeaths){if(!first)out<<",";first=false;out<<"{\"id\":"<<e.id<<",\"name\":"<<std::quoted(e.name)<<",\"hp\":"<<e.hp<<",\"removed\":"<<(e.removed?"true":"false")<<",\"spawnX\":"<<e.spawnPosition.x<<"}";}out<<"],\"approachWeaponFrames\":{";first=true;for(const auto& [name,count]:approachWeaponFrames){if(!first)out<<",";first=false;out<<std::quoted(name)<<":"<<count;}out<<"}}\n";out.flush();};emit(output);emit(std::cout);
 }
 std::cerr<<"Boss combat passed="<<passed<<" total="<<total<<" failed="<<total-passed<<'\n';return total>0&&passed==total?0:1;
}
#endif
