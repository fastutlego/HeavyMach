#include "../Sources/Core/Player.hpp"
#include <cassert>
#include <cmath>
#include <vector>
using namespace heavymach;
namespace {
struct Host:PlayerHost {
 std::vector<int32_t> random;size_t cursor=0;std::vector<PlayerEvent> events;bool pool=true;int poolCalls=0;
 int32_t random31()override{assert(cursor<random.size());return random[cursor++];}
 bool acquireParticle()override{++poolCalls;return pool;}
 void event(const PlayerEvent& e)override{events.push_back(e);}
 void reset(std::initializer_list<int32_t> seq){random=seq;cursor=0;events.clear();poolCalls=0;}
};
SaveSlot slot(){SaveSlot s;GlobalInfo(s).selectSlot(0);s.setInt(0x18,256);return s;}
}
void runPlayerTests(){
 auto s=slot();GobHvM_Player p(s);Host h;GameData data;
 assert(originalLootTable().size()==22&&originalLootTable()[21].name=="P_Wpn_ElShock");
 // Unlock branch consumes exactly one draw; missing weapon still unlocks and sounds.
 h.reset({0});p.pickupItem(data,h);assert(s.bytes[0x3b4]==13&&h.cursor==1&&h.poolCalls==0);assert(h.events.back().index==3);
 // Step22 weighted lottery consumes no unlock draw. Boundary100 selects HP_Max.
 s.bytes[0x3b4]=22;p.hp=1;h.reset({100});p.pickupItem(data,h);assert(h.cursor==1&&p.hp==256);assert(h.events[1].kind==PlayerEventKind::PickupParticle&&h.events[1].index==1);
 // Prior to22 a failed unlock draws again; weighted117 selects Defense.
 s.bytes[0x3b4]=12;h.reset({1,117});p.pickupItem(data,h);assert(h.cursor==2&&p.passives.active(0));assert(p.passives.slots[0].time==60);
 p.passives.add("Defense",h);assert(p.passives.slots[0].time==120);p.passives.tick(h);assert(p.passives.slots[0].number==119);
 p.passives.slots[0].time=.001f;p.passives.tick(h);assert(!p.passives.active(0)&&p.passives.slots[0].time==0);assert(!p.passives.active(-1));
 // New weapon callback, definition charge and uiTile, one lottery draw at22.
 s.bytes[0x3b4]=22;WeaponInfo wi;wi.name="P_Wpn_Shotgun01";wi.charge=7;wi.uiTile=12;data.weapons.emplace(wi.name,wi);
 h.reset({317});p.pickupItem(data,h);assert(s.weaponName(4)==wi.name&&s.weaponCharge(4)==7);assert(h.events[0].kind==PlayerEventKind::WeaponInventory&&h.events[1].index==12);
 // One level per add; random selection before level sound; points are cumulative.
 h.reset({2});p.addExperience(300,h);assert(s.level()==1&&s.bytes[0x2c]==1&&p.upgradeSelection==2);assert(h.events[0].kind==PlayerEventKind::UpgradeOpen&&h.events[1].index==6);
 p.confirmUpgrade(h);assert(s.bytes[0x2d]==1&&s.bytes[0x2c]==1);
 s.bytes[0x2e]=5;p.hp=1;assert(p.upgradeParts(1,h));assert(s.bytes[0x2e]==6&&p.hp==s.i32(0x1c)&&p.hp==s.i32(0x18));assert(!p.upgradeParts(1,h));
 s.bytes[0x2f]=6;s.bytes[0x2d]=6;h.reset({0});p.openUpgrade(h);assert(p.upgradeSelection==100);
 // Exact touch boundaries and release behavior.
 WeaponState w;p.hp=256;s.bytes[0x2f]=3;p.dashVel=0;p.position={30,40};
 p.setTap({-1,0},0,{0,0},{0,0},&w);assert(p.dashVel==-5&&!w.isFire);
 p.setTap({100,0},0,{0,0},{0,0},&w);assert(!w.isFire);
 p.setTap({101,0},1,{0,0},{0,0},&w);assert(w.isFire&&w.fireTarget.x==131);
 p.setTap({-100,-100},2,{0,0},{0,0},&w);assert(!w.isFire&&w.fireTarget.x==-70);
 // Fatal damage event order retains death, hit sound, gauge, quake.
 h.reset({2});p.hp=10;p.onDamage(11,h);assert(p.hp==0&&h.cursor==1);
 assert(h.events[0].kind==PlayerEventKind::Sound&&h.events[0].index==14);
 assert(h.events[1].kind==PlayerEventKind::GameOver&&h.events[2].index==10);
 assert(h.events.back().kind==PlayerEventKind::Quake&&h.events.back().value==10);
}
