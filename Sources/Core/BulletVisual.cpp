#include "BulletVisual.hpp"
#include <bit>
#include <stdexcept>

#pragma STDC FP_CONTRACT OFF
namespace heavymach {
namespace {
float randomFraction(BulletVisualHost& host) {
    int32_t value=host.random31();
    if(value<0)throw std::domain_error("Bullet visual random31 must be nonnegative");
    return static_cast<float>(value)*0x1p-31f;
}
bool hasTile(const std::string& tile){return !tile.empty()&&tile!="none";}
DrawSprite body(const BulletState& b) {
    DrawSprite d;d.tile=b.info->bulletTile;d.frame=b.info->bulletTileNo;
    d.position=b.pos;d.rotation=b.rotate;d.z=2;
    // GlobalInfo::readWeaponChunk 0x10e00. The shorter draw overload uses
    // normal blend and alpha1, independent of the fading effect alpha.
    if(d.tile.starts_with("EnemyBullet0")){d.cols=4;d.rows=4;}
    else if(d.tile.starts_with("EnemyMissile")){d.cols=2;d.rows=4;}
    else {d.cols=4;d.rows=8;}
    return d;
}
void effect(std::vector<DrawSprite>& result,const BulletState& b) {
    if(!hasTile(b.info->effectTile))return;
    DrawSprite d;d.tile=b.info->effectTile;d.frame=b.info->effectTileNo;
    d.position=b.pos;d.rotation=b.rotate;d.alpha=b.alpha;d.blend=2;d.z=2;
    result.push_back(std::move(d));
}
}
std::vector<DrawSprite> drawBullet(BulletState& b,double worldTime,double globalTime,
                                 BulletVisualHost& host) {
    std::vector<DrawSprite> result;
    if(!b.show||b.removed||!b.info)return result;
    if(hasTile(b.info->bulletTile))result.push_back(body(b));
    switch(b.info->effectTypeNumber()) {
    case 1: // 0xab98: normal tracer, including weapons without a bullet sprite.
        effect(result,b);
        break;
    case 2: // 0xac00: Flame alternates on every draw, not every physics tick.
        if(b.flameShow)effect(result,b);
        b.flameShow=!b.flameShow;
        break;
    case 3: { // 0xac82: strict world-time boundary; update even on pool failure.
        float next=b.effectRegenTime+b.effectDelay;
        if(!(worldTime>double(next)))break;
        b.effectRegenTime=static_cast<float>(worldTime);
        auto particle=host.acquireBulletParticle();
        if(!particle)break;
        particle->liveTime=b.toPlayer?.5f:.6f;
        particle->sprite.tile=b.info->effectTile;
        particle->sprite.frame=b.tileNo; // Qob's tileNo, not WeaponInfo effectTileNo.
        particle->sprite.cols=1;particle->sprite.rows=1;
        particle->sprite.position=b.pos;
        float scale=randomFraction(host)*(b.toPlayer?.2f:.4f)+(b.toPlayer?.1f:.2f);
        particle->sprite.scale={scale,scale};
        particle->scaleVelocity=randomFraction(host)*(b.toPlayer?.5f:1.5f)+.5f;
        float angle=randomFraction(host)*std::bit_cast<float>(uint32_t{0x40490fdb});
        particle->sprite.rotation=angle+angle;
        particle->rotationVelocity=randomFraction(host)*(b.toPlayer?3.f:5.f)-(b.toPlayer?1.5f:2.5f);
        startParticle(*particle,globalTime);
        break;
    }
    case 4:case 5: // 0xafc4: no pause gate; host retains the trail's lifetime.
        host.addBulletTrailPosition(b,b.pos);
        break;
    default:break;
    }
    return result;
}
}
