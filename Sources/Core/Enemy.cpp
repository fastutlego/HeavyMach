#include "Enemy.hpp"
#include "GameData.hpp"
#include <bit>
#include <cmath>
#include <limits>
#include <stdexcept>
namespace heavymach {
namespace {
int32_t random31(EnemyHost& h) {
    const auto r=h.random31();
    if (r<0) throw std::domain_error("Enemy random() requires nonnegative 31-bit values");
    return r;
}
int32_t subtractWrap(int32_t a,int32_t b) {
    return std::bit_cast<int32_t>(static_cast<uint32_t>(a)-static_cast<uint32_t>(b));
}
void sound(EnemyHost& h,int32_t index,float pan) {
    EnemyEvent e{EnemyEventKind::Sound};e.amount=index;e.pan=pan;h.event(e);
}
}
GobHvM_Enemy::GobHvM_Enemy() { machType=2; }
bool GobHvM_Enemy::makeMachFromDefinition(const MachDefinition& d,EnemyHost& h) {
    hpMax=d.hpMax;hp=d.hpMax;giveExp=d.giveExp;machType=d.machType;
    h.event({EnemyEventKind::PartsDefinition}); // Caller applies d.parts in original recursive order.
    if (d.name=="Heli001") giveItem=2;
    else if (random31(h)%20==0) giveItem=1;
    // A non-drop draw leaves the previous giveItem unchanged, as in 0x14918.
    const bool large=hpMax>=501;
    gaugeSize=large?50.0f:16.0f;
    gaugeY=machType==1?-10.0f:(large?80.0f:40.0f);
    hasHPGauge=true;
    EnemyEvent e{EnemyEventKind::GaugeCreate};e.asset=large?"HPGaugeBoss.png":"HPGaugeEnemy.png";
    e.position={0,gaugeY};e.value=gaugeSize;e.tileNo=1;h.event(e);
    return true; // The typed definition is present; host lookup must reject missing names.
}
MotionEvents GobHvM_Enemy::tick(const EnemyFrame& f,EnemyHost& h) {
    if (!f.motion.paused) {
        if (machType==0) {
            if (position.x>f.motion.right-30.0f) speed=-0.6f;
            else if (random31(h)%50==0) {
                const int mode=random31(h)%3;
                if (position.x<f.playerPos.x+50.0f) speed=0.6f;
                else if (mode==0) {
                    if (f.playerPos.x>position.x) speed=0.8f;
                    else if (f.playerPos.x<position.x-400.0f) speed=-1.0f;
                    else speed=0;
                } else if (mode==1) speed=(random31(h)&1)==0?-0.6f:0.4f;
                else speed=0;
            }
        } else if (machType==1) {
            if (position.x>f.motion.right-30.0f) speedDest=-0.5f;
            else if (random31(h)%60==0) speedDest=(random31(h)&1)==0?-0.5f:0.5f;
            if (random31(h)%30==0) {
                if (position.x<f.playerPos.x+120.0f) speedDest=0.5f;
                if (position.y>f.playerPos.y+130.0f) gravityDest=-20;
                else if (position.y<f.playerPos.y+80.0f) gravityDest=20;
                else gravityDest=0;
            }
            speed=speed+(speedDest-speed)/30.0f;
            velocity.y=velocity.y+(gravityDest-velocity.y)/30.0f;
        }
    }
    MotionWorld world=f.motion;
    world.random=[&h] { return random31(h); };
    return GobHvMach::tick(world);
}
void GobHvM_Enemy::setHPGauge(float fraction,EnemyHost& h) const {
    if (!hasHPGauge) return;
    EnemyEvent e{EnemyEventKind::GaugeUpdate};e.scale=fraction;e.position={-(gaugeSize-fraction*gaugeSize),1};h.event(e);
}
void GobHvM_Enemy::onDamage(int32_t amount,const EnemyFrame& f,EnemyHost& h) {
    hp=subtractWrap(hp,amount);
    if (hp<1) { hp=0;onDestroy(f,h); }
    sound(h,random31(h)%3+8,(position.x-f.playerPos.x)/100.0f);
    setHPGauge(static_cast<float>(hp)/static_cast<float>(hpMax),h);
}
void GobHvM_Enemy::onDestroy(const EnemyFrame& f,EnemyHost& h) {
    if (isBoss) {giveItem=random31(h)%2+2;h.event({EnemyEventKind::KillBoss});}
    for (int32_t i=0;i<giveItem;++i) h.spawnItemBox(position);
    if (h.acquireParticle()) {
        EnemyEvent e{EnemyEventKind::Explosion};e.position={position.x,position.y+28};
        e.asset="Efx_ExplodeNPC.png";e.life=1;e.tileNo=0;e.animationFrames=16;e.scale=1;e.blend=0;h.event(e);
    }
    int32_t experience=giveExp;
    if (f.motion.gameMode==0) {
        const int32_t missionLevel=std::bit_cast<int32_t>(static_cast<uint32_t>(f.mission)*4u+4u);
        if (missionLevel<f.internalLevel) {
            const float ratio=static_cast<float>(missionLevel)/static_cast<float>(f.internalLevel);
            const float value=static_cast<float>(experience)*ratio;
            if (!std::isfinite(value) || static_cast<double>(value)>std::numeric_limits<int32_t>::max() || static_cast<double>(value)<std::numeric_limits<int32_t>::min())
                throw std::domain_error("enemy EXP float conversion outside restored int32 domain");
            experience=static_cast<int32_t>(value);
        }
    } else experience=experience/2;
    EnemyEvent e{EnemyEventKind::AddExperience};e.amount=experience;h.event(e);
    h.event({EnemyEventKind::Remove});
    sound(h,14,(position.x-f.playerPos.x)/100.0f);
}
}
