#include "../Sources/Core/GameSession.hpp"
#include "../Sources/Core/CameraShake.hpp"
#include "JourneyUpgradeHost.hpp"
#if defined(HM_VALIDATION) && HM_VALIDATION
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <map>
using namespace heavymach;
// Independent starts, never a claim of a single continuous campaign.
// Default: initial fixture only. Explicit final arg cheats enables damage blocking
// and ammunition replenishment; results never count as normal-difficulty clears.
int main(int argc,char** argv){
 const auto resources=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";
 auto data=GameData::loadDirectory(resources/"RawData");
 auto dimensions=[&](std::string_view name){auto p=resources/"Images"/name;if(!std::filesystem::exists(p))p+=".png";std::ifstream f(p,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);if(f.gcount()!=24)throw std::runtime_error("missing original image "+p.string());auto be=[&](int i){return uint32_t(b[i])<<24|uint32_t(b[i+1])<<16|uint32_t(b[i+2])<<8|b[i+3];};return Vector2{float(be(16)),float(be(20))};};
 std::ofstream output(argc>1?argv[1]:"validation-journeys.jsonl");if(!output)return 2;
 std::string filter=argc>2?argv[2]:"";int limit=argc>3?std::stoi(argv[3]):18000;
 std::ofstream trace;if(argc>4){trace.open(argv[4]);trace<<"map,tick,x,y,vx,vy,hp,mach_state,boost_used,ground,ahead,front_line,boss_count,boss_encounter,weapon,ammo,aim_x,aim_y,tilt,target,target_x,target_y,target_hp,target_boss,rng_calls,camera_x,camera_y,alive_enemies\n";}
 // Legacy mode is retained only for reproducing archived zero-offset runs.
 const bool hostCamera=argc<=6||std::string_view(argv[6])!="legacy-zero-camera";
 const bool hostHUD=argc<=7||std::string_view(argv[7])!="legacy-early-hud";
 const bool cheats=argc>8&&std::string_view(argv[8])=="cheats";
 const std::string requestedStrategy=argc>5?argv[5]:"baseline-ui";
 // Fixed from prior runs, before this complete 35-map execution; no runtime retry.
 const std::map<std::string,std::string> selectedPolicies={
  {"Stage01","boss-recovery-ui"},
  {"Stage02","boss-recovery-ui"},
  {"Stage03","boss-recovery-ui"},
  {"Stage04","boss-recovery-ui"},
  {"Stage05","boss-recovery-ui"},
  {"Stage06","boss-recovery-ui"},
  {"Stage07","boss-recovery-ui"},
  {"Stage08","journey-dash"},
  {"Stage09","boss-recovery-ui"},
  {"Stage10","journey-full-close-hold6"},
  {"Stage11","boss-recovery-ui"},
  {"Stage12","boss-recovery-ui"},
  {"Stage13","boss-recovery-ui"},
  {"Stage14","boss-recovery-ui"},
  {"Stage15","boss-recovery-ui"},
  {"Stage16","baseline-ui"},
  {"Stage17","baseline-ui"},
  {"Stage18","journey-full-still-hold2"},
  {"Stage19","journey-evasive"},
  {"Stage20","journey-periodic-dash"},
  {"Stage21","baseline-ui"},
  {"Stage22","journey-evasive-armed-hold2"},
  {"Stage23","baseline-ui"},
  {"Stage24","baseline-ui"},
  {"Stage25","boss-recovery-ui"},
  {"Stage26","boss-recovery-ui"},
  {"Stage27","boss-recovery-ui"},
  {"Stage28","journey-full-still-hold2"},
  {"Stage29","boss-recovery-ui"},
  {"Stage30","journey-evasive"},
  {"Stage31","baseline-ui"},
  {"Stage32","boss-recovery-ui"},
  {"Stage33","journey-full-still-hold2"},
  {"Stage34","baseline-ui"},
  {"Stage35","baseline-ui"},
 };
 int total=0,passed=0;
 for(const auto& entry:validationStages(data)){
  if((filter=="normal"||requestedStrategy=="selected-35")&&!entry.naturalProgression)continue;
  if(filter!="normal"&&!filter.empty()&&(","+filter+",").find(","+entry.mapName+",")==std::string::npos)continue;
 const std::string strategy=requestedStrategy=="selected-35"?selectedPolicies.at(entry.mapName):requestedStrategy;
 const bool bossPriority=cheats||strategy=="boss-priority",bossRecovery=strategy.starts_with("boss-recovery");
 const bool journey=strategy.starts_with("journey-");
 const bool fullRadar=strategy.find("full")!=std::string::npos;
 const bool continuousBoost=journey&&strategy.find("still")==std::string::npos&&strategy.find("close")==std::string::npos&&strategy.find("periodic")==std::string::npos;
 const bool originalUpgradeSlide=strategy=="boss-recovery-ui"||strategy=="baseline-ui"||journey;
  ++total;SaveSlot slot;uint32_t seed=11;size_t rngCalls=0;
  auto rng=[&]{++rngCalls;seed=seed*1664525u+1013904223u;return int32_t(seed>>1);};
  GameSession game(data,slot,dimensions,rng);ValidationScenario scenario;
  scenario.mission=entry.mission;scenario.stage=entry.stage;scenario.grades={6,6,6};scenario.allWeapons=true;
  std::string status="timeout",error;int ticks=0,clearTick=0,upgradeConfirmations=0,boosters=0,shotEvents=0,menu=-1;
  bool savedResume=false;bool cleared=false,died=false,saveEvent=false,transitionRequested=false;Vector2 initial{},final{};int hp=0,initialHP=0,minimumHP=0,clearHP=-1,initialAmmo=0,finalAmmo=0;
  int quakeEvents=0,nonzeroCameraTicks=0,upgradeWaitTicks=0,upgradeCloseCallbacks=0;float maxCameraOffset=0,resultButtonY=-1000;
  try{
   game.startValidation(scenario);game.validationSetCheats(cheats);initial=game.player()->position;initialHP=minimumHP=game.player()->hp;
   if(initial.x!=data.maps.at(entry.mapName).left+20)throw std::runtime_error("fixture did not use normal left spawn");
   for(int i=1;i<slot.weaponCount();++i)initialAmmo+=slot.weaponCharge(size_t(i));
   CameraShake cameraShake;JourneyUpgradeHost upgradeHUD;
   auto consumeEvents=[&](double globalTime){
    for(const auto& e:game.takeEvents()){
     if(e.kind==SessionEventKind::Unsupported)throw std::runtime_error("unsupported: "+e.text);
     if(hostHUD&&e.kind==SessionEventKind::PlayerUI){
      if(e.code==int(PlayerEventKind::UpgradeOpen))upgradeHUD.open();
      else if(e.code==int(PlayerEventKind::UpgradeClose))upgradeHUD.close();
     }
     if(hostCamera&&e.kind==SessionEventKind::PlayerUI&&e.code==int(PlayerEventKind::Quake)){
      cameraShake.setQuake(e.value,e.amount,e.delay,globalTime);++quakeEvents;
     }
     if(hostCamera&&e.kind==SessionEventKind::WorldUI&&e.code==int(WorldEventKind::Quake)){
      cameraShake.setQuake(10,4,.3f,globalTime);++quakeEvents;
     }
     if(e.kind==SessionEventKind::SaveChanged)saveEvent=true;
     if(e.kind==SessionEventKind::AudioName&&e.text.starts_with("Wpn_"))++shotEvents;
     if(e.kind==SessionEventKind::WorldUI&&e.code==int(WorldEventKind::ChangeScreen))menu=e.index;
    }
   };
   // Initialization is not evidence of transition save progress. Only retain
   // any actual opening event for the HUD lifecycle.
   for(const auto& e:game.takeEvents())if(hostHUD&&e.kind==SessionEventKind::PlayerUI&&e.code==int(PlayerEventKind::UpgradeOpen))upgradeHUD.open();
   bool held=false,recoveringBoss=false;float upgradeX=-240;bool upgradeClosing=false;int moveDirection=1,bossFightTicks=0;uintptr_t fightID=0;bool encounteredBoss=false;
   for(ticks=1;ticks<=limit;++ticks){
    const auto* player=game.player();if(player&&!cleared)minimumHP=std::min(minimumHP,player->hp);if(!cleared&&!died&&(!player||player->hp<=0||game.world().gameState==3)){died=true;status="player_died";}
    if(game.world().gameState==1){cleared=true;if(!clearTick){clearTick=ticks;clearHP=player?player->hp:-1;}
     if(!transitionRequested&&game.world().presentation.step==2&&(!hostHUD||game.world().presentation.buttonY>-1.f)){resultButtonY=26+game.world().presentation.buttonY;game.touch({},2);held=false;game.resultButton(game.world().presentation.missionEnd?1000:1001);transitionRequested=true;}
    }
    float tilt=0;
    if(game.world().paused){if(player&&!died&&player->upgradeStep==1){
     if(hostHUD&&!upgradeHUD.readyForConfirmation())++upgradeWaitTicks;
     else {game.confirmUpgrade();if(!hostHUD){if(originalUpgradeSlide||(strategy=="boss-recovery-result-ui"&&cleared)){upgradeX=-240;upgradeClosing=true;}else game.finishUpgradeSlide();}++upgradeConfirmations;}
    }}
    else if(!cleared&&!died){
     if(game.world().bossEncounter>0)encounteredBoss=true;
     const bool escaping=strategy.find("exitdash")!=std::string::npos&&encounteredBoss&&game.world().bossEncounter==0;
     auto enemies=game.validationEnemies();const GameSession::ValidationEnemySnapshot* target=nullptr;float nearest=650;
     for(const auto& enemy:enemies)if(!enemy.removed&&enemy.hp>0){float dx=enemy.position.x-player->position.x;if(dx>(journey?0:-120)&&dx<nearest){nearest=dx;target=&enemy;}}
     // Fixed optional controller policy; no world/health/ammo mutation.
     if(bossRecovery)for(const auto& e:enemies)if(e.boss&&!e.removed&&e.hp>0&&e.position.x-player->position.x<-100&&e.position.x-player->position.x>-650)recoveringBoss=true;
     if(bossPriority||recoveringBoss||journey){float closest=650;for(const auto& e:enemies)if(e.boss&&!e.removed&&e.hp>0){float dx=e.position.x-player->position.x;if(std::fabs(dx)<closest){closest=std::fabs(dx);nearest=dx;target=&e;}}}
     int desired=0;
     if(target&&target->boss){bool chosen=false;for(const char* name:{"P_Wpn_RaderLsr","P_Wpn_RaderMsl","P_Wpn_RaderBomb","P_Wpn_Laser","P_Wpn_ElShock","P_Wpn_Missile03","P_Wpn_Missile02","P_Wpn_Canon03","P_Wpn_Canon02"}){if(cheats&&std::string_view(name)=="P_Wpn_RaderLsr")continue;if(std::string_view(name)=="P_Wpn_RaderBomb"&&!fullRadar)continue;for(int i=1;i<slot.weaponCount();++i)if(slot.weaponName(size_t(i))==name&&slot.weaponCharge(size_t(i))>0){desired=i;chosen=true;break;}if(chosen)break;}}
     if(journey&&target&&!target->boss&&strategy.find("armed")!=std::string::npos){bool chosen=false;for(const char* name:{"P_Wpn_Gun07","P_Wpn_Gun06","P_Wpn_Gun05","P_Wpn_Gun04","P_Wpn_Gun03","P_Wpn_Gun02","P_Wpn_Shotgun02","P_Wpn_Shotgun01"}){for(int i=1;i<slot.weaponCount();++i)if(slot.weaponName(size_t(i))==name&&slot.weaponCharge(size_t(i))>0){desired=i;chosen=true;break;}if(chosen)break;}}
     if(journey&&target&&!target->boss&&(strategy.find("laser")!=std::string::npos||escaping)){bool chosen=false;for(const char* name:{"P_Wpn_Laser","P_Wpn_ElShock","P_Wpn_Canon03","P_Wpn_Missile03","P_Wpn_Gun07","P_Wpn_Gun06","P_Wpn_Gun05"}){for(int i=1;i<slot.weaponCount();++i)if(slot.weaponName(size_t(i))==name&&slot.weaponCharge(size_t(i))>0){desired=i;chosen=true;break;}if(chosen)break;}}
     if(game.selectedWeapon()!=desired){game.touch({},2);held=false;game.selectWeapon(desired);}
     Vector2 origin=game.validationAimOrigin();Vector2 aim{479,origin.y};
     if(target){aim={target->position.x+(target->bounds.left+target->bounds.right)*.5f-game.world().camera.x+240-cameraShake.offset.x,target->position.y+(target->bounds.bottom+target->bounds.top)*.5f-game.world().camera.y+160-cameraShake.offset.y};
      const auto& weapon=data.weapons.at(slot.weaponName(size_t(game.selectedWeapon())));
      if(weapon.wpnType==5){aim.x+=origin.x-(player->position.x-game.world().camera.x+240-cameraShake.offset.x);aim.y+=origin.y-(player->position.y-game.world().camera.y+160-cameraShake.offset.y);}
      Vector2 delta{aim.x-origin.x,aim.y-origin.y};
      if(weapon.wpnType!=5&&weapon.g>0&&weapon.spd>0&&delta.x>0){double v2=double(weapon.spd)*weapon.spd,g=double(weapon.g)*60;double disc=v2*v2-g*(g*double(delta.x)*delta.x+2*double(delta.y)*v2);if(disc>=0){float slope=float((v2-std::sqrt(disc))/(g*delta.x));delta.y=delta.x*slope;}}
      float extent=std::max(std::fabs(delta.x),std::fabs(delta.y));if(extent>0){float length=std::min(delta.x>0?(479-origin.x)/delta.x:delta.x<0?(1-origin.x)/delta.x:10000.f,delta.y>0?(319-origin.y)/delta.y:delta.y<0?(1-origin.y)/delta.y:10000.f);if(length>0)aim={origin.x+delta.x*length,origin.y+delta.y*length};}
     }
     aim.x=std::clamp(aim.x,1.f,479.f);aim.y=std::clamp(aim.y,1.f,319.f);
     if(!held||ticks%(journey?(strategy.ends_with("hold2")?2:strategy.ends_with("hold6")?6:1):6)==0){game.touch(aim,held?1:0);held=true;}
     // Advance normally; hold boss distance until its original boundary opens.
     tilt=target&&target->boss&&nearest<260?((bossPriority||recoveringBoss)&&nearest<180?-.1f:0.f):.1f;
     if(journey&&target&&target->boss){if(nearest<160)moveDirection=-1;else if(nearest>330)moveDirection=1;tilt=float(moveDirection)*.1f;}
     if(journey&&strategy.find("dash")!=std::string::npos&&strategy.find("exitdash")==std::string::npos&&ticks%60==30&&player->dashVel==0&&(!target||!target->boss||nearest>450))game.touch({player->position.x-game.world().camera.x+260-cameraShake.offset.x,player->position.y-game.world().camera.y+180-cameraShake.offset.y},0);
     if(journey&&strategy.find("cautious")!=std::string::npos&&target&&!target->boss)tilt=nearest>250?.1f:nearest<140?-.1f:0;
     const bool stillBoss=journey&&(strategy.find("still")!=std::string::npos||strategy.find("close")!=std::string::npos||strategy.find("guard")!=std::string::npos)&&target&&target->boss&&(strategy.find("anchor")!=std::string::npos?(nearest<180||fightID==target->id):nearest<330);
     if(stillBoss){tilt=strategy.find("near")!=std::string::npos?(nearest>190?.1f:nearest<170?-.1f:0):strategy.find("close")!=std::string::npos?(nearest>230?.1f:nearest<210?-.1f:0):0;if(fightID!=target->id){fightID=target->id;bossFightTicks=0;}++bossFightTicks;if(bossFightTicks==210&&strategy.find("nohop")==std::string::npos){game.booster();++boosters;}}
     if(escaping){tilt=.1f;if(player->dashVel==0)game.touch({player->position.x-game.world().camera.x+260-cameraShake.offset.x,player->position.y-game.world().camera.y+180-cameraShake.offset.y},0);}
     const float ahead=game.world().mapHeight(player->position.x+55);
     if(trace&&ticks%60==0){trace<<entry.mapName<<','<<ticks<<','<<player->position.x<<','<<player->position.y<<','<<player->velocity.x<<','<<player->velocity.y<<','<<player->hp<<','<<player->machState<<','<<player->useBooster<<','<<game.world().mapHeight(player->position.x)<<','<<ahead<<','<<game.world().frontLine<<','<<game.world().bossCount<<','<<game.world().bossEncounter<<','<<slot.weaponName(size_t(game.selectedWeapon()))<<','<<slot.weaponCharge(size_t(game.selectedWeapon()))<<','<<aim.x<<','<<aim.y<<','<<tilt<<','<<(target?target->name:"")<<','<<(target?target->position.x:0)<<','<<(target?target->position.y:0)<<','<<(target?target->hp:0)<<','<<(target?target->boss:false)<<','<<rngCalls<<','<<cameraShake.offset.x<<','<<cameraShake.offset.y<<',';
      for(const auto& e:enemies)if(!e.removed&&e.hp>0)trace<<e.name<<':'<<e.position.x<<':'<<e.position.y<<':'<<e.hp<<':'<<e.boss<<';';trace<<'\n';}

     if((continuousBoost||escaping)&&!stillBoss&&(player->machState==1||(player->machState==2&&player->velocity.y<0&&player->useBooster<2))){game.booster();++boosters;}
     else if(!continuousBoost&&!stillBoss&&player->machState==1&&((ahead==10000||ahead>player->position.y+20)||ticks%100==0)){game.booster();++boosters;}
     else if(!continuousBoost&&!stillBoss&&player->machState==2&&player->velocity.y<0&&player->useBooster<2&&ticks%60==0){game.booster();++boosters;}
    }
    if(!hostHUD&&upgradeClosing){upgradeX+=(660-upgradeX)/5;if(upgradeX>659){game.finishUpgradeSlide();upgradeClosing=false;}}
    const auto beforeTickPlayer=game.player();const int beforeTickHP=beforeTickPlayer?beforeTickPlayer->hp:0;const Vector2 beforeTickPosition=beforeTickPlayer?beforeTickPlayer->position:Vector2{};
    // Input occurs between frames, using the previous displayed camera offset.
    // Native HMGameView advances the quake on global time even when World pauses,
    // then sets the offset before GameSession::tick and consumes events afterward.
    const double globalTime=double(ticks)/60;
    if(hostCamera){cameraShake.tick(globalTime);game.setCameraOffset(cameraShake.offset);
     const float magnitude=std::hypot(cameraShake.offset.x,cameraShake.offset.y);
     if(magnitude>0)++nonzeroCameraTicks;maxCameraOffset=std::max(maxCameraOffset,magnitude);
    }
    // Button events are delivered before the following World tick. World
    // events are delivered before GuiGame tick; no draw is needed to sync.
    if(hostHUD)consumeEvents(globalTime);
    game.tick(globalTime,tilt);

    if(trace&&game.player()&&game.player()->hp<beforeTickHP)std::cout<<"damage map="<<entry.mapName<<" tick="<<ticks<<" before="<<beforeTickHP<<" after="<<game.player()->hp<<" x="<<game.player()->position.x<<" y="<<game.player()->position.y<<" prior_y="<<beforeTickPosition.y<<" bottom="<<game.world().map.bottom<<" state="<<game.player()->machState<<" boost="<<game.player()->useBooster<<"\n";
    if(trace&&cleared&&ticks<clearTick+80)std::cout<<"transition tick="<<ticks<<" state="<<game.world().gameState<<" paused="<<game.world().paused<<" step="<<game.world().presentation.step<<" upgrade="<<(game.player()?game.player()->upgradeStep:-1)<<" exp="<<slot.experience()<<" level="<<slot.level()<<"\n";
    consumeEvents(globalTime);
    if(hostHUD&&upgradeHUD.tick()){game.finishUpgradeSlide();++upgradeCloseCallbacks;}
    if(died&&menu>=0)break;
    if(cleared&&transitionRequested){
     if(menu>=0){status=menu==4?"clear_mission_menu":"unexpected_menu";break;}
     if(!game.world().paused&&game.world().gameState==0&&(hostHUD?upgradeHUD.step==0:!upgradeClosing)){status="clear_next_stage";break;}
    }
   }
   if(game.player()){final=game.player()->position;hp=game.player()->hp;}
   for(int i=1;i<slot.weaponCount();++i)finalAmmo+=slot.weaponCharge(size_t(i));
   if(status=="clear_next_stage"||status=="clear_mission_menu"){
    auto expected=GlobalInfo::nextStage({entry.mission,entry.stage});
    if(game.stage().mission!=expected.mission||game.stage().stage!=expected.stage)throw std::runtime_error("wrong next stage");
    int previous=GlobalInfo::currentStage({entry.mission,entry.stage}),next=GlobalInfo::currentStage(expected);
    auto savedPosition=next>previous?expected:StagePosition{entry.mission,entry.stage};
    if(slot.i32(0xc)!=savedPosition.mission||slot.i32(0x10)!=savedPosition.stage||slot.i32(0x14)!=std::max(previous,next)||!saveEvent)throw std::runtime_error("wrong saved progress");
    if(status=="clear_next_stage"&&!game.player())throw std::runtime_error("next stage player missing");
   }
   // Round-trip only the resulting bytes; never apply another fixture for resume.
   SaveData saved;saved.slots[0]=slot;auto restored=SaveData::decode(saved.encode());if(restored.slots[0].bytes!=slot.bytes)throw std::runtime_error("save roundtrip mismatch");
   if(status=="clear_next_stage"||status=="clear_mission_menu"){
    if(cheats&&game.validationCheatStats().bossKills!=entry.bosses.size())throw std::runtime_error("not all placed bosses killed");
    auto& reloaded=restored.slots[0];int mission=reloaded.i32(0xc),stageIndex=reloaded.i32(0x10),farthest=reloaded.i32(0x14);
    GameSession resumed(data,reloaded,dimensions,[]{return int32_t(0);});
    resumed.start(0,mission,stageIndex); // Ordinary entry, no fixture or cheats.
    if(!resumed.player()||resumed.stage().mission!=mission||resumed.stage().stage!=stageIndex||reloaded.i32(0x14)!=farthest)throw std::runtime_error("ordinary saved resume failed");
    for(const auto& e:resumed.takeEvents())if(e.kind==SessionEventKind::Unsupported)throw std::runtime_error("resume unsupported: "+e.text);
    savedResume=true;
   }
  }catch(const std::exception& e){status="exception";error=e.what();}
  const bool ok=status=="clear_next_stage"||status=="clear_mission_menu";passed+=ok;
  auto next=game.stage();auto cheatStats=game.validationCheatStats();
  output<<std::fixed<<std::setprecision(3)<<"{\"saved_resume\":"<<(savedResume?"true":"false")<<",\"cheats\":"<<(cheats?"true":"false")<<",\"blocked_hits\":"<<cheatStats.blockedHits<<",\"fall_rescues\":"<<cheatStats.fallRescues<<",\"refilled_rounds\":"<<cheatStats.refilledRounds<<",\"boss_kills\":"<<cheatStats.bossKills<<",\"expected_bosses\":"<<entry.bosses.size()<<",\"map\":\""<<entry.mapName<<"\",\"mission\":"<<entry.mission<<",\"stage\":"<<entry.stage<<",\"natural_progression\":"<<(entry.naturalProgression?"true":"false")<<",\"strategy\":\""<<strategy<<"\",\"independent_start\":true,\"status\":\""<<status<<"\",\"ticks\":"<<std::min(ticks,limit)<<",\"clear_tick\":"<<clearTick<<",\"spawn_x\":"<<initial.x<<",\"spawn_y\":"<<initial.y<<",\"final_x\":"<<final.x<<",\"initial_hp\":"<<initialHP<<",\"minimum_hp\":"<<minimumHP<<",\"clear_hp\":"<<clearHP<<",\"hp\":"<<hp<<",\"exp\":"<<slot.experience()<<",\"level\":"<<slot.level()<<",\"next_mission\":"<<next.mission<<",\"next_stage\":"<<next.stage<<",\"saved_mission\":"<<slot.i32(0xc)<<",\"saved_stage\":"<<slot.i32(0x10)<<",\"farthest\":"<<slot.i32(0x14)<<",\"menu\":"<<menu<<",\"save_event\":"<<(saveEvent?"true":"false")<<",\"initial_finite_ammo\":"<<initialAmmo<<",\"final_finite_ammo\":"<<finalAmmo<<",\"shot_events\":"<<shotEvents<<",\"booster_requests\":"<<boosters<<",\"upgrade_confirmations\":"<<upgradeConfirmations<<",\"rng_calls\":"<<rngCalls<<",\"camera_host\":"<<(hostCamera?"true":"false")<<",\"quake_events\":"<<quakeEvents<<",\"nonzero_camera_ticks\":"<<nonzeroCameraTicks<<",\"max_camera_offset\":"<<maxCameraOffset<<",\"hud_host\":"<<(hostHUD?"true":"false")<<",\"upgrade_wait_ticks\":"<<upgradeWaitTicks<<",\"upgrade_close_callbacks\":"<<upgradeCloseCallbacks<<",\"result_button_y\":"<<resultButtonY<<",\"error\":"<<std::quoted(error)<<"}\n";output.flush();
  std::cout<<entry.mapName<<" "<<status<<" ticks="<<ticks<<" hp="<<hp<<" x="<<final.x<<"\n";
 }
 std::cout<<"Independent stage journeys "<<passed<<"/"<<total<<"; not a continuous campaign\n";
 return total>0&&passed==total?0:1;
}
#endif
