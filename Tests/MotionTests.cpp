#include "../Sources/Core/Motion.hpp"
#include <array>
#include <cassert>
#include <cmath>
using namespace heavymach;
namespace {
bool has(const MotionEvents& e, MotionEventKind k) { for (auto v:e) if(v.kind==k) return true; return false; }
bool near(float a,float b) { return std::fabs(a-b)<0.0001f; }
}
void runMotionTests() {
    std::array<GroundSegment,2> ground{{{ColVector({10,0},{0,0}),-1,1},{ColVector({20,0},{10,0}),0,-1}}};
    std::array<int,2> ids{0,1};
    MotionWorld w; w.ground=ground; w.collisionCandidates=ids; w.backLine=-100; w.frontLine=1000; w.right=900; w.bottom=-100;
    GobHvMach m; assert(m.hp==256 && m.machType==3 && m.machState==0 && m.boosterVel==230);
    m.player=true; m.machState=1; m.position={5,0.5f}; m.setCurrentCol(0,w); m.setMoveVel(1); m.tick(w);
    assert(near(m.position.x,6.4f) && near(m.position.y,0.5f));
    m.setMoveVel(-0.6f); assert(m.dashVel==-2.5f && near(m.speed,-0.84f));
    m.dashVel=0; m.setMoveVel(1,true); assert(near(m.speed,1.82f));
    m.position={10,0.5f};m.speed=1;m.processLand(w); assert(m.currentCol==1 && m.position.x==10 && m.position.y==0);
    m.position={20,0.5f};m.speed=1;m.processLand(w);assert(m.currentCol==-1);m.processLand(w);assert(m.machState==2 && m.velocity.y==30);
    m.machState=1;m.setCurrentCol(1,w);m.player=false;m.position={20,0.5f};m.processLand(w); assert(m.currentCol==1 && m.position.x==20);
    m.player=true;m.position={5,2};m.velocity={0,-120};m.speed=0;m.machState=2;m.useBooster=2;
    m.processFly(w);assert(m.machState==1 && near(m.position.y,0.5f) && m.useBooster==0);
    assert(has(m.setBooster(0),MotionEventKind::BoosterSmoke));assert(m.machState==2 && m.velocity.y==230);
    assert(m.setBooster(0).empty());assert(has(m.setBooster(1),MotionEventKind::BoosterSmoke));assert(m.useBooster==1);
    assert(m.setBooster(1).empty());assert(has(m.setBooster(5,true),MotionEventKind::BoosterSmoke));assert(m.useBooster==2 && near(m.velocity.y,299));
    assert(m.setBooster(5).empty());m.hp=0;m.machState=1;assert(m.setBooster(5).empty());
    m.hp=100;m.machState=2;m.machType=1;m.position={5,-101};m.velocity={0,0};m.speed=0;
    auto e=m.processFly(w);assert(has(e,MotionEventKind::Damage));assert(m.velocity.y==0);assert(m.hp==100); // Damage is a host event.
    m.position={871,0};m.speed=1;m.player=true;m.machState=2;e=m.tick(w);assert(has(e,MotionEventKind::NextStage)&&m.machState==3);
    m.machState=2;m.speed=1;w.bossCount=1;e=m.tick(w);assert(!has(e,MotionEventKind::NextStage)&&m.speed==0);
    w.bossCount=0;w.gameMode=1;m.speed=1;e=m.tick(w);assert(!has(e,MotionEventKind::NextStage));
    w.paused=true;m.velocity={0,50};auto saved=m.position;e=m.tick(w);assert(m.position.x==saved.x && m.position.y==saved.y && has(e,MotionEventKind::TickChildren));
    m.visible=false;assert(m.tick(w).empty());m.visible=true;w.paused=false;
    m.machState=1;m.position={5,0};m.setCurrentCol(0,w);m.speed=0;m.useEngineBounce=true;w.time=0.18;m.processLand(w);assert(near(m.engineBounce,0.36f));
    m.speed=1;m.processLand(w);assert(near(m.engineBounce,1.35f));
    m.machState=0;m.speed=1;w.globalTime=0.18;w.random=[] {return int32_t{0};};m.hasWeapon=true;e=m.processModel(w);assert(near(m.engineBounce,0.9f)&&has(e,MotionEventKind::ModelAim));
    w.random={};e=m.processModel(w);assert(has(e,MotionEventKind::ModelRandomRequired));
    // Reverse traversal uses prev and snaps to the previous segment origin.
    m.machState=1;m.player=true;m.dashVel=0;m.setCurrentCol(1,w);m.position={10,0};m.speed=-1;m.processLand(w);
    assert(m.currentCol==0 && m.position.x==10 && m.position.y==0);
    std::array<GroundSegment,1> slope{{{ColVector({10,10},{0,0}),-1,-1}}};
    MotionWorld sw=w;sw.ground=slope;std::array<int,1> sid{0};sw.collisionCandidates=sid;
    m.position={5,5};m.setCurrentCol(0,sw);m.speed=1;m.machState=1;m.processLand(sw);
    assert(m.speed==1.5f && m.position.x<5 && m.machState==1); // Steep uphill slides back.
    m.position={5,5};m.setCurrentCol(0,sw);m.speed=-1;m.machState=1;m.processLand(sw);
    assert(m.machState==2 && m.velocity.y==30); // Steep downhill launches.
    m.machState=2;m.machType=3;m.position={5,2};m.velocity={0,-120};m.speed=0;sw.ground=ground;sw.collisionCandidates={};
    m.processFly(sw);assert(m.machState==2 && m.position.y<0); // Empty candidate list is not all terrain.

}
