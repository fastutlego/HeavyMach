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
 // Expanded visibility does not move physical boundaries or the boss trigger.
 for(float width:{480.f,568.8889f,695.f,696.f}){
  Host wideHost;WorldState wide;wide.setCanvasWidth(width);wide.openMap(m,0,wideHost);
  assert(wide.backLine==0&&wide.frontLine==2010&&wide.motionWorld(20,0).backLine==0);
  wide.processCamera(Vector2{20,110});assert(wide.camera.x==width*.5f);
  wide.processRegen(400,wideHost);assert(wide.bossEncounter==0&&wideHost.enemies==(width==480?0:1));
  wide.processRegen(411,wideHost);assert(wide.bossEncounter==1&&wide.frontLine==1050&&wideHost.enemies==1);
  wide.backLine=800;wide.frontLine=1100;wide.processCamera(Vector2{900,100});
  if(width>480)assert(wide.camera.x==950); // Narrow lock is centered; physics keeps its 300-unit gate.
  assert(wide.motionWorld(900,0).backLine==800&&wide.motionWorld(900,0).frontLine==1100);
 }
 // Tiny original backgrounds receive uniform cover scaling only in wide mode.
 WorldState cover;cover.map=m;cover.hasBackground=true;cover.backgroundSize={554,340};
 cover.backLine=0;cover.frontLine=2010;cover.setCanvasWidth(696);
 assert(cover.backgroundDrawScale==696.f/554.f);
 for(auto position:{Vector2{0,0},Vector2{1000,320},Vector2{2200,900}}){
  cover.processCamera(position);
  auto relative=Vector2{cover.backgroundPosition.x-cover.camera.x,cover.backgroundPosition.y-cover.camera.y};
  assert(relative.x-cover.backgroundSize.x*cover.backgroundDrawScale*.5f<=-348+.001f);
  assert(relative.x+cover.backgroundSize.x*cover.backgroundDrawScale*.5f>=348-.001f);
  assert(relative.y-cover.backgroundSize.y*cover.backgroundDrawScale*.5f<=-160+.001f);
  assert(relative.y+cover.backgroundSize.y*cover.backgroundDrawScale*.5f>=160-.001f);
 }
 cover.setCanvasWidth(480);assert(cover.backgroundDrawScale==1);
 assert(cover.bgScale==std::min((554.f-480)/(2000-480),(340.f-320)/(640-320)));
 // Objects and ordinary enemies prepare inside the new right edge at map start.
 auto extended=m;extended.objects={{"A",{680,100},0}};extended.enemies={{"Tank001",{680,100},false},{"Boss01",{900,100},true}};
 WorldState visible;Host visibleHost;visible.setCanvasWidth(696);visible.openMap(extended,0,visibleHost);
 visible.tick(visibleHost.player,{0,0},visibleHost);
 assert(visibleHost.objects==1&&visibleHost.enemies==1&&visible.bossEncounter==0&&visible.frontLine==2010);

 // Map transitions retain the old camera, but may not preactivate the new map.
 auto nextMap=m;nextMap.objects={{"A",{680,100},0},{"A",{1500,100},0}};
 nextMap.enemies={{"Tank001",{680,100},false},{"Tank001",{1500,100},false},{"Boss01",{900,100},true}};
 WorldState transition;Host transitionHost;transition.setCanvasWidth(696);transition.camera={10000,160};
 transition.openMap(nextMap,0,transitionHost);
 assert(transition.camera.x==348&&transitionHost.objects==1&&transitionHost.enemies==1);
 assert(!transition.objectsSpawned[1]&&!transition.enemiesSpawned[1]&&transition.bossEncounter==0);
 for(int n=0;n<10;++n)transition.tick(transitionHost.player,{0,0},transitionHost);
 assert(transitionHost.objects==1&&transitionHost.enemies==1&&transition.frontLine==2010);
 // Once the player actually advances, the same distant definitions become eligible.
 transition.processRegen(1100,transitionHost);
 assert(transitionHost.objects==2&&transitionHost.enemies==3&&transition.bossEncounter==1);

}
