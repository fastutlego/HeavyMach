#include "../Sources/Core/Combat.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
using namespace heavymach;
namespace {
struct Host:CombatHost {
 std::vector<int32_t> randoms;size_t index=0;bool terrain=false,camera=true,pool=false;
 std::optional<CombatTarget> target;int sweeps=0,lookups=0,spawned=0;bool spawnedToPlayer=false;
 Vector2 spawnPos{};float spawnAngle=0;std::vector<int32_t> damageValues;std::vector<CombatEvent> events;
 int32_t random31() override {return index<randoms.size()?randoms[index++]:0;}
 bool sweepTerrain(Vector2& p,const Vector2&) override {++sweeps;if(terrain)p={7,8};return terrain;}
 bool inCamera(const BulletState&) override{return camera;}
 std::optional<CombatTarget> targetAt(bool,Vector2) override{++lookups;return target;}
 void damage(CombatTarget,int32_t n) override{damageValues.push_back(n);}
 bool acquireParticle() override{return pool;}
 void event(const CombatEvent& e) override{events.push_back(e);}
 void spawnBullet(const WeaponInfo&,bool t,float a,Vector2 p,double) override{++spawned;spawnedToPlayer=t;spawnAngle=a;spawnPos=p;}
 int count(CombatEventKind k)const{int n=0;for(auto&e:events)if(e.kind==k)++n;return n;}
};
void near(float x,float y){assert(std::fabs(x-y)<.0001f);}
}
int main(){
 // 0x9dc4: two-sided speed jitter, but one-sided negative aim jitter; no merged RNG.
 WeaponInfo info;info.spd=120;info.spdRand=.5f;info.aimRand=.2f;info.liveTm=10;
 Host h;h.randoms={1073741824,0,1073741824};BulletState b;
 setBulletFire(b,info,.3f,{10,20},1.25,h);
 near(b.vel.x,150.f*std::cos(.2f));near(b.vel.y,150.f*std::sin(.2f));near(b.effectDelay,.04f);assert(h.index==3);
 // Pause still dispatches superclass tick but must not move, collide or consume RNG.
 CombatFrame f;f.time=2;f.paused=true;auto before=b.pos;tickBullet(b,f,h);near(b.pos.x,before.x);assert(h.sweeps==0);assert(h.count(CombatEventKind::BaseBulletTick)==1);
 // Fixed tick: gravity is subtracted before multiplying by 1/60.
 f.paused=false;info.g=3;b.vel={60,63};b.pos={0,0};tickBullet(b,f,h);near(b.vel.y,60);near(b.pos.x,1);near(b.pos.y,1);
 // Non-piercing terrain collision uses the corrected position and does not add displacement.
 Host wall;wall.terrain=true;b.removed=false;b.pos={0,0};tickBullet(b,f,wall);assert(b.removed);near(b.pos.x,7);near(b.pos.y,8);
 // Upgrade uses integer damage/8, then float boost truncation; piercing skips terrain and survives.
 Host target;target.target=42;info.pierce=true;info.dmg=10;b.removed=false;f.weaponUpgrade=3;f.damageBoost=true;tickBullet(b,f,target);
 assert(target.sweeps==0);assert(target.damageValues.back()==19);assert(!b.removed);
 b.toPlayer=true;info.dmg=11;f.shield=true;tickBullet(b,f,target);assert(target.damageValues.back()==5);
 // Impact sound belongs to successful explosion allocation, not every collision.
 Host empty;info.pierce=false;collideBullet(b,true,f,empty);assert(empty.count(CombatEventKind::ImpactSound)==0);
 Host particles;particles.pool=true;collideBullet(b,false,f,particles);assert(particles.count(CombatEventKind::ImpactSound)==1);near(particles.events[0].pos.y,b.pos.y+40);
 // Lifetime has a strict >90% threshold, then decrements alpha per tick.
 Host aging;info.g=0;info.liveTm=10;b.startTime=0;b.alpha=1;b.toPlayer=false;b.removed=false;f.time=9;tickBullet(b,f,aging);near(b.alpha,1);
 f.time=9.01;tickBullet(b,f,aging);near(b.alpha,1.f-0x1.111112p-4f);
 // Type7 out-of-camera projectile apex relocation is relative to the player.
 Host mortar;mortar.camera=false;mortar.randoms={1073741824};info.wpnType=7;info.liveTm=100;b.pos={0,500};b.vel={0,60};f.playerPos={123,0};tickBullet(b,f,mortar);near(b.vel.y,0);near(b.pos.x,123);assert(mortar.lookups==0);
 // Burst and reload boundaries are strict; ammo decrements once after the whole burst.
 Host gun;WeaponInfo burst;burst.shotCnt=2;burst.shotDly=.1f;burst.delay=1;burst.spd=120;
 WeaponState w;w.weaponInfo=&burst;w.aimDirDest=0;int32_t charge=2;
 CombatFrame wf;wf.time=1;wf.selectedWeapon=1;wf.selectedCharge=&charge;wf.socket1={10,0};
 assert(fireWeapon(w,wf,gun));assert(charge==2);assert(gun.count(CombatEventKind::ChargePreview)==1);
 tickWeapon(w,wf,gun);assert(gun.spawned==0);
 wf.time=1.01;tickWeapon(w,wf,gun);assert(gun.spawned==1);assert(w.fireCount==1);assert(charge==2);
 wf.time=1.1;tickWeapon(w,wf,gun);assert(gun.spawned==1);
 wf.time=1.12;tickWeapon(w,wf,gun);assert(gun.spawned==2);assert(charge==1);assert(!w.isReload);
 wf.time=static_cast<double>(w.fireTime)+1;tickWeapon(w,wf,gun);assert(!w.isReload);
 wf.time+=.001;tickWeapon(w,wf,gun);assert(w.isReload);assert(w.fireTime==0);assert(gun.count(CombatEventKind::ReloadComplete)==1);
 // Original time-zero fire sentinel is preserved, not silently fixed.
 WeaponState zero;zero.weaponInfo=&burst;wf.time=0;fireWeapon(zero,wf,gun);wf.time=.5;int sp=gun.spawned;tickWeapon(zero,wf,gun);assert(gun.spawned==sp);
 // Type5 zero-speed sky drop uses target X and owner Y, not the normal muzzle.
 Host sky;WeaponInfo drop;drop.wpnType=5;drop.shotCnt=1;WeaponState dw;dw.weaponInfo=&drop;dw.fireTarget={200,0};wf.selectedWeapon=0;wf.time=1;wf.ownerPos={50,20};fireWeapon(dw,wf,sky);wf.time=1.01;tickWeapon(dw,wf,sky);near(sky.spawnPos.x,150);near(sky.spawnPos.y,340);near(sky.spawnAngle,0);
 // AI signed dx+dy gate and three-state timer. Step2 still fires on its exit tick.
 Host ai;WeaponInfo wi;wi.shotCnt=1;wi.delay=1;WeaponState aw;aw.weaponInfo=&wi;aw.delayTime=0;
 CombatFrame af;af.machType=0;af.isPlayer=false;af.isMainPlayer=false;af.ownerPos={600,0};af.weaponPos=af.ownerPos;af.time=1;
 processWeaponAI(aw,af,ai);assert(aw.aiStep==0);
 af.ownerPos={100,0};af.weaponPos=af.ownerPos;processWeaponAI(aw,af,ai);assert(aw.aiStep==1);assert(aw.isReload);
 af.time=1.31;processWeaponAI(aw,af,ai);assert(aw.aiStep==2);
 af.time=2;processWeaponAI(aw,af,ai);assert(aw.aiStep==0);assert(!aw.isReload);assert(aw.fireCount==1);
 std::cout<<"Combat source boundary tests passed\n";
}
