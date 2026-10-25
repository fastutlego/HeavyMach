#pragma once
#include "Visuals.hpp"

namespace heavymach {
struct BulletVisualHost {
    virtual ~BulletVisualHost()=default;
    // Return a pool entry already reset as GobWorld::getFreeParticle does.
    virtual ParticleState* acquireBulletParticle()=0;
    virtual int32_t random31()=0;
    // No-op if this bullet has no trail. Called from draw, including pause.
    virtual void addBulletTrailPosition(BulletState&,Vector2)=0;
};
// One original GobBullet draw call, for a visible/in-camera bullet. Side effects
// include alternating flame, smoke emission and trail append. Do not call twice
// for one frame (e.g. once to collect and again to render).
// Particle delay compares world time; particle start records global time.
std::vector<DrawSprite> drawBullet(BulletState&,double worldTime,double globalTime,
                                 BulletVisualHost&);
}
