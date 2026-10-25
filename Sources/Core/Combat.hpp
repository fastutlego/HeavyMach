#pragma once
#include "Geometry.hpp"
#include "GameData.hpp"
#include <string_view>
#include <cstdint>
#include <optional>

namespace heavymach {
struct BulletState {
    bool toPlayer=false, show=true, removed=false, flameShow=true;
    const WeaponInfo* info=nullptr;
    int16_t tileNo=0; // QobImage inherited frame, used by smoke drawing.
    Vector2 pos{}, vel{}, velTick{};
    float rotate=0, alpha=1, startTime=0, effectRegenTime=0, effectDelay=.06f;
};
struct WeaponState {
    const WeaponInfo* weaponInfo=nullptr;
    Vector2 muzzlePos{},fireOffset{},weaponVec{},fireTarget{};
    float aimDir=0,aimDirDest=.1f,fireTime=0;
    int32_t fireCount=0; float fireDelay=0;
    bool isFire=false,isReload=true;
    int32_t aiStep=0; float resetTime=0,delayTime=0;
};
struct CombatFrame {
    double time=0; bool paused=false;
    Vector2 playerPos{},ownerPos{},weaponPos{},rotPos{},socket0{},socket1{};
    int32_t machType=3; bool isPlayer=true,isMainPlayer=true,ownerInScreen=true;
    uint8_t weaponUpgrade=0; bool damageBoost=false,shield=false;
    int16_t selectedWeapon=0; int32_t* selectedCharge=nullptr;
};
enum class CombatEventKind {
    TrailCreate, TerrainExplosion, TargetExplosion, DamageFlash, MuzzleFlash, Shell,
    FireSound, ImpactSound, RemoveBullet, ChargePreview, ChargeChanged, RemoveWeapon,
    ReloadComplete, ReloadGauge, BaseBulletTick
};
struct CombatEvent {
    CombatEventKind kind;
    Vector2 pos{},velocity{};
    std::string_view tile{},soundName{};
    const char* asset=nullptr;
    int32_t tileNo=0,animationFrames=0,blend=0,blocks=0;
    uint32_t sound=0;
    float life=0,gravity=0,rotationSpeed=0,scale=1,pan=0,value=0;
    float width=0,length=0,fadeDelay=0,fadeTime=0;
    bool wind=false,diffuse=false;
};
using CombatTarget=uintptr_t;
// Host supplies original world/collision/pool/render behavior; no synthetic hits.
struct CombatHost {
    virtual ~CombatHost()=default;
    virtual int32_t random31()=0; // Same 0..0x7fffffff stream and order as original random().
    virtual bool sweepTerrain(Vector2& pos,const Vector2& displacement)=0;
    virtual bool inCamera(const BulletState&)=0;
    virtual std::optional<CombatTarget> targetAt(bool toPlayer,Vector2 pos)=0;
    virtual void damage(CombatTarget,int32_t)=0;
    virtual bool acquireParticle()=0;
    virtual void event(const CombatEvent&)=0;
    virtual void spawnBullet(const WeaponInfo&,bool toPlayer,float angle,Vector2 pos,double time)=0;
};
void setBulletFire(BulletState&,const WeaponInfo&,float angle,Vector2 pos,double time,CombatHost&);
void collideBullet(BulletState&,bool targetHit,const CombatFrame&,CombatHost&);
void tickBullet(BulletState&,const CombatFrame&,CombatHost&);
void initializeWeapon(WeaponState&,double time,CombatHost&);
void setWeaponAim(WeaponState&,float angle);
void setWeaponFire(WeaponState&,bool firing,Vector2 target);
void refreshWeaponMuzzle(WeaponState&,const CombatFrame&);
bool fireWeapon(WeaponState&,const CombatFrame&,CombatHost&);
void processWeaponAI(WeaponState&,const CombatFrame&,CombatHost&);
void tickWeapon(WeaponState&,const CombatFrame&,CombatHost&);
// Aim-only part of refreshRotationPos (0x13530); host owns attachment transforms.
void refreshWeaponAim(WeaponState&,bool isPlayer,float parentRotation);
}
