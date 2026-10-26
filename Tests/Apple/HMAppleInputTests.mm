#import <XCTest/XCTest.h>
#import <objc/runtime.h>
#import "HMGameView.h"
#import "HMGLRenderer.h"
#import "HMMedia.h"
#import "HMOpenALAudio.h"
#import "HMMenus.h"
#import "HMValidationControls.h"
#import "HMValidationPanel.h"
#include "GameSession.hpp"
#include "KeyboardInput.hpp"
#include "Viewport.hpp"
#include "TiltState.hpp"
#include "TitleState.hpp"
#include "LoadingTransition.hpp"
#include <algorithm>
#include <chrono>
#include <vector>
@interface HMGameView (AppleInputTests)
- (void)frame:(id)sender;
- (BOOL)handleKeyCode:(unsigned)key pressed:(BOOL)down;
- (void)sampleOriginalAcceleration;
- (void)advanceOriginalFrame;
- (void)drawLogicalFrame;
- (void)drawKeyboardIndicators;
- (void)updateCanvasGeometry;
- (void)refreshScreen:(int)screen;
- (void)startSlot:(int)slot mode:(int)mode mission:(int)mission stage:(int)stage;
- (void)routePoint:(CGPoint)point state:(int)state token:(id)token;
- (void)keyboardDisconnected:(NSNotification*)notification;
@end
#if TARGET_OS_OSX
@interface HMValidationPanel (InputTests)
- (void)start:(id)sender;
@end
#endif
@interface HMInputTestView:HMGameView
@property NSUInteger gameSaveCalls;
@property NSUInteger keySaveCalls;
@end
@implementation HMInputTestView
- (void)persistSave {self.gameSaveCalls++;}
- (void)saveKeyboardBindings {self.keySaveCalls++;}
@end
namespace {
template<class T>T& storage(id object,const char* name){Ivar v=class_getInstanceVariable(object_getClass(object),name);if(!v)throw std::runtime_error(name);return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>((__bridge void*)object)+ivar_getOffset(v));}
void key(HMGameView* view,unsigned code){[view handleKeyCode:code pressed:YES];[view handleKeyCode:code pressed:NO];}
void render(HMGameView* view){HMGLRenderer* gl=[view valueForKey:@"glRenderer"];HMMedia* media=[view valueForKey:@"media"];[gl setLogicalWidth:[[view valueForKey:@"canvasWidth"]doubleValue] height:[[view valueForKey:@"canvasHeight"]doubleValue]];if([gl beginFrame]){media.glRenderer=gl;[view drawLogicalFrame];[view drawKeyboardIndicators];media.glRenderer=nil;[gl endFrame];}}
void frames(HMGameView* view,int count){for(int i=0;i<count;++i){[view sampleOriginalAcceleration];[view advanceOriginalFrame];render(view);}}
}
#if TARGET_OS_OSX
namespace {
NSDictionary* macFrameMetrics(const std::vector<double>& values) {
 if(values.empty())return @{@"samples":@0};
 auto sorted=values;std::sort(sorted.begin(),sorted.end());double sum=0;for(double value:values)sum+=value;
 return @{@"samples":@(values.size()),@"mean_ms":@(sum/values.size()),@"p95_ms":@(sorted[(sorted.size()*95+99)/100-1]),@"max_ms":@(sorted.back())};
}
}
@interface HMMacTimedInputView:HMInputTestView {
@public std::vector<double> work,intervals;double previous;NSUInteger count,maxBullets;
}
@property XCTestExpectation* finished;
@property BOOL firing;
@end
@implementation HMMacTimedInputView
- (void)frame:(id)sender {
 if(!self.finished){[super frame:sender];return;}
 if(self.firing){auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self,"_session");if(session){session->touch({470,160},1);maxBullets=std::max(maxBullets,session->bulletCount());}}
 auto clock=[] {return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();};
 double begin=clock();[super frame:sender];double elapsed=(clock()-begin)*1000;
 if(count>=30){work.push_back(elapsed);if(previous)intervals.push_back((begin-previous)*1000);}
 previous=begin;
 if(++count==210){[self setApplicationActive:NO];[self.finished fulfill];self.finished=nil;}
}
@end
#endif
@interface HMAppleInputTests:XCTestCase
@property HMGameView* game;
@property NSURL* temp;
@end
@implementation HMAppleInputTests
- (void)setUp {
 [super setUp];self.game=[[HMInputTestView alloc]initWithFrame:CGRectMake(0,0,1024,768)];
 [[self.game valueForKey:@"frameTimer"]invalidate];[[self.game valueForKey:@"motionTimer"]invalidate];
 self.temp=[NSURL fileURLWithPath:[NSTemporaryDirectory() stringByAppendingPathComponent:NSUUID.UUID.UUIDString] isDirectory:YES];[NSFileManager.defaultManager createDirectoryAtURL:self.temp withIntermediateDirectories:YES attributes:nil error:nil];
 [self.game setValue:[self.temp URLByAppendingPathComponent:@"HvM.dat"] forKey:@"saveURL"];
 storage<heavymach::SaveData>(self.game,"_save")={};storage<heavymach::KeyboardInput>(self.game,"_keyboardControls")={};
 [self.game setValue:@1 forKey:@"randomState"];[self.game updateCanvasGeometry];
}
- (void)tearDown {[self.game setApplicationActive:NO];self.game=nil;[NSFileManager.defaultManager removeItemAtURL:self.temp error:nil];[super tearDown];}
- (void)startFixture {
 heavymach::ValidationScenario fixture;fixture.allWeapons=true;fixture.internalLevel=4;fixture.grades={4,4,4};fixture.playerPosition=heavymach::Vector2{250,100};
 storage<std::optional<heavymach::ValidationScenario>>(self.game,"_validationFixture")=fixture;
 [self.game startSlot:0 mode:0 mission:0 stage:0];frames(self.game,100);
 XCTAssertNil([self.game valueForKey:@"loadError"]);
}
#if TARGET_OS_OSX
- (void)testMacPresentedFrameCostsWithProductionTimer {
 HMGameView* original=[(NSObject*)NSApp.delegate valueForKey:@"game"];
 [original setApplicationActive:NO];
 NSWindow* window=[[NSWindow alloc]initWithContentRect:NSMakeRect(0,0,1200,800) styleMask:NSWindowStyleMaskBorderless backing:NSBackingStoreBuffered defer:NO];
 window.releasedWhenClosed=NO;
 @try {
 for(NSString* size in @[@"1200x800",@"screen_size"]){
  NSRect rectangle=[size isEqualToString:@"screen_size"]?NSScreen.mainScreen.frame:NSMakeRect(50,50,1200,800);
  [window setFrame:rectangle display:YES];
  for(NSString* scene in @[@"title_animation",@"slot_preview",@"stage01_firing"]){@autoreleasepool{
   HMMacTimedInputView* view=[[HMMacTimedInputView alloc]initWithFrame:window.contentView.bounds];
   [view setApplicationActive:NO];window.contentView=view;[window makeKeyAndOrderFront:nil];[NSApp activateIgnoringOtherApps:YES];
   auto& save=storage<heavymach::SaveData>(view,"_save");save={};for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);
   [view setValue:@11 forKey:@"randomState"];
   storage<heavymach::KeyboardInput>(view,"_keyboardControls")={};
   [view setValue:@NO forKey:@"resumeRequired"];storage<heavymach::LoadingTransition>(view,"_loading")={};
   if([scene isEqualToString:@"slot_preview"])[view refreshScreen:3];
   if([scene isEqualToString:@"stage01_firing"])[view startSlot:0 mode:0 mission:0 stage:0];
   view.firing=[scene isEqualToString:@"stage01_firing"];
   view.finished=[self expectationWithDescription:[size stringByAppendingString:scene]];
   XCTestExpectation* done=view.finished;[view setApplicationActive:YES];
   [self waitForExpectations:@[done] timeout:30];[view setApplicationActive:NO];
   HMGLRenderer* gl=[view valueForKey:@"glRenderer"];
   NSDictionary* record=@{@"phase":@"native_mac_production_timer",@"size":size,@"scene":scene,@"view_width":@(view.bounds.size.width),@"view_height":@(view.bounds.size.height),@"scale":@(window.backingScaleFactor),@"frame_work":macFrameMetrics(view->work),@"callback_intervals":macFrameMetrics(view->intervals),@"renderer":[gl diagnostics],@"audio":[[view valueForKey:@"media"] audio].diagnostics,@"max_bullets":@(view->maxBullets),@"game_save_calls":@(view.gameSaveCalls)};
   NSData* json=[NSJSONSerialization dataWithJSONObject:record options:NSJSONWritingSortedKeys error:nil];NSLog(@"HM_MAC_FRAME_PERF %@",[[NSString alloc]initWithData:json encoding:NSUTF8StringEncoding]);
   XCTAssertEqual(view->work.size(),size_t(180));XCTAssertEqualObjects([gl diagnostics][@"lastError"],@"");XCTAssertNil([view valueForKey:@"loadError"]);
   if(view.firing)XCTAssertGreaterThan(view->maxBullets,NSUInteger(0));
   XCTAssertEqual(view.gameSaveCalls,NSUInteger(0));
   XCTAssertLessThan([macFrameMetrics(view->work)[@"p95_ms"]doubleValue],1000.0/60.0,@"Native Mac frame work budget");
   window.contentView=nil;
  }}
 }
 } @finally {[window close];[original.window makeKeyAndOrderFront:nil];[original setApplicationActive:YES];}
}
- (void)testValidationPanelStartsWithChosenWeaponAtEveryGrade {
 auto& data=storage<heavymach::GameData>(self.game,"_data");
 HMValidationPanel* panel=[[HMValidationPanel alloc]initWithData:&data];
 NSPopUpButton* weapon=[panel valueForKey:@"weapon"];
 NSButton* all=[panel valueForKey:@"all"];
 auto gradeIvar=class_getInstanceVariable(panel.class,"_grades");
 auto grades=reinterpret_cast<NSTextField* __strong *>(reinterpret_cast<uint8_t*>((__bridge void*)panel)+ivar_getOffset(gradeIvar));
 __block std::optional<heavymach::ValidationScenario> received;
 panel.applyFixture=^(const heavymach::ValidationScenario& scenario){received=scenario;};
 for(int grade=0;grade<=6;++grade)for(bool allWeapons:{false,true}){
  for(int part=0;part<3;++part)grades[part].integerValue=grade;
  all.state=allWeapons?NSControlStateValueOn:NSControlStateValueOff;
  for(NSString* name in weapon.itemTitles){
   [weapon selectItemWithTitle:name];received.reset();[panel start:nil];
   XCTAssertTrue(received.has_value());if(!received)continue;
   heavymach::SaveSlot slot;heavymach::prepareValidationSlot(slot,data,*received);
   XCTAssertGreaterThanOrEqual(received->selectedWeapon,0);
   XCTAssertLessThan(received->selectedWeapon,slot.weaponCount());
   XCTAssertEqualObjects([NSString stringWithUTF8String:slot.weaponName(received->selectedWeapon).c_str()],name);
  }
 }
}
#endif
- (void)testFullscreenRatiosAndTouchInverse {
 for(CGSize size:{CGSizeMake(1024,768),CGSizeMake(1366,1024),CGSizeMake(1440,900),CGSizeMake(912,420),CGSizeMake(800,900)}){
  self.game.frame=CGRectMake(0,0,size.width,size.height);[self.game updateCanvasGeometry];auto& vp=storage<heavymach::Viewport>(self.game,"_viewport");
  XCTAssertEqualWithAccuracy(vp.x,0,.6);XCTAssertEqualWithAccuracy(vp.y,0,.6);
  XCTAssertEqualWithAccuracy(vp.logicalWidth*vp.scale,size.width,.6);XCTAssertEqualWithAccuracy(vp.logicalHeight*vp.verticalScale(),size.height,.6);
  auto top=vp.logical(1,1),bottom=vp.logical(size.width-1,size.height-1);XCTAssertTrue(top.has_value());XCTAssertTrue(bottom.has_value());
  render(self.game);HMGLRenderer* gl=[self.game valueForKey:@"glRenderer"];XCTAssertNotNil([gl snapshot]);
 }
}
- (void)testPausedResizeAndUpperCanvasAim {
 [self startFixture];auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");
 key(self.game,0x29);double time=session->world().gameTime;float x=session->player()->position.x;
 self.game.frame=CGRectMake(0,0,1400,800);[self.game updateCanvasGeometry];render(self.game);
 auto& vp=storage<heavymach::Viewport>(self.game,"_viewport");XCTAssertEqualWithAccuracy(vp.logicalWidth*vp.scale,1400,.6);XCTAssertEqualWithAccuracy(vp.logicalHeight*vp.verticalScale(),800,.6);
 XCTAssertEqual(session->world().gameTime,time);XCTAssertEqual(session->player()->position.x,x);
 self.game.frame=CGRectMake(0,0,1024,768);[self.game updateCanvasGeometry];key(self.game,0x29);frames(self.game,1);
 NSObject* token=[NSObject new];CGPoint point=CGPointMake(vp.x+470*vp.scale,vp.y+2*vp.verticalScale());[self.game routePoint:point state:0 token:token];
 auto origin=session->validationAimOrigin();auto player=session->player()->position;auto target=session->validationWeaponState().fireTarget;
 XCTAssertEqualWithAccuracy(target.x,player.x+470-origin.x,.01);XCTAssertEqualWithAccuracy(target.y,player.y+358-origin.y,.01);
 [self.game routePoint:point state:2 token:token];
}
- (void)testHeightExtensionPreservesAircraftBossMotion {
 auto& data=storage<heavymach::GameData>(self.game,"_data");HMMedia* media=[self.game valueForKey:@"media"];
 auto tiles=[media](std::string_view name){CGSize size=[media imageSize:[NSString stringWithUTF8String:std::string(name).c_str()]];return heavymach::Vector2{float(size.width),float(size.height)};};
 heavymach::SaveSlot a,b;uint32_t ra=1,rb=1;auto random=[](uint32_t& state){state^=state<<13;state^=state>>17;state^=state<<5;return int32_t(state&0x7fffffff);};
 heavymach::GameSession original(data,a,tiles,[&]{return random(ra);}),expanded(data,b,tiles,[&]{return random(rb);});
 heavymach::ValidationScenario fixture;fixture.mission=2;fixture.stage=4;fixture.bossIndex=0;fixture.internalLevel=20;fixture.grades={6,6,6};
 expanded.setCanvasHeight(360);original.startValidation(fixture);expanded.startValidation(fixture);
 for(int frame=0;frame<360;++frame){original.tick((frame+1)/60.,0);expanded.tick((frame+1)/60.,0);auto first=original.validationEnemies(),second=expanded.validationEnemies();XCTAssertEqual(first.size(),second.size());for(size_t i=0;i<std::min(first.size(),second.size());++i){XCTAssertEqualWithAccuracy(first[i].position.x,second[i].position.x,.001);XCTAssertEqualWithAccuracy(first[i].position.y,second[i].position.y,.001);}}
}
- (void)testKeyboardMoveAimJumpFireAndRelease {
 [self startFixture];auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");
 NSUInteger saves=((HMInputTestView*)self.game).gameSaveCalls;
 float start=session->player()->position.x;[self.game handleKeyCode:7 pressed:YES];frames(self.game,80);XCTAssertGreaterThan(session->player()->position.x,start+10);
 [self.game handleKeyCode:7 pressed:NO];XCTAssertEqual(storage<heavymach::TiltState>(self.game,"_tilt").y,0);
 float right=session->player()->position.x;[self.game handleKeyCode:4 pressed:YES];frames(self.game,60);[self.game handleKeyCode:4 pressed:NO];XCTAssertLessThan(session->player()->position.x,right);
 float y=session->player()->position.y;key(self.game,0x2c);frames(self.game,8);XCTAssertGreaterThan(session->player()->position.y,y);
 [self.game handleKeyCode:0x52 pressed:YES];frames(self.game,10);[self.game handleKeyCode:0x52 pressed:NO];CGPoint aim=storage<CGPoint>(self.game,"_keyboardAim");XCTAssertGreaterThan(aim.y,100);
 [self.game handleKeyCode:9 pressed:YES];frames(self.game,40);XCTAssertTrue(storage<BOOL>(self.game,"_keyboardFire"));XCTAssertGreaterThan(session->bulletCount(),size_t(0));
 [self.game keyboardDisconnected:nil];XCTAssertFalse(storage<BOOL>(self.game,"_keyboardFire"));XCTAssertFalse(storage<heavymach::KeyboardInput>(self.game,"_keyboardControls").moving());
 key(self.game,0x29);XCTAssertTrue(session->world().paused);key(self.game,0x29);XCTAssertFalse(session->world().paused);
 XCTAssertEqual(((HMInputTestView*)self.game).gameSaveCalls,saves);
 XCTAssertEqual(((HMInputTestView*)self.game).keySaveCalls,NSUInteger(0));
}
- (void)testEveryWeaponKeyboardSelectionAndFire {
 [self startFixture];auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");auto& data=storage<heavymach::GameData>(self.game,"_data");
 auto names=heavymach::validationWeapons(data);XCTAssertEqual(names.size(),size_t(20));
 for(const auto& name:names){
  heavymach::ValidationScenario fixture;fixture.weapons={name};fixture.grades={4,4,4};fixture.internalLevel=4;
  storage<std::optional<heavymach::ValidationScenario>>(self.game,"_validationFixture")=fixture;[self.game startSlot:0 mode:0 mission:0 stage:0];frames(self.game,110);
  auto& slot=storage<heavymach::SaveData>(self.game,"_save").slots[0];
  key(self.game,0x14);XCTAssertEqual(session->selectedWeapon(),slot.weaponCount()-1);key(self.game,0x08);XCTAssertEqual(session->selectedWeapon(),0);
  int index=-1;for(int i=0;i<slot.weaponCount();++i)if(slot.weaponName(i)==name)index=i;XCTAssertGreaterThanOrEqual(index,0);
  for(int i=0;i<index;++i)key(self.game,0x08);XCTAssertEqual(session->selectedWeapon(),index);
  storage<CGPoint>(self.game,"_keyboardAim")=CGPointMake(450,220);storage<BOOL>(self.game,"_hasKeyboardAim")=YES;
  NSUInteger saves=((HMInputTestView*)self.game).gameSaveCalls;
  int shots=0;session->validationObserveShots([&](const heavymach::WeaponInfo&,float,heavymach::Vector2){++shots;});
  [self.game handleKeyCode:9 pressed:YES];size_t maxBullets=0;for(int n=0;n<240;++n){frames(self.game,1);maxBullets=std::max(maxBullets,session->bulletCount());}
  [self.game handleKeyCode:9 pressed:NO];XCTAssertGreaterThan(shots,0,@"%s",name.c_str());session->validationObserveShots({});XCTAssertNil([self.game valueForKey:@"loadError"]);
  XCTAssertEqual(((HMInputTestView*)self.game).gameSaveCalls,saves,@"%s",name.c_str());XCTAssertEqual(((HMInputTestView*)self.game).keySaveCalls,NSUInteger(0));
  NSLog(@"HM_APPLE_WEAPON %s bullets=%zu playerShots=%d saveCalls=%lu",name.c_str(),maxBullets,shots,(unsigned long)(((HMInputTestView*)self.game).gameSaveCalls-saves));
 }
}
- (void)testKeyboardMenusAndSettings {
 [self.game refreshScreen:3];frames(self.game,10);HMMenus* menus=[self.game valueForKey:@"menus"];
 key(self.game,0x2b);XCTAssertFalse(CGRectIsEmpty([menus keyboardFocusRect]));
 // Navigate by keyboard focus, never activate a hidden button or synthesize a touch.
 for(int i=0;i<12&&!storage<heavymach::LoadingTransition>(self.game,"_loading").visible();++i){
  if(storage<int>(menus,"_keyboardFocus")==1000)key(self.game,0x28);else key(self.game,0x2b);frames(self.game,1);
 }
 XCTAssertTrue(storage<heavymach::LoadingTransition>(self.game,"_loading").visible());frames(self.game,100);
 XCTAssertTrue(storage<BOOL>(self.game,"_showMenu"));
 for(int screen=0;screen<2;++screen){
  for(int i=0;i<40&&!storage<heavymach::LoadingTransition>(self.game,"_loading").visible();++i){if(storage<int>(menus,"_keyboardFocus")==1000)key(self.game,0x28);else key(self.game,0x2b);frames(self.game,1);}
  XCTAssertTrue(storage<heavymach::LoadingTransition>(self.game,"_loading").visible());frames(self.game,120);
 }
 XCTAssertNotEqual(storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session").get(),nullptr);
 key(self.game,0x3b);XCTAssertTrue(storage<BOOL>(self.game,"_keyboardSettingsOpen"));
 key(self.game,0x28);key(self.game,0x1d);XCTAssertEqual(storage<heavymach::KeyboardInput>(self.game,"_keyboardControls").bindings[0],0x1du);
 key(self.game,0x2a);XCTAssertEqual(storage<heavymach::KeyboardInput>(self.game,"_keyboardControls").bindings[0],4u);
 key(self.game,0x29);XCTAssertFalse(storage<BOOL>(self.game,"_keyboardSettingsOpen"));
}
- (void)testTouchFireWhileKeyboardMoves {
 [self startFixture];auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");auto& vp=storage<heavymach::Viewport>(self.game,"_viewport");
 NSObject* touch=[NSObject new];CGPoint point=CGPointMake(vp.x+470*vp.scale,vp.y+(vp.logicalHeight-220)*vp.verticalScale());
 [self.game handleKeyCode:7 pressed:YES];[self.game routePoint:point state:0 token:touch];frames(self.game,40);
 XCTAssertGreaterThan(session->bulletCount(),size_t(0));XCTAssertTrue(storage<heavymach::KeyboardInput>(self.game,"_keyboardControls").moving());
 [self.game routePoint:point state:2 token:touch];[self.game handleKeyCode:7 pressed:NO];
 XCTAssertFalse(storage<heavymach::KeyboardInput>(self.game,"_keyboardControls").moving());
}
@end
