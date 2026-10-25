#include "World.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>
namespace heavymach {
std::optional<uintptr_t> worldTargetAt(bool toPlayer,Vector2 p,std::span<const TargetView> enemies,const TargetView* player){
 if(toPlayer){if(player&&player->contains&&player->contains(p))return player->id;return {};}
 for(const auto& e:enemies)if(e.id&&e.inScreen&&e.contains&&e.contains(p))return e.id;return {};
}
void WorldState::openMap(const MapDefinition& definition,int mode,WorldHost& host){
 map=definition;gameMode=mode;backLine=map.left;frontLine=map.right+10;
 center={map.left+(map.right-map.left)*.5f,map.bottom+(map.top-map.bottom)*.5f};
 float count=(map.right-map.left)/100.0f+1;
 if(!std::isfinite(count)||count<1||count>=32768)throw std::invalid_argument("Original collision bin count outside int16 range");
 bins.assign(size_t(int(count)),{});ground.clear();
 auto cols=map.collisionVectors();ground.resize(cols.size());
 for(size_t i=0;i<cols.size();++i){
  auto& g=ground[i];g.collision=cols[i];g.prev=cols[i].prev?int(cols[i].prev-cols.data()):-1;g.next=cols[i].next?int(cols[i].next-cols.data()):-1;
  g.collision.prev=nullptr;g.collision.next=nullptr;
 }
 size_t ci=0;
 for(const auto& path:map.collisionPaths)for(size_t j=1;j<path.size();++j,++ci){
  float current=path[j].x,previous=path[j-1].x;
  for(size_t b=0;b<bins.size();++b){float left=100.0f*float(b)+map.left-50.0f;float right=100.0f+left+50.0f;
   if(left<=current&&right>=previous)bins[b].push_back(int(ci));}
 }
 // Keep ColVector links valid for callers of Geometry, alongside Motion's indices.
 for(auto& g:ground){g.collision.prev=g.prev>=0?&ground[size_t(g.prev)].collision:nullptr;g.collision.next=g.next>=0?&ground[size_t(g.next)].collision:nullptr;}
 bossCount=0;bossEncounter=0;objectRegenPosition=-10000;preparedVisibleRight=-10000;objectsSpawned.assign(map.objects.size(),false);enemiesSpawned.assign(map.enemies.size(),false);bossEncounterActivated.assign(map.enemies.size(),false);mapEnemiesDefeated.assign(map.enemies.size(),false);
 objectHalfWidths.clear();for(const auto& o:map.objects)objectHalfWidths.push_back(host.mapTileSize(o.name+".png").x*.5f);
 hasBackground=!map.background.empty();if(hasBackground){backgroundSize=host.mapTileSize(map.background);updateBackgroundGeometry();}
 for(auto& p:particles){p=ParticleState{};resetParticle(p,0);p.sprite.z=3;p.show=false;}
 playerSpawn.x=mode==1?map.right-480:map.left+20;float h=mapHeight(playerSpawn.x);playerSpawn.y=h==10000?180:h+10;
 host.spawnPlayer(playerSpawn);host.event({WorldEventKind::ExperienceZero});host.event({WorldEventKind::SelectDefaultWeapon});host.event({WorldEventKind::GameStart});
 if(canvasWidth>480){
  // The wider view may expose an old map's retained camera before its first tick.
  // Start within this map's valid follow range; original 480 retains its camera.
  const float half=canvasWidth*.5f;
  camera.x=frontLine-backLine<canvasWidth?(backLine+frontLine)*.5f:
   std::clamp(camera.x,backLine+half,std::clamp(playerSpawn.x+140,backLine+half,frontLine-half));
 }
 processRegen(playerSpawn.x,host);
}
std::span<const int> WorldState::collisionCandidates(float x)const{
 if(bins.empty())return {};float f=(x-map.left)/100.0f;
 if(!std::isfinite(f))throw std::invalid_argument("Nonfinite collision lookup");
 int index=f<0?0:f>=float(bins.size())?int(bins.size()-1):int(f);return bins[size_t(index)];
}
bool WorldState::sweepTerrain(Vector2& p,const Vector2& d)const{for(int i:collisionCandidates(p.x))if(ground[size_t(i)].collision.checkCollideVel(p,d))return true;return false;}
float WorldState::mapHeight(float x)const{Vector2 p{x,4000};return sweepTerrain(p,{0,-10000})?p.y:10000;}
MotionWorld WorldState::motionWorld(float x,double global,std::function<int32_t()> random)const{
 MotionWorld m;m.ground=ground;m.collisionCandidates=collisionCandidates(x);m.backLine=backLine;m.frontLine=frontLine;m.right=map.right;m.bottom=map.bottom;m.bossCount=bossCount;m.gameMode=gameMode;m.time=gameTime;m.globalTime=global;m.paused=paused;m.random=std::move(random);return m;
}
void WorldState::setCanvasWidth(float width){canvasWidth=CanvasGeometry::checkedWidth(width);if(hasBackground)updateBackgroundGeometry();}
void WorldState::updateBackgroundGeometry(){
 backgroundDrawScale=1;
 if(canvasWidth>480&&backgroundSize.x>0&&backgroundSize.y>0)
  backgroundDrawScale=std::max({1.f,canvasWidth/backgroundSize.x,320.f/backgroundSize.y});
 auto size=Vector2{backgroundSize.x*backgroundDrawScale,backgroundSize.y*backgroundDrawScale};
 if(canvasWidth==480){float x=(size.x-480)/(map.right-map.left-480);float y=(size.y-320)/(map.top-map.bottom-320);bgScale=x<y?x:y;}
 else {float dx=map.right-map.left-canvasWidth,dy=map.top-map.bottom-320;
  float x=dx>0?(size.x-canvasWidth)/dx:0,y=dy>0?(size.y-320)/dy:0;bgScale=std::max(0.f,std::min(x,y));}
}
void WorldState::processCamera(std::optional<Vector2> p){
 if(p){camera.x+=(p->x+140-camera.x)/3.0f;camera.y+=(p->y+70-camera.y)/3.0f;
  if(canvasWidth==480){if(camera.x<backLine+240)camera.x=backLine+240;if(camera.x>frontLine-240)camera.x=frontLine-240;}
  else if(frontLine-backLine<canvasWidth)camera.x=(backLine+frontLine)*.5f;
  else camera.x=std::clamp(camera.x,backLine+canvasWidth*.5f,frontLine-canvasWidth*.5f);
  if(camera.y<map.bottom+160)camera.y=map.bottom+160;if(camera.y>map.top-160)camera.y=map.top-160;}
 if(hasBackground){backgroundPosition={camera.x+(center.x-camera.x)*bgScale,camera.y+(center.y-camera.y)*bgScale};
  if(canvasWidth>480){float dx=std::max(0.f,(backgroundSize.x*backgroundDrawScale-canvasWidth)*.5f),dy=std::max(0.f,(backgroundSize.y*backgroundDrawScale-320)*.5f);
   backgroundPosition.x=std::clamp(backgroundPosition.x,camera.x-dx,camera.x+dx);
   backgroundPosition.y=std::clamp(backgroundPosition.y,camera.y-dy,camera.y+dy);}}

}
void WorldState::processRegen(float x,WorldHost& host){
 bool originalAdvance=x>objectRegenPosition+10;
 if(canvasWidth==480&&!originalAdvance)return;
 if(originalAdvance)objectRegenPosition=x;
 // Static terrain and ordinary enemies can be prepared in the added view.
 // The boss trigger retains the original 10-unit sampling and player+500 rule.
 float extent=500;
 if(canvasWidth>480){
  const float half=canvasWidth*.5f;
  // A new map retains the previous map's camera. Do not use that transient
  // position to activate the new map's distant enemies or prepare all terrain.
  const float maximumCamera=frontLine-backLine<canvasWidth?(backLine+frontLine)*.5f:
   std::clamp(x+140,backLine+half,frontLine-half);
  const float preparationCamera=std::min(camera.x,maximumCamera);
  extent=std::max(500.f,preparationCamera+half+60-x);
 }
 bool prepare=canvasWidth==480||x+extent>preparedVisibleRight;
 if(prepare)preparedVisibleRight=x+extent;
 if(prepare)for(size_t i=0;i<map.objects.size();++i)if(!objectsSpawned[i]&&map.objects[i].position.x-objectHalfWidths[i]-x<extent){objectsSpawned[i]=true;host.spawnMapObject(map.objects[i],map.objects[i].layer==0?1:2);}
 for(size_t i=0;i<map.enemies.size();++i){
  const auto& enemy=map.enemies[i];
  if(canvasWidth==480){
   if(!enemiesSpawned[i]&&enemy.position.x-x<500){enemiesSpawned[i]=true;spawnEnemy(enemy,host,i);}
  }else if(prepare&&!enemiesSpawned[i]){
   // 128 is conservative against every shipped enemy's registered bounds
   // (largest left extent 78.25); no duplicate part construction/RNG is needed.
   const float margin=128.f;
   if(enemy.position.x-x-margin<extent){enemiesSpawned[i]=true;host.spawnMapEnemy(enemy,i);}
  }
  // Prefetching prepares display/hit detection. Boss motion and weapon AI
  // wait for this original placement-based 500/10 encounter trigger.
  if(enemy.isBoss&&enemiesSpawned[i]&&!bossEncounterActivated[i]&&!mapEnemiesDefeated[i]&&originalAdvance&&enemy.position.x-x<500){
   if(host.mapEnemyAlive(i)){bossEncounterActivated[i]=true;activateBossEncounter(enemy);}
   else mapEnemiesDefeated[i]=true;
  }
 }
}
void WorldState::activateBossEncounter(const EnemyDefinition& enemy){
 ++bossEncounter;if(frontLine>map.right)frontLine=std::min(enemy.position.x+150,map.right-10);
 else frontLine=std::min(std::max(frontLine,enemy.position.x+150),map.right-10);
}
void WorldState::spawnEnemy(const EnemyDefinition& enemy,WorldHost& host,std::optional<size_t> mapIndex){
 if(enemy.isBoss){activateBossEncounter(enemy);if(mapIndex)bossEncounterActivated[*mapIndex]=true;}
 if(mapIndex&&canvasWidth>480)host.spawnMapEnemy(enemy,*mapIndex);else host.spawnEnemy(enemy);
}
void WorldState::killMapBoss(size_t index){
 if(index>=mapEnemiesDefeated.size())return;
 if(mapEnemiesDefeated[index])return;
 mapEnemiesDefeated[index]=true;
 // A boss killed before its encounter cannot unlock some other living boss.
 if(bossEncounterActivated[index])killBoss();
}
void WorldState::killBoss(){--bossEncounter;--bossCount;if(bossEncounter<1){bossEncounter=0;frontLine=map.right+10;}}
void WorldState::removeTrash(float boundary,std::span<const TargetView> objects,const std::function<bool(uintptr_t,float)>& behind,const std::function<void(uintptr_t)>& remove){
 for(const auto& o:objects)if(o.id&&behind(o.id,boundary))remove(o.id);if(backLine<boundary)backLine=boundary;
}
ParticleState* WorldState::freeParticle(double time){for(auto& p:particles)if(!p.show){resetParticle(p,time);return &p;}return nullptr;}
void WorldState::clearObjects(WorldHost& host){host.event({WorldEventKind::ClearObjects});hasBackground=false;for(auto& p:particles)p.show=false;}
void WorldState::nextStage(StagePosition& stage,WorldHost& host){
 presentation=StagePresentation{};presentation.step=0;paused=true;host.event({WorldEventKind::RemoveLoadingScreen});host.event({WorldEventKind::TitleIn});
 ++stage.stage;if(stage.stage>=GlobalInfo::stageCount(stage.mission)){++stage.mission;stage.stage=0;presentation.missionEnd=true;}
 char name[64];if(presentation.missionEnd)std::snprintf(name,sizeof(name),"GameLoading_%02dE.jpg",std::min(stage.mission,7));else std::snprintf(name,sizeof(name),"GameLoading_%02d.jpg",std::min(stage.mission+1,7));
 presentation.loadingAsset=name;presentation.typoFrame=presentation.missionEnd?0:4;
 if(stage.mission>6){stage.mission=6;stage.stage=0;}gameState=1;host.event({WorldEventKind::SaveProgress,0,stage});
}
void WorldState::resultButton(int id,WorldHost& host){if(gameState==0||presentation.step!=2)return;if(id!=1000&&id!=1001)return;presentation.step=3;if(id==1000)presentation.missionEnd=true;host.event({WorldEventKind::ButtonSound});}
void WorldState::processStageClear(StagePosition stage,WorldHost& host){
 auto& p=presentation;
 if(p.step==0||p.step==1){
  if(p.scale>1){p.scale-=4;if(p.scale<1){p.scale=1;host.event({WorldEventKind::Quake});host.event({WorldEventKind::TitleBang});if(p.step==0)p.step=2;else p.timer=3;}p.rotation+=(-1-p.rotation)*.125f;}
  else if(p.step==1){p.timer-=1.0f/60.0f;if(p.timer<=0){p.step=7;host.event({WorldEventKind::ChangeScreen,gameMode==0?4:3});gameState=0;}}return;
 }
 if(p.step==2){p.alpha+=(1.1f-p.alpha)/50.0f;if(p.alpha>1){p.alpha=1;p.buttonY+=(0-p.buttonY)/10.0f;}return;}
 if(p.step==3){clearObjects(host);p.step=4;return;}
 if(p.step==4){host.event({WorldEventKind::ClearTileResources});if(!p.missionEnd)p.step=5;else {p.step=7;host.event({WorldEventKind::ChangeScreen,4});gameState=0;}return;}
 if(p.step==5){host.event({WorldEventKind::OpenStage,0,stage});p.step=6;return;}
 if(p.step==6){p.buttonY+=(-40-p.buttonY)/10.0f;p.alpha+=(-.1f-p.alpha)/20.0f;
  if(p.alpha<0){p.alpha=0;host.event({WorldEventKind::RemoveLoadingScreen});gameState=0;p.step=7;paused=false;}p.typoAlpha=p.alpha;}
}
void WorldState::configureSurvival(int level){survivalRegenSeed=int(std::max<int64_t>(100,400-int64_t(level)*10));}
void WorldState::processSurvival(const std::function<int32_t()>& random,WorldHost& host){
 if(!random)throw std::invalid_argument("Survival requires original random31 source");
 int seed=survivalRegenSeed*(bossEncounter!=0?5:1);if(seed<=0)throw std::invalid_argument("Invalid survival spawn seed");if(random()%seed!=0)return;
 float x=map.right+30,h=mapHeight(x),y=h==10000?50:h+1;char name[32];int category=random()%4;
 const char* patterns[]={"ArmoredCar%03d","Tank%03d","Truck%03d","Heli%03d"};const int counts[]={315,200,10,10};
 if(category<0)throw std::invalid_argument("random31 must be nonnegative");
 std::snprintf(name,sizeof(name),patterns[category],random()%counts[category]+1);spawnEnemy({name,{x,y+(category==3?200.0f:0.0f)},false},host);
 if(random()%80==0){std::snprintf(name,sizeof(name),"MBoss%02d",random()%10+1);spawnEnemy({name,{x,y},true},host);}
 else if(random()%120==0){std::snprintf(name,sizeof(name),"Boss%02d",random()%6+1);spawnEnemy({name,{x,y},true},host);}
 else if(random()%20==0)spawnEnemy({"Heli001",{x,y+200},false},host);
 if(survivalRegenSeed>50&&random()%10==0)--survivalRegenSeed;
}
void WorldState::tick(std::optional<Vector2> player,StagePosition stage,WorldHost& host,const std::function<int32_t()>& random){
 if(!show)return;if(!paused){gameTime+=double(1.0f/60.0f);if(gameState==0){if(player)processRegen(player->x,host);if(gameMode==1)processSurvival(random,host);}}
 else if(gameState>=1&&gameState<=3)processStageClear(stage,host);
 processCamera(host.cameraTarget(player));
 if(canvasWidth>480&&!paused&&gameState==0&&player)processRegen(player->x,host);
}
float WorldState::moveInput(float y){return std::fabs(y)>.03f?std::clamp(y*15.0f,-1.0f,1.0f):0;}
}
