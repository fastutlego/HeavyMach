#include "../Sources/Core/BulletVisual.hpp"
#include <cassert>
#include <cmath>
#include <vector>

namespace {
struct Host final:heavymach::BulletVisualHost {
    heavymach::ParticleState particle;
    bool available=true;int acquisitions=0,randomCalls=0,trails=0;
    heavymach::Vector2 trailPosition{};
    std::vector<int32_t> randomValues{0,1073741824,1073741824,2147483647};
    heavymach::ParticleState* acquireBulletParticle()override {
        ++acquisitions;if(!available)return nullptr;
        heavymach::resetParticle(particle,0);return &particle;
    }
    int32_t random31()override{return randomValues.at(size_t(randomCalls++));}
    void addBulletTrailPosition(heavymach::BulletState&,heavymach::Vector2 p)override{++trails;trailPosition=p;}
};
bool near(float a,float b){return std::fabs(a-b)<.000001f;}
}
void runBulletVisualTests() {
    using namespace heavymach;
    WeaponInfo info;info.bulletTile="none";info.effectTile="BulletEffect.png";info.effectType="Normal";
    BulletState bullet;bullet.info=&info;bullet.pos={10,20};bullet.rotate=.4f;bullet.alpha=.25f;
    Host host;
    auto sprites=drawBullet(bullet,1,5,host);
    assert(sprites.size()==1&&sprites[0].tile=="BulletEffect.png"&&sprites[0].blend==2&&sprites[0].alpha==.25f);
    assert(sprites[0].position.x==10&&sprites[0].rotation==.4f);
    assert(host.randomCalls==0&&host.acquisitions==0);
    info.bulletTile="EnemyMissile01.png";info.bulletTileNo=6;info.effectType="Flame";
    sprites=drawBullet(bullet,1,5,host);
    assert(sprites.size()==2&&sprites[0].cols==2&&sprites[0].rows==4&&sprites[0].frame==6);
    assert(sprites[0].alpha==1&&sprites[0].blend==0&&!bullet.flameShow);
    sprites=drawBullet(bullet,1,5,host);assert(sprites.size()==1&&bullet.flameShow);
    info.bulletTile="EnemyBullet01.png";sprites=drawBullet(bullet,1,5,host);assert(sprites[0].cols==4&&sprites[0].rows==4);
    info.bulletTile="Bullet.png";sprites=drawBullet(bullet,1,5,host);assert(sprites[0].cols==4&&sprites[0].rows==8);
    info.effectType="Smoke";info.effectTile="SmokeEffect.png";info.effectTileNo=7;bullet.tileNo=2;
    bullet.effectRegenTime=1;bullet.effectDelay=.04f;
    double boundary=double(bullet.effectRegenTime+bullet.effectDelay);
    (void)drawBullet(bullet,boundary,12,host);assert(host.acquisitions==0);
    host.available=false;(void)drawBullet(bullet,boundary+.001,12,host);
    assert(host.acquisitions==1&&host.randomCalls==0&&bullet.effectRegenTime==float(boundary+.001));
    host.available=true;(void)drawBullet(bullet,2,12,host);
    assert(host.acquisitions==2&&host.randomCalls==4&&host.particle.show);
    assert(host.particle.liveTime==.6f&&host.particle.startTime==12&&host.particle.sprite.frame==2);
    assert(near(host.particle.sprite.scale.x,.2f)&&near(host.particle.scaleVelocity,1.25f));
    assert(near(host.particle.sprite.rotation,float(3.141592653589793))&&near(host.particle.rotationVelocity,2.5f));
    Host enemyHost;bullet.toPlayer=true;(void)drawBullet(bullet,3,13,enemyHost);
    assert(enemyHost.particle.liveTime==.5f&&near(enemyHost.particle.sprite.scale.x,.1f));
    assert(near(enemyHost.particle.scaleVelocity,.75f)&&near(enemyHost.particle.rotationVelocity,1.5f));
    info.effectType="Trail";(void)drawBullet(bullet,3,13,host);
    info.effectType="Laser";(void)drawBullet(bullet,3,13,host);
    assert(host.trails==2&&host.trailPosition.x==10&&host.randomCalls==4);
    bullet.show=false;assert(drawBullet(bullet,4,14,host).empty());assert(host.trails==2);
    bullet.show=true;bullet.removed=true;assert(drawBullet(bullet,4,14,host).empty());
}
