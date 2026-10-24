#include "../Sources/Core/World.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <array>
#include <algorithm>
using namespace heavymach;
namespace {
void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
struct Case {int mission,stage,map,nextMission,nextStage,nextMap;bool missionEnd;};
// Literal oracle transcribed from original init0xfd44 and setNextStage0xb8f0.
// No production map/nextStage function constructs these expected rows.
constexpr Case cases[]={
 {0,0,1,0,1,2,false},{0,1,2,0,2,3,false},{0,2,3,0,3,4,false},{0,3,4,1,0,5,true},
 {1,0,5,1,1,6,false},{1,1,6,1,2,7,false},{1,2,7,1,3,8,false},{1,3,8,1,4,9,false},{1,4,9,2,0,10,true},
 {2,0,10,2,1,11,false},{2,1,11,2,2,12,false},{2,2,12,2,3,13,false},{2,3,13,2,4,14,false},{2,4,14,3,0,25,true},
 {3,0,25,3,1,26,false},{3,1,26,3,2,27,false},{3,2,27,3,3,28,false},{3,3,28,3,4,29,false},{3,4,29,4,0,15,true},
 {4,0,15,4,1,16,false},{4,1,16,4,2,17,false},{4,2,17,4,3,18,false},{4,3,18,4,4,19,false},{4,4,19,5,0,20,true},
 {5,0,20,5,1,21,false},{5,1,21,5,2,22,false},{5,2,22,5,3,23,false},{5,3,23,5,4,24,false},{5,4,24,6,0,30,true},
 {6,0,30,6,1,31,false},{6,1,31,6,2,32,false},{6,2,32,6,3,33,false},{6,3,33,6,4,34,false},{6,4,34,6,5,35,false},{6,5,35,6,0,30,true},
 // Direct-only mission7: clamp occurs even before that mission's final stage.
 {7,0,36,6,0,30,false},{7,1,37,6,0,30,false},{7,2,38,6,0,30,false},{7,3,39,6,0,30,false},{7,4,40,6,0,30,false},{7,5,41,6,0,30,true}
};
std::string key(int n){char b[32];std::snprintf(b,sizeof(b),"Stage%02d",n);return b;}
struct Host final:WorldHost {
 WorldState& world;const GameData& data;SaveSlot& slot;std::filesystem::path images;
 std::vector<WorldEvent> events;std::vector<int> opened;int screen=-1,players=0;bool routeOpen=true;
 Host(WorldState& w,const GameData& d,SaveSlot& s,std::filesystem::path i):world(w),data(d),slot(s),images(std::move(i)){}
 Vector2 tileSize(std::string_view name)override{auto p=images/name;if(!std::filesystem::exists(p))p+=".png";std::ifstream f(p,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);check(f.gcount()==24,"asset size read");auto be=[&](int i){return uint32_t(b[i])<<24|uint32_t(b[i+1])<<16|uint32_t(b[i+2])<<8|b[i+3];};return {float(be(16)),float(be(20))};}
 void spawnMapObject(const MapObjectDefinition&,int)override{}
 void spawnEnemy(const EnemyDefinition&)override{}
 void spawnPlayer(Vector2)override{++players;}
 void event(const WorldEvent& e)override{events.push_back(e);if(e.kind==WorldEventKind::SaveProgress){GlobalInfo info(slot);info.updateLastStage(e.stage);}if(e.kind==WorldEventKind::ChangeScreen)screen=e.value;
  if(e.kind==WorldEventKind::OpenStage){int n=GlobalInfo::mapNumber(e.stage);opened.push_back(n);if(routeOpen)world.openMap(data.maps.at(key(n)),0,*this);}}
 int count(WorldEventKind kind)const{return int(std::count_if(events.begin(),events.end(),[&](auto& e){return e.kind==kind;}));}
};
bool exits(WorldState& world,float x,float speed,int hp=256,int state=2,bool player=true,bool visible=true){
 GobHvMach m;m.player=player;m.visible=visible;m.machState=int16_t(state);m.machType=1;m.position={x,world.map.top+100};m.speed=speed;m.hp=hp;
 auto result=m.tick(world.motionWorld(x,0));return std::any_of(result.begin(),result.end(),[](auto& e){return e.kind==MotionEventKind::NextStage;});
}
}
int main(int argc,char**argv){try{
 const auto resources=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";auto data=GameData::loadDirectory(resources/"RawData");
 std::ofstream out;if(argc>1)out.open(argv[1]);int normal=0,direct=0,gates=0;
 constexpr std::array<int,8> expectedCounts{4,5,5,5,5,5,6,6};for(int m=0;m<8;++m)check(GlobalInfo::stageCount(m)==expectedCounts[m],"original mission count");
 for(size_t index=0;index<std::size(cases);++index){const auto& c=cases[index];StagePosition s{c.mission,c.stage};check(GlobalInfo::mapNumber(s)==c.map,"original map order");check(GlobalInfo::currentStage(s)==int(index),"original ordinal");auto next=GlobalInfo::nextStage(s);check(next.mission==c.nextMission&&next.stage==c.nextStage,"GlobalInfo next oracle");check(GlobalInfo::mapNumber(next)==c.nextMap,"next map oracle");
  SaveSlot slot;GlobalInfo info(slot);info.selectSlot(0);slot.setInt(0xc,c.mission);slot.setInt(0x10,c.stage);slot.setInt(0x14,int(index));WorldState w;Host h(w,data,slot,resources/"Images");w.openMap(data.maps.at(key(c.map)),0,h);
  // Exit fixtures intentionally isolate original gates, not actual combat.
  w.frontLine=w.map.right+10;w.bossCount=0;const float edge=w.map.right-30;const float over=std::nextafter(edge,INFINITY);
  auto gate=[&](bool actual,bool expected){check(actual==expected,"Motion exit gate");++gates;};
  gate(exits(w,std::nextafter(edge,-INFINITY),1),false);gate(exits(w,edge,1),false);gate(exits(w,over,1),true);
  gate(exits(w,over,0),false);gate(exits(w,over,-1),false);gate(exits(w,over,1,0),true);gate(exits(w,over,1,-1),true);
  gate(exits(w,over,1,256,0),false);gate(exits(w,over,1,256,1),true);gate(exits(w,over,1,256,3),false);gate(exits(w,over,1,256,2,false),false);gate(exits(w,over,1,256,2,true,false),false);
  w.paused=true;gate(exits(w,over,1),false);w.paused=false;w.gameMode=1;gate(exits(w,over,1),false);w.gameMode=0;
  w.bossCount=1;gate(exits(w,over,1),false);w.bossCount=-1;gate(exits(w,over,1),true);w.bossCount=0;
  // Encounter counter itself is not read by Motion; the frontLine is its gate.
  w.bossEncounter=1;gate(exits(w,over,1),true);w.bossEncounter=0;
  w.spawnEnemy({"Boss01",{w.map.right-200,0},true},h);w.spawnEnemy({"Boss01",{w.map.right-200,0},true},h);
  gate(exits(w,over,1),false);w.killBoss();check(w.bossEncounter==1,"one remaining encounter");gate(exits(w,over,1),false);w.killBoss();check(w.bossEncounter==0,"last encounter removed");gate(exits(w,over,1),true);
  h.events.clear();w.gameTime=123.5;const auto gateEvents=exits(w,over,1);check(gateEvents,"fixture exit emits");w.nextStage(s,h);
  check(s.mission==c.nextMission&&s.stage==c.nextStage,"World next oracle");check(w.paused&&w.gameState==1&&w.presentation.step==0,"clear starts paused");check(w.presentation.missionEnd==c.missionEnd,"mission end oracle");check(w.gameTime==123.5,"transition retains clock");
  char loading[64];int loadingMission=c.mission==7?7:c.mission+1;std::snprintf(loading,sizeof(loading),c.missionEnd?"GameLoading_%02dE.jpg":"GameLoading_%02d.jpg",loadingMission);check(w.presentation.loadingAsset==loading,"loading asset oracle");
  check(h.events.size()==3&&h.events[0].kind==WorldEventKind::RemoveLoadingScreen&&h.events[1].kind==WorldEventKind::TitleIn&&h.events[2].kind==WorldEventKind::SaveProgress,"clear event order");
  // Forward progress only. Final/direct clamp cannot move saved progress backward.
  bool advances=index<34;check(slot.i32(0xc)==(advances?c.nextMission:c.mission)&&slot.i32(0x10)==(advances?c.nextStage:c.stage),"saved stage");check(slot.i32(0x14)==int(advances?index+1:index),"saved farthest");
  w.resultButton(1001,h);check(w.presentation.step==0,"button before ready ignored");for(int tick=0;tick<13;++tick)w.processStageClear(s,h);check(w.presentation.step==2&&w.presentation.scale==1,"13 tick ready");check(h.count(WorldEventKind::TitleBang)==1&&h.count(WorldEventKind::Quake)==1,"ready effects once");
  w.resultButton(999,h);check(w.presentation.step==2,"unknown button ignored");w.resultButton(1001,h);check(w.presentation.step==3,"result accepted");w.resultButton(1001,h);check(w.presentation.step==3&&h.count(WorldEventKind::ButtonSound)==1,"duplicate result ignored");
  w.processStageClear(s,h);check(w.presentation.step==4&&h.count(WorldEventKind::ClearObjects)==1,"clear objects step");w.processStageClear(s,h);check(h.count(WorldEventKind::ClearTileResources)==1,"clear textures step");
  if(c.missionEnd){check(h.screen==4&&h.opened.empty()&&w.gameState==0,"mission returns menu without opening map");}
  else{check(w.presentation.step==5,"open pending");w.processStageClear(s,h);check(h.opened==std::vector<int>{c.nextMap}&&w.presentation.step==6,"actual mapped open");check(w.map.left==data.maps.at(key(c.nextMap)).left&&w.map.right==data.maps.at(key(c.nextMap)).right,"next map bounds");check(h.players==2,"next player spawn");for(int i=0;i<120&&w.gameState!=0;++i)w.processStageClear(s,h);check(!w.paused&&w.gameState==0&&w.presentation.step==7,"fade completes");check(h.screen==-1,"in mission no menu");}
  check(w.gameTime==123.5,"open/result retains clock");
  // Return-menu button on a non-final map is an original supported alternate.
  WorldState menuWorld;Host menuHost(menuWorld,data,slot,resources/"Images");StagePosition menuStage{c.mission,c.stage};menuWorld.nextStage(menuStage,menuHost);for(int t=0;t<13;++t)menuWorld.processStageClear(menuStage,menuHost);menuWorld.resultButton(1000,menuHost);menuWorld.processStageClear(menuStage,menuHost);menuWorld.processStageClear(menuStage,menuHost);check(menuHost.screen==4&&menuHost.opened.empty(),"explicit menu result");
  (c.mission<7?normal:direct)++;if(out)out<<"{\"map\":\""<<key(c.map)<<"\",\"scope\":\""<<(c.mission<7?"normal":"direct-only")<<"\",\"next_map\":\""<<key(c.nextMap)<<"\",\"mission_end\":"<<(c.missionEnd?"true":"false")<<",\"status\":\"transition_contract_pass\",\"journey_clear\":false}\n";
 }
 std::cout<<"Transition contracts normal="<<normal<<" direct-only="<<direct<<" motion gates="<<gates<<"; no journeys executed\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
