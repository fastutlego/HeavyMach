#include "../Sources/Core/World.hpp"
#include <cassert>
#include <cmath>
using namespace heavymach;
struct Host:WorldHost {
 int objects=0,enemies=0;Vector2 player{};std::vector<WorldEvent> events;
 Vector2 tileSize(std::string_view name)override{return name=="BG.png"?Vector2{960,640}:Vector2{100,40};}
 void spawnMapObject(const MapObjectDefinition&,int layer)override{assert(layer==1||layer==2);++objects;}
 void spawnEnemy(const EnemyDefinition&)override{++enemies;}
 void spawnPlayer(Vector2 p)override{player=p;}
 void event(const WorldEvent& e)override{events.push_back(e);}
};
int main(){
 MapDefinition m;m.left=0;m.right=2000;m.top=640;m.bottom=0;m.background="BG.png";
 m.collisionPaths={{{0,100},{1000,100},{2000,100}}};
 m.objects={{"A",{570,100},0}};m.enemies={{"Boss01",{900,120},true}};
 WorldState w;Host h;w.openMap(m,0,h);assert(h.player.x==20&&h.player.y==110.5f);assert(w.bins.size()==21&&w.collisionCandidates(0).size()==1);
 assert(w.bossCount==0&&w.bossEncounter==0&&h.objects==0);
 w.processRegen(30,h);assert(h.objects==0);w.processRegen(31,h);assert(h.objects==1);
 w.processRegen(400,h);assert(h.enemies==0);w.processRegen(411,h);assert(h.enemies==1&&w.frontLine==1050&&w.bossEncounter==1);
 w.killBoss();assert(w.bossCount==-1&&w.bossEncounter==0&&w.frontLine==2010);
 w.processCamera(Vector2{20,110});assert(w.camera.x==240&&w.camera.y==160);
 assert(std::fabs(w.bgScale-(480.0f/1520))<.00001f);
 auto* p=w.freeParticle(10);assert(p==&w.particles[0]);startParticle(*p,10);assert(w.freeParticle(10)==&w.particles[1]);
 std::vector<TargetView> targets{{1,false,[](Vector2){return true;}},{2,true,[](Vector2){return true;}},{3,true,[](Vector2){return true;}}};
 assert(worldTargetAt(false,{},targets,nullptr)==2);assert(worldTargetAt(true,{},targets,&targets[0])==1);
 assert(WorldState::moveInput(.03f)==0&&WorldState::moveInput(.2f)==1);
 StagePosition s{0,0};w.nextStage(s,h);assert(s.stage==1&&w.paused&&w.gameState==1);
 for(int i=0;i<13;++i)w.processStageClear(s,h);assert(w.presentation.step==2&&w.presentation.scale==1);
 w.resultButton(1001,h);assert(w.presentation.step==3);
 w.processStageClear(s,h);assert(w.presentation.step==4);w.processStageClear(s,h);assert(w.presentation.step==5);
 w.processStageClear(s,h);assert(w.presentation.step==6);w.processStageClear(s,h);assert(!w.paused&&w.gameState==0);
 double before=w.gameTime;w.tick({},s,h);assert(w.gameTime==before+double(1.0f/60.0f));
 w.configureSurvival(0);assert(w.survivalRegenSeed==400);w.configureSurvival(50);assert(w.survivalRegenSeed==100);
 w.openMap(m,0,h);assert(w.objectRegenPosition==20);assert(w.motionWorld(20,0).ground.size()==2);
}
