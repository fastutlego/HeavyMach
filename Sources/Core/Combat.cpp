#include "Combat.hpp"
#include <cmath>
#include <numbers>
#include <stdexcept>
#pragma STDC FP_CONTRACT OFF
namespace heavymach {
namespace {
constexpr float dt=0x1.111112p-6f; // original 0x3c888889
float rnd(CombatHost& h){const int32_t x=h.random31();if(x<0)throw std::invalid_argument("random31 must be nonnegative");return static_cast<float>(x)*0x1p-31f;}
void remove(BulletState& b,CombatHost& h){b.removed=true;h.event({CombatEventKind::RemoveBullet});}
void aiTail(WeaponState& w,const CombatFrame& f,CombatHost& h){if(f.machType!=3&&f.ownerInScreen)processWeaponAI(w,f,h);}
CombatEvent at(CombatEventKind k,Vector2 pos){CombatEvent e{k};e.pos=pos;return e;}
}
// 0x9dc4. Three independent random draws; original aim spread is [-aimRand,0].
void setBulletFire(BulletState& b,const WeaponInfo& info,float angle,Vector2 pos,double time,CombatHost& h){
 b.info=&info;
 float speed=info.spd+(info.spd*(rnd(h)*info.spdRand));
 speed=speed+(info.spd*((-rnd(h))*info.spdRand));
 angle=angle+((rnd(h)*info.aimRand)-info.aimRand);
 b.vel={speed*std::cos(angle),speed*std::sin(angle)};b.pos=pos;b.rotate=angle;b.startTime=static_cast<float>(time);
 if(info.effectTypeNumber()==4||info.effectTypeNumber()==5){
  auto e=at(CombatEventKind::TrailCreate,pos);e.tile=info.effectTile;e.blend=2;e.blocks=128;e.fadeDelay=.1f;e.fadeTime=.8f;e.diffuse=true;
  e.wind=info.effectTypeNumber()==4;e.width=e.wind?8.f:4.f;e.length=e.wind?24.f:32.f;h.event(e);
 }
 b.effectDelay=b.toPlayer?.06f:.04f;
}
// 0xa198. Impact sound is conditional on the original particle pool succeeding.
void collideBullet(BulletState& b,bool targetHit,const CombatFrame& f,CombatHost& h){
 if(!b.info)throw std::logic_error("Bullet has no WeaponInfo");
 if(b.info->effectTypeNumber()!=1&&h.acquireParticle()){
  auto e=at(targetHit?CombatEventKind::TargetExplosion:CombatEventKind::TerrainExplosion,b.pos);
  e.life=targetHit?.6f:.8f;e.asset=targetHit?"Efx_Explode01.png":"Efx_Explode02.png";e.animationFrames=targetHit?20:24;
  if(!targetHit)e.pos.y+=40.f;h.event(e);
  auto sound=at(CombatEventKind::ImpactSound,b.pos);sound.sound=13; // GlobalInfo sfxID index, not a host sound handle.
  sound.pan=(b.pos.x-f.playerPos.x)/100.f;h.event(sound);
 }
 if(!b.info->pierce)remove(b,h);
}
// 0xa444. Physics is per original 60 Hz tick, not an arbitrary variable delta.
void tickBullet(BulletState& b,const CombatFrame& f,CombatHost& h){
 if(!b.show)return;
 if(!b.info)throw std::logic_error("Bullet has no WeaponInfo");
 const auto& i=*b.info;
 if(!f.paused){
  if(i.g!=0.f)b.vel.y=b.vel.y-i.g;
  b.velTick={b.vel.x*dt,b.vel.y*dt};
  if(!i.pierce&&h.sweepTerrain(b.pos,b.velTick))collideBullet(b,false,f,h);
  else {b.pos.x=b.pos.x+b.velTick.x;b.pos.y=b.pos.y+b.velTick.y;}
  b.rotate=static_cast<float>(std::atan2(static_cast<double>(b.velTick.y),static_cast<double>(b.velTick.x)));
  if(!h.inCamera(b)){
   if(i.wpnType==7&&b.vel.y>0.f&&b.pos.y>f.playerPos.y+400.f){b.vel.y=0;b.pos.x=(f.playerPos.x+(rnd(h)*200.f))-100.f;}
  } else if(auto target=h.targetAt(b.toPlayer,b.pos)){
   int32_t damage=i.dmg;
   if(!b.toPlayer){damage+=(damage/8)*f.weaponUpgrade;if(f.damageBoost)damage=static_cast<int32_t>(static_cast<float>(damage)*1.5f);}
   else if(f.shield)damage/=2;
   collideBullet(b,true,f,h);h.damage(*target,damage);
   if(h.acquireParticle()){auto e=at(CombatEventKind::DamageFlash,b.pos);e.blend=2;e.life=.3f;e.asset="Efx_Damage.png";e.animationFrames=6;h.event(e);}
  }
  if(f.time-static_cast<double>(b.startTime)>static_cast<double>(i.liveTm*.9f)){
   b.alpha=b.alpha-0x1.111112p-4f; // 0x3d888889
   if(b.alpha<0.f){b.alpha=0;remove(b,h);}
  }
 }
 h.event({CombatEventKind::BaseBulletTick});
}
void initializeWeapon(WeaponState& w,double time,CombatHost& h){w=WeaponState{};w.resetTime=static_cast<float>(time);w.delayTime=rnd(h)+.5f;}
void setWeaponAim(WeaponState& w,float angle){
 w.aimDirDest=angle;double delta=static_cast<double>(w.aimDirDest-w.aimDir);
 if(delta>std::numbers::pi)w.aimDir=static_cast<float>(static_cast<double>(w.aimDir)+2*std::numbers::pi);
 else if(delta<-std::numbers::pi)w.aimDir=static_cast<float>(static_cast<double>(w.aimDir)-2*std::numbers::pi);
}
void setWeaponFire(WeaponState& w,bool firing,Vector2 target){w.isFire=firing;w.fireTarget=target;}
void refreshWeaponAim(WeaponState& w,bool player,float parentRotation){
 if(!w.weaponInfo||w.aimDir==w.aimDirDest)return;
 if(!w.weaponInfo->rotWpn){w.aimDir=player?parentRotation:w.aimDirDest;if(player)w.aimDirDest=parentRotation;}
 else {w.aimDir=w.aimDir+((w.aimDirDest-w.aimDir)*.5f);if(std::fabs(w.aimDir-w.aimDirDest)<.01f)w.aimDir=w.aimDirDest;}
}
// 0x1335c: input sockets/rotPos come from the host's original parts hierarchy.
void refreshWeaponMuzzle(WeaponState& w,const CombatFrame& f){
 w.muzzlePos={f.socket1.x-f.socket0.x,f.socket1.y-f.socket0.y};
 if(!w.weaponInfo||!w.weaponInfo->rotWpn)return;
 float angle=static_cast<float>(std::atan2(static_cast<double>(w.muzzlePos.y),static_cast<double>(w.muzzlePos.x))+static_cast<double>(w.aimDir));
 if(f.machType!=3)angle=static_cast<float>(static_cast<double>(angle)-std::numbers::pi);
 float len=std::sqrt((w.muzzlePos.x*w.muzzlePos.x)+(w.muzzlePos.y*w.muzzlePos.y));
 w.muzzlePos={static_cast<float>(static_cast<double>(f.rotPos.x)+std::cos(static_cast<double>(angle))*len),static_cast<float>(static_cast<double>(f.rotPos.y)+std::sin(static_cast<double>(angle))*len)};
}
// 0x13214. UI charge preview happens here; persistent charge decrements at burst end.
bool fireWeapon(WeaponState& w,const CombatFrame& f,CombatHost& h){
 if(!w.weaponInfo||!w.isReload)return false;
 w.fireTime=static_cast<float>(f.time);w.isReload=false;refreshWeaponMuzzle(w,f);w.fireCount=w.weaponInfo->shotCnt;w.fireDelay=0;
 if(f.isPlayer&&f.selectedWeapon!=0){if(!f.selectedCharge)throw std::logic_error("Selected weapon requires charge state");CombatEvent e{CombatEventKind::ChargePreview};e.value=static_cast<float>(*f.selectedCharge-1);h.event(e);}
 return true;
}
// 0x12a40: the original range gate is signed dx+dy, not Euclidean distance.
void processWeaponAI(WeaponState& w,const CombatFrame& f,CombatHost& h){
 if(!w.weaponInfo)return;
 const auto& i=*w.weaponInfo;
 float dx=(f.ownerPos.x+w.muzzlePos.x)-f.playerPos.x;
 float dy=f.playerPos.y-(f.ownerPos.y+w.muzzlePos.y);float sum=dx+dy;
 if(sum<0.f||std::fabs(sum)>520.f)return;
 bool due=f.time>static_cast<double>(w.resetTime+w.delayTime);
 if(w.aiStep==0&&due){
  w.aiStep=1;float angle;
  if(i.wpnType==7)angle=0x1.921fb6p+0f;
  else if(i.g<1.f){
   float y=(f.playerPos.y+10.f)-(f.weaponPos.y+w.muzzlePos.y);
   float x=f.playerPos.x-(f.weaponPos.x+w.muzzlePos.x);
   double a=std::atan2(static_cast<double>(y),static_cast<double>(x));float noise=(rnd(h)*-.03f);angle=static_cast<float>(a+static_cast<double>(noise));
  }else{
   float r=rnd(h);float fraction=(400.f-sum)/400.f;
   double a=static_cast<double>(fraction)+0x1.2d97c7f3321d2p+1;
   float noise=(r*.4f)-.2f;angle=static_cast<float>(a+static_cast<double>(noise));
   if(static_cast<double>(angle)<0x1.e28c7460698c7p+0)angle=0x1.e28c74p+0f;
  }
  setWeaponAim(w,angle);w.resetTime=static_cast<float>(f.time);w.delayTime=(rnd(h)*.5f)+.3f;
 }else if(w.aiStep==1&&due){w.aiStep=2;w.resetTime=static_cast<float>(f.time);w.delayTime=(rnd(h)*.4f)+.05f;
 }else if(w.aiStep==2){
  if(due){w.aiStep=0;w.resetTime=static_cast<float>(f.time);w.delayTime=(rnd(h)*i.delay)+.5f;}
  float jitter=(rnd(h)*.01f)-.005f;setWeaponAim(w,w.aimDir-jitter);fireWeapon(w,f,h);
 }
}
// 0x11e64 + shared epilogue/reload fragments 0x128ec, 0x129c6, 0x129f8.
void tickWeapon(WeaponState& w,const CombatFrame& f,CombatHost& h){
 if(f.paused)return;
 if(w.isFire&&std::fabs(w.aimDir-w.aimDirDest)<.1f)fireWeapon(w,f,h);
 if(w.fireTime==0.f){aiTail(w,f,h);return;}
 if(!w.weaponInfo)throw std::logic_error("Active weapon has no WeaponInfo");
 const auto& i=*w.weaponInfo;
 if(w.fireCount<1){
  float gauge=static_cast<float>((f.time-static_cast<double>(w.fireTime))/static_cast<double>(i.delay));
  if(gauge>1.f){w.fireTime=0;w.isReload=true;gauge=0;if(f.isPlayer)h.event({CombatEventKind::ReloadComplete});}
  if(f.isPlayer){CombatEvent e{CombatEventKind::ReloadGauge};e.value=gauge;h.event(e);}
  aiTail(w,f,h);return;
 }
 if(!(f.time>static_cast<double>(w.fireTime+w.fireDelay))){aiTail(w,f,h);return;}
 Vector2 pos={f.weaponPos.x+w.muzzlePos.x,f.weaponPos.y+w.muzzlePos.y};float angle=w.aimDir;
 if(i.wpnType==5){
  if(i.spd==0.f){angle=0;pos.x=(w.fireTarget.x+(rnd(h)*100.f))-50.f;pos.y=(f.ownerPos.y+320.f)+(rnd(h)*10.f);}
  else {
   pos.x=f.ownerPos.x+(rnd(h)*-20.f);pos.y=(f.ownerPos.y+320.f)+(rnd(h)*10.f);
   double a=std::atan2(static_cast<double>(w.fireTarget.y-pos.y),static_cast<double>(w.fireTarget.x-pos.x));
   float noise=rnd(h)*.2f;angle=static_cast<float>((a+static_cast<double>(noise))-static_cast<double>(.1f));
  }
 }else w.fireOffset={5.f*w.weaponVec.x,5.f*w.weaponVec.y};
 h.spawnBullet(i,f.machType!=3,angle,pos,f.time);
 if(w.fireDelay>=.1f||w.fireCount==i.shotCnt){
  if(h.acquireParticle()){auto e=at(CombatEventKind::MuzzleFlash,pos);e.asset="Explode.png";e.life=.1f;e.tileNo=h.random31()%8+1;h.event(e);}
  if(f.isMainPlayer&&(i.wpnType==0||i.wpnType==1||i.wpnType==2)&&h.acquireParticle()){
   auto e=at(CombatEventKind::Shell,{f.weaponPos.x+f.rotPos.x,f.weaponPos.y+f.rotPos.y});e.asset="Bullet.png";e.tileNo=16;e.life=1.5f;e.gravity=300.f;
   e.velocity.x=(rnd(h)*-100.f)-(i.wpnType==0?10.f:30.f);e.velocity.y=(rnd(h)*50.f)+50.f;e.rotationSpeed=i.wpnType==0?40.f:20.f;e.scale=i.wpnType==0?.5f:1.f;h.event(e);
  }
  auto e=at(CombatEventKind::FireSound,pos);e.soundName=i.fireSFX;e.pan=(f.ownerPos.x-f.playerPos.x)/100.f;h.event(e);
 }
 w.fireTime=static_cast<float>(f.time);w.fireDelay=i.shotDly;--w.fireCount;
 if(w.fireCount<=0&&f.isPlayer&&f.selectedWeapon!=0){
  if(!f.selectedCharge)throw std::logic_error("Selected weapon requires charge state");
  --*f.selectedCharge;CombatEvent e{CombatEventKind::ChargeChanged};e.value=static_cast<float>(*f.selectedCharge);h.event(e);
  if(*f.selectedCharge<1)h.event({CombatEventKind::RemoveWeapon});
 }
 aiTail(w,f,h);
}
}
