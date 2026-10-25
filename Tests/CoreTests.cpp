#include "GlobalInfo.hpp"
#include "Viewport.hpp"
#include "TitleState.hpp"
#include "TiltState.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
using namespace heavymach;
void runGeometryTests();
void runMotionTests();
void runGameDataTests();
void runEnemyTests();
void runPlayerTests();
void runItemBoxTests();
void runBulletVisualTests();
void runTrailTests();
void runTrailRasterTests();
void runCameraShakeTests();
void runLoadingTransitionTests();
void runTitleStateTests();

int main(int argc, char** argv) {
    runGeometryTests(); runMotionTests(); runGameDataTests(); runEnemyTests(); runPlayerTests(); runItemBoxTests(); runBulletVisualTests(); runTrailTests(); runTrailRasterTests(); runCameraShakeTests(); runLoadingTransitionTests(); runTitleStateTests();
    TiltState tilt; tilt.sample(0,1,0); assert(tilt.y==.1f);
    tilt.sample(0,0,0); assert(tilt.y==float(double(.1f)*(1.0-double(.1f))));
    const auto viewport=Viewport::fit(62,0,830,440);
    assert(viewport.logical(viewport.x,viewport.y)==std::pair(0.f,320.f));
    assert(viewport.logical(viewport.x+480*viewport.scale,viewport.y+320*viewport.scale)==std::pair(480.f,0.f));
    assert(!viewport.logical(viewport.x-1,viewport.y));
    const auto odd=Viewport::drawable(2622,1206,3);
    assert(odd.x*3==406 && odd.y==0);
    assert(480*odd.scale*3==1809 && 320*odd.verticalScale()*3==1206);
    assert(odd.logical(odd.x,odd.y)==std::pair(0.f,320.f));
    assert(odd.logical(odd.x+480*odd.scale,odd.y+320*odd.verticalScale())==std::pair(480.f,0.f));
    for(auto size:std::vector<std::pair<int,int>>{{960,640},{1920,1080},{2736,1260},{2868,1320},{2623,1207}}){
      auto canvas=CanvasGeometry::forDrawable(size.first,size.second,true);
      assert(canvas.width>=480&&canvas.height==320&&canvas.rasterWidth()>=canvas.width);
      auto full=Viewport::drawable(size.first,size.second,3,canvas.width);
      assert(full.x==0&&full.y==0);
      auto center=full.logical(size.first/6.,size.second/6.);
      assert(center&&std::fabs(center->first-canvas.centerX())<.001&&std::fabs(center->second-160)<.001);
      assert(!full.logical(-1,0));
      auto right=full.logical(full.x+canvas.width*full.scale,full.y+320*full.verticalScale());
      assert(right&&std::fabs(right->first-canvas.width)<.001&&right->second==0);
      assert(CanvasGeometry::forDrawable(size.first,size.second,false).width==480);
    }
    TitleState title;
    assert(!title.released(0));
    assert(title.released(0));
    for (int i=1;i<=120;i++) title.tick(i/60.,0,0);
    assert(title.showTitle && title.showEpisode && title.titleScale==1 && title.episodeScale==1);
    assert(GlobalInfo::thresholds(65).next==1831618501);
    assert(GlobalInfo::thresholds(66).next==INT32_MAX);
    assert(GlobalInfo::thresholds(67).previous==INT32_MAX);
    assert(GlobalInfo::thresholds(99).next==INT32_MAX);
    SaveSlot high;high.bytes[8]=1;high.setInt(0x28,66);high.setInt(0x20,INT32_MAX);
    assert(GlobalInfo(high).addExperience(0).leveledUp);
    assert(high.level()==67 && std::isnan(high.f32(0x24)));high.validate();
    GlobalInfo(high).addExperience(1);assert(high.experience()==INT32_MIN);
    SaveData save;
    auto& slot = save.slots[1];
    slot.bytes[8] = 1;
    slot.bytes[0x400] = 0xA7; // Unknown original fields must survive round trips.
    GlobalInfo info(slot);
    // Original first threshold is 300; UI displays internal level + 1.
    assert(GlobalInfo::thresholds(0).previous == 0);
    assert(GlobalInfo::thresholds(0).next == 300);
    assert(!info.addExperience(299).leveledUp);
    assert(slot.level() == 0 && slot.bytes[0x2c] == 0);
    assert(info.addExperience(1).leveledUp);
    assert(slot.level() == 1 && slot.bytes[0x2c] == 1);
    assert(slot.f32(0x24) == 0);
    assert(info.addExperience(100000).leveledUp);
    assert(slot.level() == 2); // A while-loop would incorrectly grant many levels.
    assert(slot.f32(0x24) == 1);

    assert(info.addWeaponItem("P_Wpn_Gun01",99) == 0);
    assert(info.addWeaponItem("P_Wpn_Canon01",10) == 1);
    for (int i=0;i<8;++i) info.addWeaponItem("P_Wpn_Canon01",10);
    assert(slot.weaponCharge(1) == 60 && slot.weaponCount() == 2);
    info.addWeaponItem("P_Wpn_Canon01",5);
    assert(slot.weaponCharge(1) == 30); // Cap depends on this pickup, even when smaller.
    info.addWeaponItem("P_Wpn_Laser",3);
    assert(info.selectWeapon(4) == 1);
    assert(!info.removeWeapon(0));
    assert(info.removeWeapon(1));
    assert(slot.weaponName(1) == "P_Wpn_Laser" && slot.weaponCount() == 2);
    assert(!info.selectWeapon(-1));

    // Original strcpy preserves bytes following the terminator; memcpy uses
    // (newCount * 28 - index), including stale inventory bytes (0x10944).
    SaveSlot exact; exact.bytes.fill(0x5a); exact.bytes[8]=0;
    exact.setInt(0x30,0); GlobalInfo exactInfo(exact);
    exactInfo.addWeaponItem("Gun",1);
    assert(exact.bytes[0x38]==0x5a);
    exactInfo.addWeaponItem("Laser",2);
    exactInfo.addWeaponItem("Bomb",3);
    auto expected=exact.bytes; const size_t offset=0x34+28;
    for(size_t i=0;i<2*28-1;++i)expected[offset+i]=exact.bytes[offset+28+i];
    expected[0x30]=2;
    assert(exactInfo.removeWeapon(1)); assert(exact.bytes==expected);
    exact.setWeapon(0,"Gun",INT32_MAX);
    exactInfo.addWeaponItem("Gun",1); assert(exact.weaponCharge(0)==INT32_MIN);
    exactInfo.addWeaponItem("Large",INT32_MAX);
    assert(exact.weaponCharge(2)==-6); // ARM32 shift/add then signed cap.

    assert(GlobalInfo::mapNumber({3,0}) == 25);
    assert(GlobalInfo::mapNumber({4,0}) == 15);
    auto next = GlobalInfo::nextStage({0,3});
    assert(next.mission == 1 && next.stage == 0);
    assert(info.updateLastStage(next));
    assert(!info.updateLastStage({0,2}));
    assert(slot.i32(0xc)==1 && slot.i32(0x10)==0 && slot.i32(0x14)==4);
    next=GlobalInfo::nextStage({6,5});
    assert(next.mission==6 && next.stage==0);
    assert(GlobalInfo::maximumHP(0)==256 && GlobalInfo::maximumHP(1)==256);
    assert(GlobalInfo::maximumHP(2)==320); // float 1.3 promoted to double: 64.999997...
    assert(GlobalInfo::movementSpeed(0)==1.3f);

    const auto encoded = save.encode();
    assert(SaveData::decode(encoded).encode() == encoded);
    bool rejected=false;
    try { (void)SaveData::decode(std::span(encoded).first(100)); }
    catch (const std::runtime_error&) { rejected=true; }
    assert(rejected);
    auto broken=encoded;
    broken[SaveSlot::byteCount + 0x30] = 255;
    rejected=false;
    try { (void)SaveData::decode(broken); } catch (const std::runtime_error&) { rejected=true; }
    assert(rejected);
    // Optional read-only comparison against a save produced by the ORIGINAL guest.
    for (int i=1;i<argc;++i) {
        std::ifstream file(argv[i],std::ios::binary);
        if (!file) throw std::runtime_error("cannot open original save fixture");
        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)),{});
        const auto roundTrip = SaveData::decode(bytes).encode();
        assert(std::equal(bytes.begin(),bytes.end(),roundTrip.begin(),roundTrip.end()));
        std::cout << "original HvM.dat exact round-trip: " << argv[i] << '\n';
    }
    std::cout << "Core regression checks passed; this is not full gameplay equivalence.\n";
}
