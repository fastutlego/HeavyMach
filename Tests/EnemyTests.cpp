#include "../Sources/Core/Enemy.hpp"
#include "../Sources/Core/GameData.hpp"
#include <cassert>
#include <cmath>
#include <deque>
#include <vector>
using namespace heavymach;
namespace {
struct Host:EnemyHost {
    std::deque<int32_t> randoms;
    std::vector<EnemyEvent> events;
    std::vector<Vector2> boxes;
    bool particle=true,consumeItemRng=false;
    int draws=0;
    int32_t random31() override { assert(!randoms.empty());auto n=randoms.front();randoms.pop_front();++draws;return n; }
    bool acquireParticle() override { return particle; }
    void spawnItemBox(Vector2 p) override { boxes.push_back(p);if(consumeItemRng)(void)random31(); }
    void event(const EnemyEvent& e) override {events.push_back(e);}
    void reset(std::initializer_list<int32_t> r) {randoms=r;events.clear();boxes.clear();draws=0;}
};
bool near(float a,float b){return std::fabs(a-b)<0.0001f;}
EnemyFrame frame(){EnemyFrame f;f.motion.backLine=-1000;f.motion.frontLine=10000;f.motion.right=9000;f.motion.bottom=-1000;return f;}
}
void runEnemyTests(){
    Host h;GobHvM_Enemy e;assert(e.machType==2&&!e.isBoss&&e.giveExp==0&&e.hp==256);
    MachDefinition d;d.name="Truck001";d.hpMax=100;d.giveExp=30;d.machType=0;
    h.reset({1});e.makeMachFromDefinition(d,h);assert(e.hp==100&&e.hpMax==100&&e.giveItem==0&&e.gaugeSize==16&&e.gaugeY==40&&h.draws==1);
    h.reset({0});e.makeMachFromDefinition(d,h);assert(e.giveItem==1);
    d.name="Heli001";d.hpMax=501;d.machType=1;h.reset({});e.makeMachFromDefinition(d,h);assert(e.giveItem==2&&e.gaugeSize==50&&e.gaugeY==-10&&h.draws==0);
    auto f=frame();e.machState=3;e.machType=0;e.position={1000,0};e.speed=.2f;
    h.reset({1});e.tick(f,h);assert(e.speed==.2f&&h.draws==1);
    h.reset({0,0});e.tick(f,h);assert(e.speed==-1&&h.draws==2);
    e.position.x=20;h.reset({0,2});e.tick(f,h);assert(e.speed==.6f&&h.draws==2); // Mode draw still consumed.
    e.position.x=100;h.reset({0,1,0});e.tick(f,h);assert(e.speed==-.6f);
    h.reset({0,1,1});e.tick(f,h);assert(e.speed==.4f);
    h.reset({0,2});e.tick(f,h);assert(e.speed==0);
    e.position.x=8990;h.reset({});e.tick(f,h);assert(e.speed==-.6f&&h.draws==0);
    e.machType=1;e.position={1000,140};e.speed=0;e.velocity.y=0;h.reset({0,1,0});e.tick(f,h);assert(near(e.speed,0.5f/30)&&near(e.velocity.y,-20.0f/30));
    e.position={20,70};e.speed=0;e.velocity.y=0;h.reset({1,0});e.tick(f,h);assert(near(e.speed,0.5f/30)&&near(e.velocity.y,20.0f/30));
    e.position.y=100;h.reset({1,0});e.tick(f,h);assert(e.gravityDest==0);
    f.motion.paused=true;h.reset({});auto speed=e.speed;e.tick(f,h);assert(e.speed==speed&&h.draws==0);f.motion.paused=false;
    e.hp=100;e.hpMax=100;e.hasHPGauge=true;e.gaugeSize=16;e.position={100,0};h.reset({2});e.onDamage(25,f,h);
    assert(e.hp==75&&h.events[0].kind==EnemyEventKind::Sound&&h.events[0].amount==10&&h.events[0].pan==1);
    assert(h.events[1].kind==EnemyEventKind::GaugeUpdate&&h.events[1].scale==.75f&&h.events[1].position.x==-4);
    e.hp=10;e.giveItem=1;e.giveExp=100;e.isBoss=false;f.internalLevel=8;f.mission=0;h.reset({7,1});h.consumeItemRng=true;e.onDamage(20,f,h);
    assert(e.hp==0&&h.boxes.size()==1&&h.draws==2); // Item init draw before hit sound.
    assert(h.events[0].kind==EnemyEventKind::Explosion&&h.events[0].position.y==28);
    assert(h.events[1].kind==EnemyEventKind::AddExperience&&h.events[1].amount==50);
    assert(h.events[2].kind==EnemyEventKind::Remove&&h.events[3].amount==14&&h.events[4].amount==9);
    e.isBoss=true;e.giveExp=31;f.motion.gameMode=1;h.reset({1});h.consumeItemRng=false;h.particle=false;e.onDestroy(f,h);
    assert(e.giveItem==3&&h.boxes.size()==3&&h.events[0].kind==EnemyEventKind::KillBoss);
    assert(h.events[1].kind==EnemyEventKind::AddExperience&&h.events[1].amount==15&&h.events[2].kind==EnemyEventKind::Remove);
}
