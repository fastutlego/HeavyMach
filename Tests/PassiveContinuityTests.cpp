#include "../Sources/Core/GameSession.hpp"
#include <bit>
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace heavymach;
namespace {
void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
struct PickupHost final:PlayerHost {
 int32_t random31()override{return 0;}bool acquireParticle()override{return false;}
 void event(const PlayerEvent&)override{}
};
}
int main(){try{
 const auto resources=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources";
 auto data=GameData::loadDirectory(resources/"RawData");SaveSlot slot;GlobalInfo(slot).selectSlot(0);
 auto tileSize=[&](std::string_view name){auto path=resources/"Images"/name;if(!std::filesystem::exists(path))path+=".png";std::ifstream f(path,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);require(f.gcount()==24,"PNG dimension fixture");auto be=[&](int n){return uint32_t(b[n])<<24|uint32_t(b[n+1])<<16|uint32_t(b[n+2])<<8|b[n+3];};return Vector2{float(be(16)),float(be(20))};};
 GameSession session(data,slot,tileSize,[]{return 0;});session.start(0,0,0);
 // Fixture setup only: place a live player at the original exit gate and isolate
 // boss/regen combat. All transition state changes below use actual tick/buttons.
 auto& world=const_cast<WorldState&>(session.world());auto* player=const_cast<GobHvM_Player*>(session.player());
 require(player,"initial player");world.frontLine=world.map.right+10;world.bossCount=0;
 player->position={world.map.right-20,world.map.top+100};player->machState=2;
 PickupHost host;for(auto name:{"Defense","Attack","Booster","Speed"})player->passives.add(name,host);
 double time=0;auto tick=[&]{time+=1.0/60.0;session.tick(time,1);};tick();
 require(session.stage().stage==1&&world.gameState==1&&world.paused,"actual Motion exit starts next-stage transition");
 for(int n=0;n<30&&world.presentation.step!=2;++n)tick();require(world.presentation.step==2,"result panel ready");
 // Cross the integer display boundary on the precise tick which destroys the
 // player. Original GuiGame::processPassiveSlot still runs after World tick.
 for(auto& effect:player->passives.slots){effect.time=1.01f;effect.number=1;}
 session.takeEvents();session.resultButton(1001);require(world.presentation.step==3,"next stage button accepted");
 float expected=1.01f;const float dt=std::bit_cast<float>(0x3c888889u);
 tick();expected-=dt;require(!session.player()&&world.presentation.step==4,"clear tick has no player");
 auto events=session.takeEvents();int passiveUpdates=0;
 for(auto& e:events)if(e.kind==SessionEventKind::PlayerUI&&e.code==int(PlayerEventKind::Passive)&&e.active&&e.amount==0)++passiveUpdates;
 int failures=0;if(passiveUpdates!=4){std::cerr<<"FAIL player-absent transition tick: expected 4 passive countdown events, actual "<<passiveUpdates<<'\n';++failures;}
 tick();expected-=dt;require(!session.player()&&world.presentation.step==5,"resource-clear tick has no player");
 tick();expected-=dt;require(session.player()&&world.presentation.step==6,"real openMap creates next player");
 for(int i=0;i<4;++i){const auto& effect=session.player()->passives.slots[i];if(!effect.active||effect.time!=expected){std::cerr<<"FAIL carried passive "<<i<<": expected active/time="<<expected<<" actual "<<effect.active<<'/'<<effect.time<<'\n';++failures;}}
 if(failures)return 1;
 // GameMain/GuiGame lifetime outlasts both the world session and selected slot.
 PlayerPassives shared;SaveSlot firstSlot,secondSlot;
 GlobalInfo(firstSlot).selectSlot(0);GlobalInfo(secondSlot).selectSlot(1);
 float sharedExpected=60.f;
 {
  GameSession first(data,firstSlot,tileSize,[]{return 0;},&shared);first.start(0,0,0);
  require(&first.player()->passives==&shared,"first player binds external owner");
  const auto saved=firstSlot.bytes;
  for(auto name:{"Defense","Attack","Booster","Speed"})shared.add(name,host);
  require(firstSlot.bytes==saved,"adding passives does not persist into raw save bytes");
  first.setPaused(true);first.tick(1,0);sharedExpected-=dt;
  // Moving the session transfers its heap Impl; references must remain valid.
  GameSession moved(std::move(first));
  require(&moved.player()->passives==&shared,"session move preserves external reference");
  require(firstSlot.bytes==saved,"paused passive tick does not persist into save");
 }
 const auto firstSaved=firstSlot.bytes,secondSaved=secondSlot.bytes;
 for(int n=0;n<30;++n){shared.tick();sharedExpected-=dt;}
 for(auto& effect:shared.slots)require(effect.active&&effect.time==sharedExpected,"menu ticks consume shared remaining time without player");
 require(firstSlot.bytes==firstSaved&&secondSlot.bytes==secondSaved,"menu ticks do not write either save slot");
 {
  GameSession second(data,secondSlot,tileSize,[]{return 0;},&shared);second.start(0,0,0);
  require(&second.player()->passives==&shared,"different slot reentry binds same UI owner");
  for(auto& effect:second.player()->passives.slots)require(effect.active&&effect.time==sharedExpected,"different slot retains all passive durations");
  const auto saved=secondSlot.bytes;
  second.setPaused(true);second.tick(2,0);sharedExpected-=dt;
  for(auto& effect:shared.slots)require(effect.time==sharedExpected,"reentered paused session ticks shared owner exactly once");
  require(secondSlot.bytes==saved,"reentry passive tick does not persist into raw save");
 }
 // Expiration occurs while no GameSession exists. Reentry must not revive it.
 int menuTicks=0;
 while(shared.active(0)&&menuTicks<4000){shared.tick();++menuTicks;}
 require(menuTicks>0&&menuTicks<4000,"effects expire during menu residence");
 for(auto& effect:shared.slots)require(!effect.active&&effect.time==0,"all menu effects expire and clamp time to zero");
 {
  GameSession expired(data,firstSlot,tileSize,[]{return 0;},&shared);expired.start(0,0,0);
  for(auto& effect:expired.player()->passives.slots)require(!effect.active&&effect.time==0,"expired effects stay expired on reentry");
 }
 // A fresh application/UI owner cannot recover temporary effects from a slot.
 shared.add("Attack",host);PlayerPassives fresh;
 GameSession freshSession(data,firstSlot,tileSize,[]{return 0;},&fresh);freshSession.start(0,0,0);
 for(auto& effect:freshSession.player()->passives.slots)require(!effect.active&&effect.time==0,"fresh UI owner starts empty despite reused save");
 require(shared.active(1),"fresh owner does not reset another owner");
 std::cout<<"PASS external lifetime: two slots, session destruction/move, menu countdown/expiration, fresh owner and raw-save isolation\n";
 std::cout<<"PASS all 4 effects survive clear/openMap; countdown includes both player-absent ticks\n";return 0;
}catch(const std::exception& e){std::cerr<<"Fixture/error: "<<e.what()<<'\n';return 2;}}
