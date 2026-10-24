#include "../Sources/Core/ValidationScenario.hpp"
#if defined(HM_VALIDATION) && HM_VALIDATION
#include "../Sources/Core/GameSession.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <cmath>
using namespace heavymach;
namespace {
struct ValidationImpactHost : CombatHost {
 int amount=0,hits=0;
 int32_t random31() override{return 0;}
 bool sweepTerrain(Vector2&,const Vector2&) override{return false;}
 bool inCamera(const BulletState&) override{return true;}
 std::optional<CombatTarget> targetAt(bool,Vector2) override{return CombatTarget(1);}
 void damage(CombatTarget,int32_t value) override{amount=value;++hits;}
 bool acquireParticle() override{return false;}
 void event(const CombatEvent&) override{}
 void spawnBullet(const WeaponInfo&,bool,float,Vector2,double) override{}
};
int validationGunImpact(const GameData& data,int grade){
 char name[40];std::snprintf(name,sizeof(name),"P_Wpn_Gun%02d",grade+1);
 auto found=data.weapons.find(name);if(found==data.weapons.end())found=data.weapons.find("E_Default_Wpn");assert(found!=data.weapons.end());
 BulletState bullet;bullet.info=&found->second;CombatFrame frame;frame.weaponUpgrade=uint8_t(grade);
 ValidationImpactHost host;tickBullet(bullet,frame,host);assert(host.hits==1);
 assert(host.amount==found->second.dmg+(found->second.dmg/8)*grade);return host.amount;
}
}
void runValidationScenarioTests(){
 auto resources=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";auto data=GameData::loadDirectory(resources/"RawData");auto stages=validationStages(data);assert(stages.size()==41);
 assert(stages.front().mission==0&&stages.front().stage==0&&stages.front().mapName=="Stage01");assert(stages.back().mission==7&&stages.back().stage==5&&!stages.back().naturalProgression);
 ValidationScenario preset;preset.internalLevel=2;preset.nearLevelUp=true;preset.grades={2,3,4};preset.allWeapons=true;SaveSlot slot;prepareValidationSlot(slot,data,preset);
 assert(slot.level()==2&&slot.experience()==GlobalInfo::thresholds(2).next-1&&slot.bytes[0x2f]==2&&slot.bytes[0x2e]==3&&slot.bytes[0x2d]==4);
 auto available=validationWeapons(data);for(const auto& n:available){bool found=false;for(int i=0;i<slot.weaponCount();++i)found|=slot.weaponName(size_t(i))==n;assert(found);}
 auto before=slot.bytes;auto invalid=preset;invalid.grades[0]=7;bool rejected=false;try{prepareValidationSlot(slot,data,invalid);}catch(const std::invalid_argument&){rejected=true;}assert(rejected&&slot.bytes==before);
 // Missing runtime terrain must reject fixture preparation atomically.
 auto noGroundData=data;noGroundData.maps.at("Stage03").collisionPaths.clear();ValidationScenario noGround;noGround.stage=2;noGround.bossIndex=0;
 auto preservedSlot=slot.bytes;bool noGroundRejected=false;try{prepareValidationSlot(slot,noGroundData,noGround);}catch(const std::runtime_error&){noGroundRejected=true;}assert(noGroundRejected&&slot.bytes==preservedSlot);
 auto dims=[&](std::string_view name){auto p=resources/"Images"/name;if(!std::filesystem::exists(p))p+=".png";std::ifstream f(p,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);assert(f.gcount()==24);auto be=[&](int i){return uint32_t(b[i])<<24|uint32_t(b[i+1])<<16|uint32_t(b[i+2])<<8|b[i+3];};return Vector2{float(be(16)),float(be(20))};};
 uint32_t seed=11;auto rng=[&](){seed=seed*1664525+1013904223;return int32_t(seed>>1);};GameSession game(data,slot,dims,rng);game.startValidation(preset);assert(slot.level()==2&&!game.world().paused);game.takeEvents();
 // Explicit cheat-fixture reset begins with no temporary loot effects. Normal
 // screen and stage changes keep them; those paths have separate continuity tests.
 auto& effects=const_cast<GobHvM_Player*>(game.player())->passives;
 effects.slots[0]={true,40,40};game.startValidation(preset);
 assert(!game.player()->passives.active(0));game.takeEvents();
 game.validationAddExperience(1);assert(slot.level()==3&&game.world().paused);auto events=game.takeEvents();bool opened=false;for(auto& e:events)opened|=e.kind==SessionEventKind::PlayerUI&&e.code==int(PlayerEventKind::UpgradeOpen);assert(opened);
 assert(game.selectUpgrade(0));game.confirmUpgrade();assert(slot.bytes[0x2f]==3);game.finishUpgradeSlide();assert(!game.world().paused);game.tick(1./60,0);bool changedBody=false;for(auto& d:game.sprites())changedBody|=d.tile=="P_Body_04.png";assert(changedBody);
 auto bossStage=std::find_if(stages.begin(),stages.end(),[](const auto& s){return !s.bosses.empty();});assert(bossStage!=stages.end());ValidationScenario boss;boss.mission=bossStage->mission;boss.stage=bossStage->stage;boss.bossIndex=0;game.startValidation(boss);
 assert(game.player()->position.x<bossStage->bosses[0].position.x&&game.enemyCount()>0&&game.world().gameState==0&&game.player()->hp==256);
 int upgradeCases=0;const char* patterns[]={"P_Body_%02d","P_Top_%02d","P_Wpn_Gun%02d"};const size_t offsets[]={0x2f,0x2e,0x2d};
 for(int part=0;part<3;++part)for(int grade=0;grade<6;++grade){ValidationScenario upgrade;upgrade.nearLevelUp=true;upgrade.grades[size_t(part)]=uint8_t(grade);game.startValidation(upgrade);float priorSpeed=game.player()->moveSpeed;int priorHP=game.player()->hp;int priorDamage=validationGunImpact(data,slot.bytes[0x2d]);game.validationAddExperience(1);assert(slot.level()==1&&slot.experience()==300);assert(game.selectUpgrade(part));game.confirmUpgrade();game.finishUpgradeSlide();game.tick(1./60,0);
  char name[40];std::snprintf(name,sizeof(name),patterns[part],grade+2);auto expected=data.parts.at(name).tileName();bool seen=false;for(auto& sprite:game.sprites())seen|=sprite.tile==expected;assert(seen&&slot.bytes[offsets[part]]==grade+1);
  if(part==0){assert(std::fabs(game.player()->moveSpeed-priorSpeed-.15f)<.00001f);game.tick(2./60,.1f);assert(std::fabs(game.player()->speed-game.player()->moveSpeed)<.00001f);}
  if(part==1){assert(game.player()->hp==GlobalInfo::maximumHP(uint8_t(grade+1))&&slot.i32(0x18)==game.player()->hp&&slot.i32(0x1c)==game.player()->hp);if(grade==0)assert(game.player()->hp==priorHP);else assert(game.player()->hp>priorHP);}
  if(part==2){int damage=validationGunImpact(data,slot.bytes[0x2d]);assert(damage>priorDamage);}
  int savedLevel=slot.level(),savedExp=slot.experience();float savedSpeed=game.player()->moveSpeed;int savedHPMax=slot.i32(0x1c);int savedDamage=validationGunImpact(data,slot.bytes[0x2d]);
  SaveData save;save.slots[0]=slot;auto decoded=SaveData::decode(save.encode());assert(decoded.slots[0].level()==savedLevel&&decoded.slots[0].experience()==savedExp);GameSession resumed(data,decoded.slots[0],dims,rng);resumed.start(0,0,0);for(int n=0;n<15;++n)resumed.tick(double(n+1)/60,0);seen=false;for(auto& sprite:resumed.sprites())seen|=sprite.tile==expected;assert(seen&&decoded.slots[0].bytes[offsets[part]]==grade+1);
  assert(decoded.slots[0].level()==savedLevel&&decoded.slots[0].experience()==savedExp);
  assert(resumed.player()->moveSpeed==savedSpeed&&decoded.slots[0].i32(0x1c)==savedHPMax&&validationGunImpact(data,decoded.slots[0].bytes[0x2d])==savedDamage);++upgradeCases;
 }
 ValidationScenario capped;capped.nearLevelUp=true;capped.grades={6,6,6};game.startValidation(capped);
 float cappedSpeed=game.player()->moveSpeed;int cappedHP=game.player()->hp,cappedDamage=validationGunImpact(data,6);
 game.validationAddExperience(1);assert(slot.level()==1&&slot.experience()==300&&game.world().paused);
 for(int part=0;part<3;++part)assert(!game.selectUpgrade(part));
 game.confirmUpgrade();game.finishUpgradeSlide();assert(!game.world().paused);
 assert(slot.bytes[0x2f]==6&&slot.bytes[0x2e]==6&&slot.bytes[0x2d]==6);
 assert(game.player()->moveSpeed==cappedSpeed&&game.player()->hp==cappedHP&&validationGunImpact(data,slot.bytes[0x2d])==cappedDamage);
 size_t bossCount=0,safeBossApproaches=0,rejectedBossApproaches=0;for(const auto& entry:stages){bossCount+=entry.bosses.size();for(size_t b=0;b<entry.bosses.size();++b){ValidationScenario approach;approach.mission=entry.mission;approach.stage=entry.stage;approach.bossIndex=b;bool rejectedApproach=false;try{assert(validationSpawnPosition(data,approach).has_value());++safeBossApproaches;}catch(const std::runtime_error&){rejectedApproach=true;++rejectedBossApproaches;}
  bool noSafeGround=false; // Whole-map fallback now finds stable starts for all 48 authored placements.
  assert(rejectedApproach==noSafeGround);
  if(rejectedApproach){auto preserved=slot.bytes;bool failed=false;try{game.startValidation(approach);}catch(const std::runtime_error&){failed=true;}assert(failed&&slot.bytes==preserved);}
}ValidationScenario every;every.mission=entry.mission;every.stage=entry.stage;game.startValidation(every);for(int n=0;n<3;++n)game.tick(double(n+1)/60,0);assert(game.player());}
 assert(safeBossApproaches==48&&rejectedBossApproaches==0);
 int weaponsChecked=0,ammoWeapons=0,trailWeapons=0,effectWeapons=0;
 for(const auto& name:available){
  ValidationScenario weaponScenario;weaponScenario.allWeapons=true;game.startValidation(weaponScenario);
  int index=-1;for(int n=0;n<slot.weaponCount();++n)if(slot.weaponName(size_t(n))==name)index=n;assert(index>=0);
  game.selectWeapon(index);assert(game.selectedWeapon()==index);game.takeEvents();
  const auto& info=data.weapons.at(name);int beforeAmmo=slot.weaponCharge(size_t(index));bool bullet=false,sound=false,effect=false,body=false,trail=false,released=false;
  for(int frame=0;frame<900;++frame){
   if(!released)game.touch({450,180},frame==0?0:1);
   game.tick(double(frame+1)/60,0);bullet|=game.bulletCount()>0;
   for(const auto& e:game.takeEvents())sound|=e.kind==SessionEventKind::AudioName&&e.text==info.fireSFX;
   for(const auto& item:game.renderItems()){
    if(const auto* sprite=std::get_if<DrawSprite>(&item)){body|=sprite->tile==info.bulletTile;effect|=sprite->tile==info.effectTile;}
    else if(const auto* mesh=std::get_if<TrailMesh>(&item)){trail|=mesh->tile==info.effectTile&&!mesh->vertices.empty();effect|=trail;}
   }
   if(bullet&&!released){game.touch({450,180},2);released=true;}
   bool consumed=index==0?bullet:slot.weaponCharge(size_t(index))<beforeAmmo;
   bool expectedEffect=info.effectTypeNumber()>0&&!info.effectTile.empty()&&info.effectTile!="none";
   bool expectedBody=!info.bulletTile.empty()&&info.bulletTile!="none";
   if(bullet&&sound&&consumed&&(!expectedEffect||effect)&&(!expectedBody||body))break;
  }
  std::cout<<"Weapon "<<name<<" index="<<index<<" bullets="<<bullet<<" sound="<<sound<<" body="<<body<<" effect="<<effect<<" trail="<<trail<<" ammo="<<beforeAmmo<<"->"<<slot.weaponCharge(size_t(index))<<std::endl;
  assert(bullet&&sound);if(!info.bulletTile.empty()&&info.bulletTile!="none")assert(body);
  if(info.effectTypeNumber()>0&&!info.effectTile.empty()&&info.effectTile!="none"){assert(effect);++effectWeapons;}
  if(info.effectTypeNumber()==4||info.effectTypeNumber()==5){assert(trail);++trailWeapons;}
  int afterAmmo=slot.weaponCharge(size_t(index));if(index==0)assert(afterAmmo==beforeAmmo);else {assert(afterAmmo==beforeAmmo-1);++ammoWeapons;}
  SaveData weaponSave;weaponSave.slots[0]=slot;auto reloaded=SaveData::decode(weaponSave.encode());assert(reloaded.slots[0].weaponName(size_t(index))==name&&reloaded.slots[0].weaponCharge(size_t(index))==afterAmmo);
  GameSession restoredWeapon(data,reloaded.slots[0],dims,rng);restoredWeapon.start(0,0,0);restoredWeapon.selectWeapon(index);assert(restoredWeapon.selectedWeapon()==index&&reloaded.slots[0].weaponCharge(size_t(index))==afterAmmo);
  bool resumedBullet=false,resumedSound=false,resumedReleased=false;restoredWeapon.takeEvents();
  for(int frame=0;frame<900;++frame){if(!resumedReleased)restoredWeapon.touch({450,180},frame==0?0:1);restoredWeapon.tick(double(frame+1)/60,0);resumedBullet|=restoredWeapon.bulletCount()>0;
   for(const auto& e:restoredWeapon.takeEvents())resumedSound|=e.kind==SessionEventKind::AudioName&&e.text==info.fireSFX;
   if(resumedBullet&&resumedSound&&!resumedReleased){restoredWeapon.touch({450,180},2);resumedReleased=true;}
   if(resumedBullet&&resumedSound&&(index==0||reloaded.slots[0].weaponCharge(size_t(index))==afterAmmo-1))break;
  }
  assert(resumedBullet&&resumedSound&&reloaded.slots[0].weaponCharge(size_t(index))==(index==0?afterAmmo:afterAmmo-1));++weaponsChecked;
 }
 // Cheating is explicit and does not leak into a fresh fixture. In particular,
 // a single remaining round must be restored before RemoveWeapon fires.
 ValidationScenario cheatFixture;cheatFixture.allWeapons=true;cheatFixture.grades={6,6,6};
 game.startValidation(cheatFixture);int cheatWeapon=1;auto cheatName=slot.weaponName(cheatWeapon);
 slot.setWeapon(cheatWeapon,cheatName,1);game.selectWeapon(cheatWeapon);game.validationSetCheats(true);
 game.touch({470,160},0);
 for(int n=1;n<=900&&game.validationCheatStats().refilledRounds<3;++n)game.tick(n/60.,0);
 assert(game.validationCheatStats().refilledRounds>=3&&game.selectedWeapon()==cheatWeapon&&slot.weaponCharge(cheatWeapon)==1);
 game.validationSetCheats(false);
 for(int n=901;n<=1800&&game.selectedWeapon()!=0;++n)game.tick(n/60.,0);
 assert(game.selectedWeapon()==0);
 ValidationScenario damageFixture;damageFixture.stage=2;damageFixture.bossIndex=0;
 game.startValidation(damageFixture);game.validationSetCheats(true);int cheatHP=game.player()->hp;
 int cheatTick=1;
 for(;cheatTick<=3600&&game.validationCheatStats().blockedHits==0;++cheatTick)game.tick(cheatTick/60.,0);
 assert(game.validationCheatStats().blockedHits>0&&game.player()->hp==cheatHP);
 game.validationSetCheats(false);
 for(int end=cheatTick+3600;cheatTick<=end&&game.player()&&game.player()->hp==cheatHP;++cheatTick)game.tick(cheatTick/60.,0);
 assert(!game.player()||game.player()->hp<cheatHP);
 game.validationSetCheats(true);game.startValidation(damageFixture);cheatHP=game.player()->hp;
 for(int n=1;n<=3600&&game.player()&&game.player()->hp==cheatHP;++n)game.tick(n/60.,0);
 assert(game.validationCheatStats().blockedHits==0&&(!game.player()||game.player()->hp<cheatHP));
 std::cout<<"Validation cheats: invulnerability on/off, one-round ammo, fixture reset passed\n";
 std::cout<<"Validation weapons="<<weaponsChecked<<" consuming="<<ammoWeapons<<" effects="<<effectWeapons<<" trails="<<trailWeapons<<std::endl;
 std::cout<<"Validation upgrades="<<upgradeCases<<" stages="<<stages.size()<<" bosses="<<bossCount<<" safeBossApproaches="<<safeBossApproaches<<" rejectedBossApproaches="<<rejectedBossApproaches<<" level/EXP restore=18 ability checks=18 cap=3"<<"\n";

}
int main(){runValidationScenarioTests();}
#endif
