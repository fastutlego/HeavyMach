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
#include "VTOL.hpp"
#include "HMAimOverlay.hpp"
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
- (void)setInputAimMode:(heavymach::AimMode)mode;
- (CGRect)aimControlRect:(int)control;
@end
#if TARGET_OS_OSX
@interface HMValidationPanel (InputTests)
- (void)start:(id)sender;
@end
#endif
@interface HMInputTestView:HMGameView
@property NSUInteger gameSaveCalls;
@property NSUInteger keySaveCalls;
@property NSUInteger aimSaveCalls;
@end
@implementation HMInputTestView
- (void)saveAimMode {self.aimSaveCalls++;}
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
 storage<heavymach::AimMode>(self.game,"_preferredAimMode")=heavymach::AimMode::manual;
 [self.game setValue:@1 forKey:@"randomState"];[self.game updateCanvasGeometry];
}
- (void)tearDown {[self.game setApplicationActive:NO];self.game=nil;[NSFileManager.defaultManager removeItemAtURL:self.temp error:nil];[super tearDown];}
- (void)testVTOLExpansionRendersAndUsesSeparateTextures {
 // Reproducible neutral movement; a connected physical device may be tilted.
 ((HMValidationControls*)[self.game valueForKey:@"validationControls"]).selectedSegmentIndex=1;
#if !TARGET_OS_OSX
 CGSize screen=UIScreen.mainScreen.bounds.size;self.game.frame=CGRectMake(0,0,std::max(screen.width,screen.height),std::min(screen.width,screen.height));[self.game updateCanvasGeometry];
#endif
 auto& data=storage<heavymach::GameData>(self.game,"_data");XCTAssertNil([self.game valueForKey:@"loadError"]);
 HMMedia* media=[self.game valueForKey:@"media"];HMGLRenderer* gl=[self.game valueForKey:@"glRenderer"];
 int index=0;const int missions[]={1,2,5},stages[]={1,2,1};const char* maps[]={"Stage06","Stage12","Stage21"};
 for(const auto& spec:heavymach::vtolSpecs()){
  auto& map=data.maps.at(maps[index]);auto enemy=std::find_if(map.enemies.begin(),map.enemies.end(),[&](const auto& e){return e.name==spec.name;});XCTAssertTrue(enemy!=map.enemies.end());if(enemy==map.enemies.end())return;
  heavymach::ValidationScenario fixture;fixture.mission=missions[index];fixture.stage=stages[index];fixture.allWeapons=true;fixture.internalLevel=50;fixture.grades={6,6,6};fixture.playerPosition=heavymach::Vector2{enemy->position.x-130,enemy->position.y-100};
  storage<std::optional<heavymach::ValidationScenario>>(self.game,"_validationFixture")=fixture;
  [self.game startSlot:0 mode:0 mission:fixture.mission stage:fixture.stage];
  auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");XCTAssertTrue(bool(session));if(!session)return;session->validationSetCheats(true);
  bool seen=false;size_t maxBullets=0;std::vector<double> costs;NSUInteger before=((HMInputTestView*)self.game).gameSaveCalls;
  for(int n=0;n<180;++n){auto begin=std::chrono::steady_clock::now();frames(self.game,1);if(n>=30)costs.push_back(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count());maxBullets=std::max(maxBullets,session->bulletCount());for(auto& sprite:session->sprites())if(sprite.tile==spec.tile){seen=true;XCTAssertTrue(std::isfinite(sprite.rotation));}}
  XCTAssertTrue(seen,@"%s",spec.name);CGSize size=[media imageSize:[NSString stringWithUTF8String:spec.tile]];XCTAssertEqual(size.width,spec.width);XCTAssertEqualWithAccuracy(size.height,spec.width*spec.aspect,.51);
  XCTAssertEqual(((HMInputTestView*)self.game).gameSaveCalls,before);
#if !TARGET_OS_OSX
  UIImage* picture=[gl snapshot];XCTAssertNotNil(picture);if(picture){XCTAttachment* a=[XCTAttachment attachmentWithImage:picture];a.name=[NSString stringWithUTF8String:spec.name];a.lifetime=XCTAttachmentLifetimeKeepAlways;[self addAttachment:a];}
#endif
  std::sort(costs.begin(),costs.end());double sum=0;for(double c:costs)sum+=c;
  NSLog(@"HM_VTOL %s texture=%gx%g seen=%d bullets=%zu render_mean_ms=%.3f render_p95_ms=%.3f",spec.name,size.width,size.height,seen,maxBullets,sum/costs.size(),costs[costs.size()*95/100]);++index;
 }
}
- (void)testVTOLSizeAndEdgeComparison {
 // Same GPU path and scale as gameplay. Rows: Heli001, Heli006, Yak, Harrier, F35B.
 // Columns show level flight and pitched flight without other enemies hiding the body.
 HMMedia* media=[HMMedia new];HMGLRenderer* gl=[HMGLRenderer new];XCTAssertNotNil(gl);
 media.glRenderer=gl;XCTAssertTrue([gl beginFrame]);
 [media draw:@"BG_City01.png" x:240 y:160];
 NSArray<NSString*>* names=@[@"E_Heli_Body_01.png",@"E_Heli_Body_06.png",@"Extensions/VTOL_Yak141.png",@"Extensions/VTOL_Harrier.png",@"Extensions/VTOL_F35B.png"];
 for(NSUInteger row=0;row<names.count;++row){
  CGSize size=[media imageSize:names[row]];CGFloat height=size.height;
  if(row>=2){const auto& spec=heavymach::vtolSpecs()[row-2];height=spec.width*spec.aspect;XCTAssertGreaterThan(size.width,[media imageSize:names[0]].width);XCTAssertLessThan(size.width,[media imageSize:names[1]].width);}
  for(int col=0;col<2;++col)[media draw:names[row] x:150+col*180 y:280-row*55 columns:1 rows:1 tile:0 scaleX:1 scaleY:height/size.height rotation:col*.14 alpha:1];
 }
 [gl finish];[gl endFrame];media.glRenderer=nil;XCTAssertEqualObjects([gl diagnostics][@"lastError"],@"");
#if !TARGET_OS_OSX
 UIImage* picture=[gl snapshot];XCTAssertNotNil(picture);if(picture){XCTAttachment* a=[XCTAttachment attachmentWithImage:picture];a.name=@"VTOL_Size_Edges";a.lifetime=XCTAttachmentLifetimeKeepAlways;[self addAttachment:a];}
#endif
}
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
  for(NSString* scene in @[@"title_animation",@"slot_preview",@"stage01_firing"]){
  __weak HMGameView* releasedView;__weak HMMedia* releasedMedia;__weak HMGLRenderer* releasedRenderer;__weak HMOpenALAudio* releasedAudio;
  @autoreleasepool{
   HMMacTimedInputView* view=[[HMMacTimedInputView alloc]initWithFrame:window.contentView.bounds];
   releasedView=view;releasedMedia=[view valueForKey:@"media"];releasedRenderer=[view valueForKey:@"glRenderer"];releasedAudio=releasedMedia.audio;
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
  }
  XCTAssertNil(releasedView,@"Game view must be released after scene teardown");
  XCTAssertNil(releasedMedia,@"Media must be released after scene teardown");
  XCTAssertNil(releasedRenderer,@"GL renderer must be released after scene teardown");
  XCTAssertNil(releasedAudio,@"OpenAL owner must be released after scene teardown");
  NSLog(@"HM_MAC_TEARDOWN %@ %@ view=%d media=%d renderer=%d audio=%d",size,scene,releasedView==nil,releasedMedia==nil,releasedRenderer==nil,releasedAudio==nil);
  }
 }
 } @finally {[window close];[original.window makeKeyAndOrderFront:nil];[original setApplicationActive:YES];}
}
- (void)testMacNativeFullscreenUsesEntireDrawableAndRestoresWindow {
 NSDictionary* desktop=CFBridgingRelease(CGSessionCopyCurrentDictionary());
 XCTSkipIf([desktop[@"CGSSessionScreenIsLocked"] boolValue],@"macOS desktop is locked; native fullscreen transition requires an unlocked session");
 NSWindow* window=[(NSObject*)NSApp.delegate valueForKey:@"window"];
 HMGameView* view=[(NSObject*)NSApp.delegate valueForKey:@"game"];
 XCTAssertNotNil(window);XCTAssertNotNil(view);if(!window||!view)return;
 NSRect initial=window.frame;
 BOOL wasFull=(window.styleMask&NSWindowStyleMaskFullScreen)!=0;
 @try {
  if(!wasFull){
   XCTestExpectation* entered=[self expectationForNotification:NSWindowDidEnterFullScreenNotification object:window handler:nil];
   [window toggleFullScreen:nil];[self waitForExpectations:@[entered] timeout:15];
  }
  XCTAssertTrue((window.styleMask&NSWindowStyleMaskFullScreen)!=0);
  [window.contentView layoutSubtreeIfNeeded];[view setApplicationActive:YES];[view frame:nil];
  NSRect screen=window.screen.frame,content=window.contentView.bounds,backing=[view convertRectToBacking:view.bounds];
  XCTAssertEqualWithAccuracy(window.frame.size.width,screen.size.width,1);
  XCTAssertEqualWithAccuracy(window.frame.size.height,screen.size.height,1);
  XCTAssertEqualWithAccuracy(content.size.width,view.bounds.size.width,.01);
  XCTAssertEqualWithAccuracy(content.size.height,view.bounds.size.height,.01);
  auto& viewport=storage<heavymach::Viewport>(view,"_viewport");
  XCTAssertEqualWithAccuracy(viewport.x,0,.01);XCTAssertEqualWithAccuracy(viewport.y,0,.01);
  XCTAssertEqualWithAccuracy(viewport.logicalWidth*viewport.scale,content.size.width,.01);
  XCTAssertEqualWithAccuracy(viewport.logicalHeight*viewport.verticalScale(),content.size.height,.01);
  HMGLRenderer* gl=[view valueForKey:@"glRenderer"];
  XCTAssertEqualWithAccuracy([[gl valueForKey:@"displayWidth"]doubleValue],backing.size.width,1);
  XCTAssertEqualWithAccuracy([[gl valueForKey:@"displayHeight"]doubleValue],backing.size.height,1);
  XCTAssertEqualObjects([gl diagnostics][@"lastError"],@"");
  NSLog(@"HM_MAC_FULLSCREEN window=%@ content=%@ view=%@ backing=%@ viewport=(%.3f,%.3f,%.3f,%.3f) renderer=%@",NSStringFromRect(window.frame),NSStringFromRect(content),NSStringFromRect(view.bounds),NSStringFromRect(backing),viewport.x,viewport.y,viewport.logicalWidth*viewport.scale,viewport.logicalHeight*viewport.verticalScale(),[gl diagnostics][@"renderer"]);
 } @finally {
  [view setApplicationActive:NO];
  if(!wasFull&&(window.styleMask&NSWindowStyleMaskFullScreen)){
   XCTestExpectation* exited=[self expectationForNotification:NSWindowDidExitFullScreenNotification object:window handler:nil];
   [window toggleFullScreen:nil];[self waitForExpectations:@[exited] timeout:15];
   XCTAssertEqualWithAccuracy(window.frame.size.width,initial.size.width,1);
   XCTAssertEqualWithAccuracy(window.frame.size.height,initial.size.height,1);
  }
 }
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
- (void)autoAimArenaWithWeapon:(const std::string&)name {
 // Test-only copy of Stage01: flat ground, one real helicopter AI added after
 // the intro. Increased HP permits measuring repeated impacts, not forced hits.
 auto& data=storage<heavymach::GameData>(self.game,"_data");auto& map=data.maps.at("Stage01");
 map.enemies.clear();map.objects.clear();map.collisionPaths={{{0,40},{10000,40}}};
 data.machines.at("Heli001").hpMax=100000;
 heavymach::ValidationScenario fixture;fixture.weapons={name};fixture.grades={4,4,4};fixture.internalLevel=4;fixture.playerPosition=heavymach::Vector2{350,100};
 storage<std::optional<heavymach::ValidationScenario>>(self.game,"_validationFixture")=fixture;
 ((HMValidationControls*)[self.game valueForKey:@"validationControls"]).selectedSegmentIndex=1;
 storage<heavymach::AimMode>(self.game,"_preferredAimMode")=heavymach::AimMode::manual;
 [self.game startSlot:0 mode:0 mission:0 stage:0];frames(self.game,110);
 auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");
 auto& slot=storage<heavymach::SaveData>(self.game,"_save").slots[0];int index=-1;
 for(int n=0;n<slot.weaponCount();++n)if(slot.weaponName(n)==name)index=n;
 XCTAssertGreaterThanOrEqual(index,0);for(int n=0;n<index;++n)key(self.game,0x08);
 auto p=session->player()->position;session->validationSpawnEnemy({"Heli001",{p.x+240,p.y+140},false});
 [self.game setInputAimMode:heavymach::AimMode::automatic];frames(self.game,2);
}
- (void)testAutoAimEveryWeaponWithMovementAndJump {
 auto& data=storage<heavymach::GameData>(self.game,"_data");auto names=heavymach::validationWeapons(data);XCTAssertEqual(names.size(),size_t(20));
 int index=0;
 for(const auto& name:names){
  [self autoAimArenaWithWeapon:name];auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");
  auto target=session->aimSnapshot().target;XCTAssertNotEqual(target,uintptr_t(0),@"%s",name.c_str());
  NSUInteger saves=((HMInputTestView*)self.game).gameSaveCalls,aimSaves=((HMInputTestView*)self.game).aimSaveCalls;
  float x=session->player()->position.x,y=session->player()->position.y,maxY=y;bool moved=false;
  int shots=0;session->validationObserveShots([&](const heavymach::WeaponInfo&,float,heavymach::Vector2){++shots;});
  NSObject* fireTouch=[NSObject new];auto r=[self.game aimControlRect:2];auto vp=storage<heavymach::Viewport>(self.game,"_viewport");
  CGPoint firePoint=CGPointMake(vp.x+CGRectGetMidX(r)*vp.scale,vp.y+(vp.logicalHeight-CGRectGetMidY(r))*vp.verticalScale());
  if(index%2)[self.game handleKeyCode:9 pressed:YES];else [self.game routePoint:firePoint state:0 token:fireTouch];
  [self.game handleKeyCode:4 pressed:YES];key(self.game,0x2c);
  for(int frame=0;frame<180;++frame){
   if(frame==25){[self.game handleKeyCode:4 pressed:NO];[self.game handleKeyCode:7 pressed:YES];}
   if(frame==50)[self.game handleKeyCode:7 pressed:NO];
   frames(self.game,1);maxY=std::max(maxY,session->player()->position.y);moved=moved||std::fabs(session->player()->position.x-x)>5;
  }
  if(index%2)[self.game handleKeyCode:9 pressed:NO];else [self.game routePoint:firePoint state:2 token:fireTouch];
  session->validationObserveShots({});auto aim=session->aimSnapshot();
  XCTAssertGreaterThan(shots,0,@"%s",name.c_str());XCTAssertGreaterThan(aim.targetHits,uint64_t(0),@"%s",name.c_str());
  bool damaged=false;for(const auto& enemy:session->validationEnemies())if(enemy.id==target)damaged=enemy.hp<enemy.hpMax;
  XCTAssertTrue(damaged,@"%s",name.c_str());XCTAssertTrue(moved,@"%s",name.c_str());XCTAssertGreaterThan(maxY,y+5,@"%s",name.c_str());
  XCTAssertFalse(session->validationWeaponState().isFire);XCTAssertEqual(((HMInputTestView*)self.game).gameSaveCalls,saves);XCTAssertEqual(((HMInputTestView*)self.game).aimSaveCalls,aimSaves);
  NSLog(@"HM_AUTO_AIM %s shots=%d assisted=%llu hits=%llu jump=%.2f moved=%d saveWrites=%lu",name.c_str(),shots,aim.assistedShots,aim.targetHits,maxY-y,moved,(unsigned long)(((HMInputTestView*)self.game).gameSaveCalls-saves));
#if !TARGET_OS_OSX
  if(index==0||name=="P_Wpn_Canon03"||name=="P_Wpn_RaderLsr"){
   HMGLRenderer* gl=[self.game valueForKey:@"glRenderer"];UIImage* picture=[gl snapshot];if(picture){XCTAttachment* a=[XCTAttachment attachmentWithImage:picture];a.name=[@"AutoAim_" stringByAppendingString:[NSString stringWithUTF8String:name.c_str()]];a.lifetime=XCTAttachmentLifetimeKeepAlways;[self addAttachment:a];}
  }
#endif
  ++index;
 }
}
- (void)testAutoAimLabelStaysInsideCanvas {
 for(float width:{480.f,700.f})for(float height:{320.f,360.f})for(float x:{-20.f,200.f,width-10})for(float y:{10.f,height-5}){
  for(auto status:{heavymach::AimStatus::tracking,heavymach::AimStatus::ready,heavymach::AimStatus::blocked}){
   auto label=heavymach::aimHUDLabel(x,x+45,y,width,height,status,1);
   auto strokes=heavymach::aimHUDText(label);XCTAssertGreaterThan(strokes.size,size_t(0));
   for(size_t i=0;i<strokes.size;i+=2){XCTAssertGreaterThanOrEqual(strokes.points[i],0);XCTAssertLessThan(strokes.points[i],width);XCTAssertGreaterThan(strokes.points[i+1],64);XCTAssertLessThan(strokes.points[i+1],height);}
   NSString* expected=status==heavymach::AimStatus::ready?@"LOCKED":status==heavymach::AimStatus::blocked?@"BLOCKED":@"TRACK";
   XCTAssertEqualObjects([NSString stringWithUTF8String:label.text],expected);XCTAssertFalse(label.flash);
  }
 }
 XCTAssertTrue(heavymach::aimHUDLabel(100,140,200,480,320,heavymach::AimStatus::ready,.18).flash);
}
- (void)testAutoAimControlsAttachToHUDAndNextTargetTouch {
 [self autoAimArenaWithWeapon:"P_Wpn_Gun01"];
 auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");
 for(NSValue* value in @[[NSValue valueWithCGSize:CGSizeMake(1200,900)],[NSValue valueWithCGSize:CGSizeMake(1400,650)]]){
  self.game.frame=CGRectMake(0,0,value.CGSizeValue.width,value.CGSizeValue.height);[self.game updateCanvasGeometry];
  auto offset=storage<CGPoint>(self.game,"_hudOffset");auto scale=storage<CGFloat>(self.game,"_hudScale");
  for(int button=1;button<=3;++button){CGRect r=[self.game aimControlRect:button];
   XCTAssertEqualWithAccuracy((r.origin.y-offset.y)/scale,10,.01);
   XCTAssertGreaterThanOrEqual((r.origin.x-offset.x)/scale,334);
   XCTAssertLessThanOrEqual((CGRectGetMaxX(r)-offset.x)/scale,480);
   XCTAssertLessThanOrEqual((CGRectGetMaxY(r)-offset.y)/scale,64);
  }
 }
 XCTAssertLessThan(CGRectGetMaxX([self.game aimControlRect:1]),CGRectGetMinX([self.game aimControlRect:3]));
 XCTAssertLessThan(CGRectGetMaxX([self.game aimControlRect:3]),CGRectGetMinX([self.game aimControlRect:2]));
 auto vp=storage<heavymach::Viewport>(self.game,"_viewport");CGRect r=[self.game aimControlRect:3];
 CGPoint next=CGPointMake(vp.x+CGRectGetMidX(r)*vp.scale,vp.y+(vp.logicalHeight-CGRectGetMidY(r))*vp.verticalScale());
 auto first=session->aimSnapshot().target;auto p=session->player()->position;session->validationSpawnEnemy({"Heli001",{p.x+320,p.y+150},false});
 NSObject* finger=[NSObject new];[self.game routePoint:next state:0 token:finger];[self.game routePoint:next state:2 token:finger];frames(self.game,2);
 XCTAssertNotEqual(session->aimSnapshot().target,first);XCTAssertFalse(session->validationWeaponState().isFire);
 HMMedia* media=[self.game valueForKey:@"media"];CGSize sheet=[media imageSize:@"Extensions/AutoAimControls.png"];
 XCTAssertEqual(sheet.width,138);XCTAssertEqual(sheet.height,92);
}
- (void)testAutoAimSingleToggleRestoresManualTouchFire {
 [self autoAimArenaWithWeapon:"P_Wpn_Gun01"];
 auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");
 XCTAssertTrue(CGRectIsNull([self.game aimControlRect:0]));
 XCTAssertTrue(CGRectIsNull([self.game aimControlRect:4]));
 auto vp=storage<heavymach::Viewport>(self.game,"_viewport");auto r=[self.game aimControlRect:1];
 CGPoint a=CGPointMake(vp.x+CGRectGetMidX(r)*vp.scale,vp.y+(vp.logicalHeight-CGRectGetMidY(r))*vp.verticalScale());
 NSObject* toggle=[NSObject new];
 [self.game routePoint:a state:0 token:toggle];[self.game routePoint:a state:2 token:toggle];
 XCTAssertEqual(session->aimMode(),heavymach::AimMode::manual);
 NSObject* world=[NSObject new];CGPoint target=CGPointMake(vp.x+430*vp.scale,vp.y+(vp.logicalHeight-180)*vp.verticalScale());
 [self.game routePoint:target state:0 token:world];frames(self.game,12);
 XCTAssertTrue(session->validationWeaponState().isFire);
 [self.game routePoint:target state:2 token:world];
 [self.game routePoint:a state:0 token:toggle];[self.game routePoint:a state:2 token:toggle];
 XCTAssertEqual(session->aimMode(),heavymach::AimMode::automatic);
 XCTAssertFalse(session->validationWeaponState().isFire);
 // A cancelled toggle must not change the mode.
 [self.game routePoint:a state:0 token:toggle];[self.game routePoint:a state:3 token:toggle];
 XCTAssertEqual(session->aimMode(),heavymach::AimMode::automatic);
}
- (void)testAutoAimModeSwitchAndTouchKeyboardOwnership {
 [self autoAimArenaWithWeapon:std::string("P_Wpn_Gun01")];auto& session=storage<std::unique_ptr<heavymach::GameSession>>(self.game,"_session");
 auto vp=storage<heavymach::Viewport>(self.game,"_viewport");auto point=[&](int button){auto r=[self.game aimControlRect:button];return CGPointMake(vp.x+CGRectGetMidX(r)*vp.scale,vp.y+(vp.logicalHeight-CGRectGetMidY(r))*vp.verticalScale());};
 NSObject* fire=[NSObject new];NSObject* mode=[NSObject new];NSObject* world=[NSObject new];
 [self.game routePoint:point(2) state:0 token:fire];frames(self.game,12);XCTAssertTrue(session->validationWeaponState().isFire);
 // Releasing an unrelated finger cannot release the dedicated fire button.
 CGPoint empty=CGPointMake(vp.x+vp.logicalWidth*.7*vp.scale,vp.y+vp.logicalHeight*.3*vp.verticalScale());
 [self.game routePoint:empty state:0 token:world];[self.game routePoint:empty state:3 token:world];frames(self.game,2);XCTAssertTrue(session->validationWeaponState().isFire);
 [self.game handleKeyCode:9 pressed:YES];[self.game routePoint:point(2) state:2 token:fire];frames(self.game,2);XCTAssertTrue(session->validationWeaponState().isFire);
 [self.game handleKeyCode:7 pressed:YES];
 [self.game routePoint:point(1) state:0 token:mode];[self.game routePoint:point(1) state:2 token:mode];
 XCTAssertEqual(session->aimMode(),heavymach::AimMode::manual);XCTAssertFalse(session->validationWeaponState().isFire);
 XCTAssertTrue(storage<heavymach::KeyboardInput>(self.game,"_keyboardControls").down(heavymach::KeyAction::right));
 [self.game handleKeyCode:9 pressed:YES];frames(self.game,2);XCTAssertFalse(session->validationWeaponState().isFire);
 [self.game handleKeyCode:9 pressed:NO];[self.game handleKeyCode:9 pressed:YES];frames(self.game,2);XCTAssertTrue(session->validationWeaponState().isFire);
 [self.game setInputAimMode:heavymach::AimMode::off];[self.game handleKeyCode:9 pressed:NO];key(self.game,9);frames(self.game,4);XCTAssertFalse(session->validationWeaponState().isFire);
 [self.game handleKeyCode:7 pressed:NO];[self.game setInputAimMode:heavymach::AimMode::automatic];frames(self.game,2);
 auto first=session->aimSnapshot().target;auto p=session->player()->position;session->validationSpawnEnemy({"Heli001",{p.x+320,p.y+150},false});key(self.game,0x15);frames(self.game,2);XCTAssertNotEqual(session->aimSnapshot().target,first);
 [self.game handleKeyCode:9 pressed:YES];[self.game keyboardDisconnected:nil];frames(self.game,2);XCTAssertFalse(session->validationWeaponState().isFire);
 // Switching mode while the fire finger remains down must discard its capture.
 [self.game routePoint:point(2) state:0 token:fire];frames(self.game,2);XCTAssertTrue(session->validationWeaponState().isFire);
 [self.game routePoint:point(1) state:0 token:mode];[self.game routePoint:point(1) state:2 token:mode];
 [self.game setInputAimMode:heavymach::AimMode::automatic];[self.game routePoint:point(2) state:1 token:fire];frames(self.game,2);XCTAssertFalse(session->validationWeaponState().isFire);
 [self.game routePoint:point(2) state:2 token:fire];[self.game routePoint:point(2) state:0 token:fire];frames(self.game,2);XCTAssertTrue(session->validationWeaponState().isFire);
 [self.game routePoint:point(2) state:3 token:fire];frames(self.game,2);XCTAssertFalse(session->validationWeaponState().isFire);
 XCTAssertEqual(((HMInputTestView*)self.game).keySaveCalls,NSUInteger(0));
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
