#import <XCTest/XCTest.h>
#import <objc/runtime.h>
#import <CoreMotion/CoreMotion.h>
#import "../../Sources/iOS/HMGameView.h"
#import "../../Sources/iOS/HMGameHUD.h"
#import "../../Sources/iOS/HMMedia.h"
// This test bundle is hosted only by the HM_VALIDATION=1 app target.
#define HM_VALIDATION 1
#include "../../Sources/Core/GameSession.hpp"
#include <bit>
#include <stdexcept>

@interface HMGameView (PassiveLifecycleTest)
- (void)startSlot:(int)slot mode:(int)mode mission:(int)mission stage:(int)stage;
- (void)refreshScreen:(int)screen;
- (void)frame:(CADisplayLink *)link;
- (void)gameButton:(int)identifier state:(int)state;
@end
namespace {
template<class T> T& storage(id object,const char* name){
    Ivar ivar=class_getInstanceVariable(HMGameView.class,name);
    if(!ivar)throw std::runtime_error(name);
    return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>((__bridge void*)object)+ivar_getOffset(ivar));
}
struct PassiveHost final:heavymach::PlayerHost {
    int32_t random31()override{return 0;}bool acquireParticle()override{return false;}
    void event(const heavymach::PlayerEvent&)override{}
};
}
// The real view/session/menu clock runs. Only persistence and audio/event output
// are intercepted, so neither test nor user save files are changed or played.
@interface HMPassiveTestFrame : NSObject
@property(nonatomic) CFTimeInterval timestamp;
@end
@implementation HMPassiveTestFrame
@end
@interface HMPassiveTestView : HMGameView
@end

