#include "../Sources/Core/Visuals.hpp"
#include <cassert>
#include <cmath>
#include <numbers>
using namespace heavymach;
bool near(float a,float b){return std::fabs(a-b)<.0001f;}
int main(){
 ParticleState p;p.sprite.alpha=.2f;resetParticle(p,0);assert(p.sprite.alpha==.2f&&p.sprite.blend==2);
 p.velocity={60,0};p.gravity=60;p.rotationVelocity=60;p.scaleVelocity=6;p.liveTime=2;p.tileAnimation=true;p.animationFrames=4;
 startParticle(p,0);int callbacks=0;tickParticle(p,1,[&]{++callbacks;});
 assert(near(p.sprite.position.x,1)&&near(p.velocity.y,-1)&&near(p.sprite.rotation,1));
 assert(near(p.sprite.scale.x,1.1f)&&p.sprite.frame==2&&p.sprite.alpha==.5f);
 p.selfRemove=true;tickParticle(p,2,[&]{++callbacks;});assert(!p.show&&p.removed&&callbacks==2);
 tickParticle(p,3,[&]{++callbacks;});assert(callbacks==2);
 ParticleState ground;resetParticle(ground,0);startParticle(ground,0);ground.groundPosition=0;ground.velocity={10,-10};ground.rotationVelocity=2;
 tickParticle(ground,0);assert(ground.sprite.position.y==0&&near(ground.velocity.x,7)&&near(ground.velocity.y,3)&&ground.rotationVelocity==1);
 ColVector c;c.normal={0,1};ground.velocity={10,-10};ground.rotationVelocity=-2;bounceParticle(ground,c);
 assert(near(ground.velocity.x,2.1f)&&near(ground.velocity.y,3)&&ground.rotationVelocity==0);
 PartsInfo base;base.name="Base";base.tileCnt=2;base.sockets={{2,3},{12,13}};
 PartsInfo child;child.name="Child";child.tileCnt=1;child.sockets={{4,5},{14,15}};
 PartsState b;b.info=&base;attachParts(b,nullptr,0);PartsState q;q.info=&child;attachParts(q,&b,1);
 assert(q.offset.x==6&&q.offset.y==5&&q.basePosition.x==10&&q.basePosition.y==10);
 auto d=drawParts(q,{100,200},float(std::numbers::pi/2),2,1,0,0);
 assert(near(d.position.x,95)&&near(d.position.y,208)&&d.cols==1);
 drawParts(b,{3,0},0,0,1,1,0);assert(b.tileFrame==0);
 d=drawParts(b,{3.01f,0},0,0,1,1,0);assert(d.frame==1);
 child.partsType=3;child.tileCnt=4;d=drawParts(q,{0,0},0,4,0,0,.015);assert(d.frame==3&&near(d.position.y,7));
 WeaponInfo wi;wi.rotWpn=1;WeaponState w;w.weaponInfo=&wi;w.aimDir=0;w.aimDirDest=1;w.fireOffset={10,5};
 d=drawWeapon(q,w,{0,0},true,3,.1f,2);assert(w.aimDir==.5f&&near(w.fireOffset.x,8)&&near(w.fireOffset.y,4)&&d.frame==0);
}
