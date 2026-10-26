#pragma once
#include "GameData.hpp"
#include "CanvasGeometry.hpp"
#include "Motion.hpp"
#include "Visuals.hpp"
#include "GlobalInfo.hpp"
#include <array>
namespace heavymach {
enum class WorldEventKind { ClearObjects, ClearTileResources, ExperienceZero, SelectDefaultWeapon,
 GameStart, SaveProgress, TitleIn, TitleBang, Quake, ButtonSound, ChangeScreen, OpenStage, RemoveLoadingScreen };
struct WorldEvent { WorldEventKind kind; int value=0; StagePosition stage{}; };
struct WorldHost {
 virtual ~WorldHost()=default;
 virtual Vector2 tileSize(std::string_view asset)=0;
 virtual Vector2 mapTileSize(std::string_view asset){return tileSize(asset);}
 virtual void spawnMapObject(const MapObjectDefinition&,int originalLayer)=0;
 // Host constructs GobHvM_Enemy, sets position, state=2, isBoss, layer=1.
 virtual void spawnEnemy(const EnemyDefinition&)=0;
 // Stable map indices distinguish prefetched bosses from active encounters.
 virtual void spawnMapEnemy(const EnemyDefinition& enemy,size_t){spawnEnemy(enemy);}
 virtual bool mapEnemyAlive(size_t)const{return true;}
 // Host constructs GobHvM_Player, upgrades parts, state=2, engineBounce=true, layer=1.
 virtual void spawnPlayer(Vector2)=0;
 virtual void event(const WorldEvent&)=0;
 // processStageClear may remove/replace camObject before processCamera.
 virtual std::optional<Vector2> cameraTarget(std::optional<Vector2> prior)const{return prior;}
};
struct TargetView { uintptr_t id=0; bool inScreen=false; std::function<bool(Vector2)> contains; };
std::optional<uintptr_t> worldTargetAt(bool toPlayer,Vector2,std::span<const TargetView> enemies,const TargetView* player);
struct StagePresentation {
 int step=7;bool missionEnd=false;float timer=0,scale=50,rotation=5,alpha=0,buttonY=-50,typoAlpha=1;
 std::string loadingAsset;int typoFrame=4;
};
class WorldState {
public:
 WorldState()=default;
 WorldState(const WorldState&)=delete;
 WorldState& operator=(const WorldState&)=delete;
 WorldState(WorldState&&)=default;
 WorldState& operator=(WorldState&&)=default;
 MapDefinition map;
 std::vector<GroundSegment> ground;
 std::vector<std::vector<int>> bins;
 std::vector<bool> objectsSpawned,enemiesSpawned,bossEncounterActivated,mapEnemiesDefeated;
 std::vector<float> objectHalfWidths;
 std::array<ParticleState,64> particles;
 Vector2 camera{},backgroundPosition{},center{},playerSpawn{};
 float canvasHeight=320,canvasWidth=480,backgroundDrawScale=1,preparedVisibleRight=-10000;Vector2 backgroundSize{};
 float backLine=-240,frontLine=240,bgScale=.5f,objectRegenPosition=-10000;
 int bossCount=0,bossEncounter=0,gameMode=0,survivalRegenSeed=0;
 int16_t gameState=0;bool paused=false,show=true,hasBackground=false;
 double gameTime=0;
 StagePresentation presentation;
 // Existing object graph must be cleared by caller before replacing a live map.
 void openMap(const MapDefinition&,int mode,WorldHost&);
 std::span<const int> collisionCandidates(float x) const;
 float mapHeight(float x) const;
 bool sweepTerrain(Vector2& point,const Vector2& delta) const;
 MotionWorld motionWorld(float x,double globalTime,std::function<int32_t()> random={}) const;
 void setCanvasWidth(float width);
 void updateBackgroundGeometry();
 void processCamera(std::optional<Vector2> followedPosition);
 void processRegen(float playerX,WorldHost&);
 void spawnEnemy(const EnemyDefinition&,WorldHost&,std::optional<size_t> mapIndex={});
 void activateBossEncounter(const EnemyDefinition&);
 void killMapBoss(size_t mapIndex);
 void killBoss();
 void removeTrash(float boundary,std::span<const TargetView> objects,
                  const std::function<bool(uintptr_t,float)>& isBehind,
                  const std::function<void(uintptr_t)>& remove);
 ParticleState* freeParticle(double globalTime);
 void clearObjects(WorldHost&);
 void nextStage(StagePosition&,WorldHost&);
 void resultButton(int buttonId,WorldHost&);
 void processStageClear(StagePosition,WorldHost&);
 void configureSurvival(int internalLevel);
 void processSurvival(const std::function<int32_t()>& random,WorldHost&);
 // Host ticks machines, bullets and children after this ordered world prelude.
 void tick(std::optional<Vector2> playerPosition,StagePosition,WorldHost&,
           const std::function<int32_t()>& random={});
 static float moveInput(float accelY);
};
}