@interface HMTimingSilentMedia : HMMedia
@end
@implementation HMTimingSilentMedia
- (void)sound:(NSString*)name {}
- (void)sound:(NSString*)name pan:(float)pan {}
@end
@interface HMTimingTestView : HMGameView
@end
@implementation HMTimingTestView
- (void)persistSave {}
@end
@interface HMGameEventTimingTests : XCTestCase
@end
@implementation HMGameEventTimingTests
- (HMTimingTestView*)newFixture {
    HMTimingTestView *view=[[HMTimingTestView alloc] initWithFrame:CGRectMake(0,0,480,320)];
    [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
    [(CMMotionManager*)[view valueForKey:@"motion"] stopAccelerometerUpdates];
    [view setValue:[HMTimingSilentMedia new] forKey:@"media"];
    [view setValue:nil forKey:@"loadError"];
    auto& save=storage<heavymach::SaveData>(view,"_save");save={};
    for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);
    return view;
}
- (void)oneTick:(HMGameView*)view {
    HMPassiveTestFrame *frame=[HMPassiveTestFrame new];frame.timestamp=1;

    [view frame:(CADisplayLink*)frame];
}
- (void)testHPRefreshAffectsTheSameHUDTickWithoutDrawing {
    auto *view=[self newFixture];[view startSlot:0 mode:0 mission:0 stage:0];
    auto& session=storage<std::unique_ptr<heavymach::GameSession>>(view,"_session");XCTAssertTrue(bool(session));if(!session)return;
    HMGameHUD *hud=[view valueForKey:@"hud"];
    [hud setValue:@.5 forKey:@"hp"];[hud setValue:@.5 forKey:@"hpDest"];
    // Actual defense upgrade emits HPGauge1 before the following HUD tick.
    XCTAssertTrue(session->selectUpgrade(1));session->confirmUpgrade();session->setPaused(true);
    [self oneTick:view];XCTAssertEqual([[hud valueForKey:@"hp"] floatValue],.5625f);
}
- (void)testUpgradeOpenAndCloseUseEventsBeforeHUDTick {
    auto *view=[self newFixture];[view startSlot:0 mode:0 mission:0 stage:0];
    auto& session=storage<std::unique_ptr<heavymach::GameSession>>(view,"_session");XCTAssertTrue(bool(session));if(!session)return;
    HMGameHUD *hud=[view valueForKey:@"hud"];
    session->validationAddExperience(300);[self oneTick:view];
    XCTAssertEqual([[hud valueForKey:@"upgradeStep"] intValue],1);
    XCTAssertEqual([[hud valueForKey:@"upgradeX"] floatValue],-144.f);
    // No draw calls between simulation ticks: opening still advances each tick.
    [self oneTick:view];XCTAssertEqualWithAccuracy([[hud valueForKey:@"upgradeX"] floatValue],-67.2f,.00001);
    [view gameButton:1300 state:2];float before=[[hud valueForKey:@"upgradeX"] floatValue];
    XCTAssertEqual([[hud valueForKey:@"upgradeStep"] intValue],2);
    [self oneTick:view];XCTAssertEqual([[hud valueForKey:@"upgradeX"] floatValue],before+(660.f-before)/5);
}
- (void)testScreenEntryUnpausesAfterLeftoverExperienceOpensUpgrade {
    auto *view=[self newFixture];auto& save=storage<heavymach::SaveData>(view,"_save");
    // Real EXP routine advances only one level per call; this leaves EXP1000/L1.
    auto change=heavymach::GlobalInfo(save.slots[0]).addExperience(1000);
    XCTAssertTrue(change.leveledUp);XCTAssertEqual(save.slots[0].level(),1);
    [view startSlot:0 mode:0 mission:0 stage:0];
    auto& session=storage<std::unique_ptr<heavymach::GameSession>>(view,"_session");XCTAssertTrue(bool(session));if(!session)return;
    XCTAssertEqual(save.slots[0].level(),2);XCTAssertEqual(session->player()->upgradeStep,1);
    XCTAssertFalse(session->world().paused); // GameMain.setScreen_Game0x3a1c final setPause0.
}
- (void)testMenuButtonsChangeHUDImmediatelyWithoutDrawing {
    auto *view=[self newFixture];[view startSlot:0 mode:0 mission:0 stage:0];
    auto& session=storage<std::unique_ptr<heavymach::GameSession>>(view,"_session");XCTAssertTrue(bool(session));if(!session)return;
    HMGameHUD *hud=[view valueForKey:@"hud"];
    [view gameButton:100 state:2];XCTAssertTrue(session->world().paused);
    XCTAssertEqual([[hud valueForKey:@"menuStep"] intValue],1);
    [self oneTick:view];
    [view gameButton:101 state:2];XCTAssertFalse(session->world().paused);
    XCTAssertEqual([[hud valueForKey:@"menuStep"] intValue],2);
    [view gameButton:100 state:2];[view gameButton:102 state:2];
    XCTAssertFalse(session->world().paused);XCTAssertEqual([[hud valueForKey:@"menuStep"] intValue],2);
}
- (void)testSlotPreviewsRefreshRetainedHUDWithoutSelectingTheirOwnSlot {
    auto *view=[self newFixture];
    HMGameHUD *hud=[view valueForKey:@"hud"];
    auto& save=storage<heavymach::SaveData>(view,"_save");
    save.slots[0].setInt(0x4c,7);save.slots[1].setInt(0x4c,13);
    [view setValue:@10 forKey:@"time"];[hud setValue:@2 forKey:@"weaponTime"];
    [view refreshScreen:3];
    XCTAssertEqual([[hud valueForKey:@"weaponTime"] floatValue],2.f);
    XCTAssertEqual(save.slots[0].i32(0x4c),7);
    [view startSlot:0 mode:0 mission:0 stage:0];
    save.slots[0].setInt(0x4c,7);
    [view setValue:@11 forKey:@"time"];[view refreshScreen:3];
    XCTAssertEqual([[hud valueForKey:@"weaponTime"] floatValue],11.f);
    XCTAssertEqual(save.slots[0].i32(0x4c),99);
    XCTAssertEqual(save.slots[1].i32(0x4c),13);
    XCTAssertEqual([[view valueForKey:@"ammoPreview"] intValue],99);
    [view setApplicationActive:NO];
}
- (void)testHUDPersistsAndTicksWhileHiddenAcrossSlotEntry {
    auto *view=[self newFixture];[self oneTick:view];
    // GuiGame.init0x151a0 starts both HP values at zero before any player exists.
    XCTAssertEqual([[[view valueForKey:@"hud"] valueForKey:@"hp"] floatValue],0.f);
    [view startSlot:0 mode:0 mission:0 stage:0];
    HMGameHUD *hud=[view valueForKey:@"hud"];
    [hud setValue:@.5 forKey:@"hp"];[hud setHPGauge:1];
    [hud setValue:@2 forKey:@"gear"];[hud setReloadGauge:0];
    [view refreshScreen:3];[self oneTick:view];
    XCTAssertEqual([[hud valueForKey:@"hp"] floatValue],.5625f);
    XCTAssertEqual([[hud valueForKey:@"gear"] floatValue],2.15f);
    [view startSlot:1 mode:0 mission:0 stage:0];
    XCTAssertEqual([view valueForKey:@"hud"],hud);
    XCTAssertEqual([[[view valueForKey:@"hud"] valueForKey:@"hp"] floatValue],.5625f);
    XCTAssertEqual([[[view valueForKey:@"hud"] valueForKey:@"gear"] floatValue],2.15f);
    [view refreshScreen:1];[self oneTick:view];
    XCTAssertEqual([[hud valueForKey:@"hp"] floatValue],.6171875f);
    [view startSlot:1 mode:0 mission:0 stage:0];
    XCTAssertEqual([view valueForKey:@"hud"],hud);
    XCTAssertEqual([[hud valueForKey:@"hp"] floatValue],.6171875f);
}
@end
@implementation HMPassiveTestView
- (void)persistSave {}
- (void)consumeGameEvents {auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self,"_session");if(session)session->takeEvents();}
@end
@interface HMPassiveLifecycleTests : XCTestCase
@end
@implementation HMPassiveLifecycleTests
- (void)testActualViewCarriesEffectsThroughMenusSlotsAndTitle {
    HMPassiveTestView *view=[[HMPassiveTestView alloc] initWithFrame:CGRectMake(0,0,480,320)];
    [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
    [(CMMotionManager*)[view valueForKey:@"motion"] stopAccelerometerUpdates];
    [view setValue:nil forKey:@"loadError"];
    auto& save=storage<heavymach::SaveData>(view,"_save");save={};
    for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);
    auto& shared=storage<heavymach::PlayerPassives>(view,"_passives");
    auto& session=storage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
    [view startSlot:0 mode:0 mission:0 stage:0];XCTAssertTrue(bool(session));if(!session)return;
    XCTAssertTrue(&session->player()->passives==&shared);
    PassiveHost host;for(auto name:{"Defense","Attack","Booster","Speed"})shared.add(name,host);
    HMPassiveTestFrame *frame=[HMPassiveTestFrame new];frame.timestamp=1;
    auto tick=[&]{[view frame:(CADisplayLink*)frame];};
    float expected=60,dt=std::bit_cast<float>(uint32_t(0x3c888889));
    session->setPaused(true);tick();expected-=dt;
    for(const auto& effect:shared.slots)XCTAssertEqual(effect.time,expected);
    [view refreshScreen:3];XCTAssertFalse(bool(session));
    tick();expected-=dt;tick();expected-=dt;
    for(const auto& effect:shared.slots)XCTAssertEqual(effect.time,expected);
    [view startSlot:1 mode:0 mission:0 stage:0];XCTAssertTrue(bool(session));if(!session)return;
    XCTAssertTrue(&session->player()->passives==&shared);
    for(const auto& effect:session->player()->passives.slots){XCTAssertTrue(effect.active);XCTAssertEqual(effect.time,expected);}
    [view refreshScreen:1];XCTAssertFalse(bool(session));tick();expected-=dt;
    for(const auto& effect:shared.slots)XCTAssertEqual(effect.time,expected);
    // Modern scene suspension does not simulate elapsed background time.
    [view setApplicationActive:NO];tick();
    for(const auto& effect:shared.slots)XCTAssertEqual(effect.time,expected);
    [view setApplicationActive:YES];
    for(auto& effect:shared.slots){effect.time=.001f;effect.number=0;}
    tick();for(const auto& effect:shared.slots){XCTAssertFalse(effect.active);XCTAssertEqual(effect.time,0);}
    XCTAssertNil([view valueForKey:@"loadError"]);
    [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
}
@end
