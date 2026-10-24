#import <XCTest/XCTest.h>
#import <objc/runtime.h>
#import <CoreMotion/CoreMotion.h>
#import "../../Sources/iOS/HMGameView.h"
#import "../../Sources/iOS/HMMedia.h"
#import "../../Sources/iOS/HMGLRenderer.h"
#import "../../Sources/iOS/HMOpenALAudio.h"
#define HM_VALIDATION 1
#import "../../Sources/iOS/HMValidationControls.h"
#include "../../Sources/Core/GameSession.hpp"
#include <algorithm>
#include <ctime>
#include <vector>
#include <stdexcept>
#include "../../Sources/Core/Viewport.hpp"
#include "../../Sources/Core/TiltState.hpp"

@interface HMGameView (FramePerformance)
- (void)frame:(CADisplayLink*)link;
- (void)drawLogicalFrame;
- (void)sampleOriginalAcceleration;
- (UIInterfaceOrientation)accelerationInterfaceOrientation;
- (int32_t)nextOriginalRandom;
- (void)refreshScreen:(int)screen;
- (void)startSlot:(int)slot mode:(int)mode mission:(int)mission stage:(int)stage;
- (void)advanceOriginalFrame;
- (void)gameButton:(int)identifier state:(int)state;
@end
namespace {
template<class T>T& perfStorage(id object,const char* name){
 Ivar ivar=class_getInstanceVariable(HMGameView.class,name);
 if(!ivar)throw std::runtime_error(name);
 return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>((__bridge void*)object)+ivar_getOffset(ivar));
}
double cpuMilliseconds(){return 1000.0*double(std::clock())/CLOCKS_PER_SEC;}
NSDictionary* metrics(const std::vector<double>& values){
 if(values.empty())return @{@"frames":@0,@"error":@"no samples"};
 auto sorted=values;std::sort(sorted.begin(),sorted.end());double sum=0;int over=0;
 for(double x:values){sum+=x;if(x>1000.0/60)++over;}
 return @{ @"mean_ms":@(sum/values.size()),@"p95_ms":@(sorted[(sorted.size()*95+99)/100-1]),@"max_ms":@(sorted.back()),@"over_60hz_budget":@(over),@"frames":@(values.size()) };
}
void emit(NSDictionary* record){NSData* json=[NSJSONSerialization dataWithJSONObject:record options:NSJSONWritingSortedKeys error:nil];NSLog(@"HM_FRAME_PERF %@",[[NSString alloc] initWithData:json encoding:NSUTF8StringEncoding]);}
}
@interface HMPerformanceFrame:NSObject
@property(nonatomic)CFTimeInterval timestamp;
@end
@implementation HMPerformanceFrame
@end
@interface HMDirectionTestKey:NSObject
@property(nonatomic)UIKeyboardHIDUsage keyCode;
@end
@implementation HMDirectionTestKey
@end
@interface HMDirectionTestPress:NSObject
@property(nonatomic,strong)HMDirectionTestKey* key;
@end
@implementation HMDirectionTestPress
@end
@interface HMPerformanceView:HMGameView
@end
@implementation HMPerformanceView
- (void)persistSave {} // No fixture or user save writes.
@end
// Physical CoreMotion samples are in portrait body axes, independent of UI rotation.
@interface HMTestAcceleration:NSObject
@property(nonatomic)CMAcceleration suppliedAcceleration;
@end
@implementation HMTestAcceleration
- (CMAcceleration)acceleration {return self.suppliedAcceleration;}
@end
@interface HMTestMotion:NSObject
@property(nonatomic,strong)HMTestAcceleration* suppliedSample;
@end
@implementation HMTestMotion
- (BOOL)isAccelerometerAvailable {return YES;}
- (CMAccelerometerData*)accelerometerData {return (CMAccelerometerData*)self.suppliedSample;}
- (void)stopAccelerometerUpdates {}
@end
@interface HMPhysicalTiltView:HMPerformanceView
@property(nonatomic)UIInterfaceOrientation suppliedOrientation;
@end
@implementation HMPhysicalTiltView
- (UIInterfaceOrientation)accelerationInterfaceOrientation {return self.suppliedOrientation;}
@end
@interface HMPerformanceSilentMedia:HMMedia
@property(nonatomic)NSUInteger soundCalls;
@property(nonatomic)NSUInteger weaponFireCalls;
@end
@implementation HMPerformanceSilentMedia
- (void)sound:(NSString*)name {++self.soundCalls;if([name hasPrefix:@"Wpn_"])++self.weaponFireCalls;}
- (void)sound:(NSString*)name pan:(float)pan {++self.soundCalls;if([name hasPrefix:@"Wpn_"])++self.weaponFireCalls;}
@end
// Display-link harness runs the actual presentation path and real audio.
@interface HMPerformanceAudibleMedia:HMMedia {
@public std::vector<double> soundTimes;
}
@end
@implementation HMPerformanceAudibleMedia
- (void)sound:(NSString*)name pan:(float)pan {double start=CACurrentMediaTime();[super sound:name pan:pan];soundTimes.push_back((CACurrentMediaTime()-start)*1000);}
@end
@interface HMDisplayProbe:NSObject {
@public std::vector<double> intervals,work,coldWork; CFTimeInterval previous; int count; NSUInteger maxBullets;
}
@property(nonatomic,strong) HMPerformanceView* view;
@property(nonatomic,strong) XCTestExpectation* done;
@property(nonatomic,strong) CADisplayLink* link;
@property(nonatomic)BOOL firing;
- (void)step:(CADisplayLink*)link;
@end
@implementation HMDisplayProbe
- (void)step:(CADisplayLink*)link {
 double now=CACurrentMediaTime();
 if(self.firing){auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(self.view,"_session");if(session){session->touch({470,160},1);maxBullets=std::max(maxBullets,session->bulletCount());}}
 double start=CACurrentMediaTime();[self.view frame:link];double elapsed=(CACurrentMediaTime()-start)*1000;
 if(count<30)coldWork.push_back(elapsed);
 if(count>=30){work.push_back(elapsed);if(previous)intervals.push_back((now-previous)*1000);}
 previous=now;++count;
 if(count==210){[link invalidate];self.link=nil;[self.done fulfill];}
}
@end
@interface HMProductionTimerView:HMPerformanceView {
@public std::vector<double> timerIntervals,timerWork,copyTimes,quadTimes,swapTimes,waitTimes;int timerCount;double previousTimer;
}
@property(nonatomic,strong)XCTestExpectation* timerDone;
@property(nonatomic)BOOL firing;
@end
@implementation HMProductionTimerView
- (void)frame:(id)sender {
 if(!self.timerDone){[super frame:sender];return;}
 double now=CACurrentMediaTime();
 if(self.firing){auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(self,"_session");if(session)session->touch({470,160},1);}
 [super frame:sender];
 if(timerCount>=30){timerIntervals.push_back((now-previousTimer)*1000);timerWork.push_back((CACurrentMediaTime()-now)*1000);}
 if(timerCount>=30){NSDictionary* t=[[self valueForKey:@"glRenderer"]diagnostics][@"presentation_timings_ms"];copyTimes.push_back([t[@"copy"] doubleValue]);quadTimes.push_back([t[@"quad"] doubleValue]);swapTimes.push_back([t[@"swap"] doubleValue]);waitTimes.push_back([t[@"wait_and_restore"] doubleValue]);}
 previousTimer=now;
 if(++timerCount==210){[self setApplicationActive:NO];[[self valueForKey:@"frameTimer"]invalidate];[self.timerDone fulfill];self.timerDone=nil;}
}
@end
@interface HMFramePerformanceTests:XCTestCase
@end
@implementation HMFramePerformanceTests
- (void)testPhysicalTiltMovesTowardLowerScreenEdgeInBothLandscapeOrientations {
 for(NSNumber* orientation in @[@(UIInterfaceOrientationLandscapeLeft),@(UIInterfaceOrientationLandscapeRight)]){@autoreleasepool{
  HMPhysicalTiltView* view=[[HMPhysicalTiltView alloc]initWithFrame:CGRectMake(0,0,912,420)];
  view.suppliedOrientation=(UIInterfaceOrientation)orientation.integerValue;
  [[view valueForKey:@"frameTimer"]invalidate];[[view valueForKey:@"motionTimer"]invalidate];
  [(CMMotionManager*)[view valueForKey:@"motion"]stopAccelerometerUpdates];
  HMTestMotion* motion=[HMTestMotion new];motion.suppliedSample=[HMTestAcceleration new];[view setValue:motion forKey:@"motion"];
  auto& save=perfStorage<heavymach::SaveData>(view,"_save");save={};[view startSlot:0 mode:0 mission:0 stage:0];
  auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");XCTAssertTrue(bool(session));if(!session)return;
  // Home-left: portrait +Y points right. Home-right: portrait +Y points left.
  const double rightY=view.suppliedOrientation==UIInterfaceOrientationLandscapeLeft?.2:-.2;
  motion.suppliedSample.suppliedAcceleration=CMAcceleration{0,rightY,-.98};
  const float start=session->player()->position.x;
  for(int i=0;i<120;++i){[view sampleOriginalAcceleration];[view advanceOriginalFrame];}
  const float right=session->player()->position.x;
  XCTAssertGreaterThan(right,start+10,@"Right edge down, orientation %@",orientation);
  motion.suppliedSample.suppliedAcceleration=CMAcceleration{0,-rightY,-.98};
  for(int i=0;i<90;++i){[view sampleOriginalAcceleration];[view advanceOriginalFrame];}
  XCTAssertLessThan(session->player()->position.x,right-10,@"Left edge down, orientation %@",orientation);
  // Screen-relative buttons must not be reversed a second time.
  HMValidationControls* controls=[view valueForKey:@"validationControls"];controls.selectedSegmentIndex=2;
  perfStorage<heavymach::TiltState>(view,"_tilt")={};[view sampleOriginalAcceleration];
  XCTAssertGreaterThan(perfStorage<heavymach::TiltState>(view,"_tilt").y,0);
  [view setApplicationActive:NO];
 }}
}
- (void)testValidationDirectionControlsMoveRealPlayerAndReleaseOnBackground {
 HMPerformanceView* view=[[HMPerformanceView alloc]initWithFrame:CGRectMake(0,0,912,420)];
 [[view valueForKey:@"frameTimer"]invalidate];[[view valueForKey:@"motionTimer"]invalidate];
 [(CMMotionManager*)[view valueForKey:@"motion"]stopAccelerometerUpdates];
 auto& save=perfStorage<heavymach::SaveData>(view,"_save");save={};
 [view startSlot:0 mode:0 mission:0 stage:0];XCTAssertNil([view valueForKey:@"loadError"]);
 auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
 XCTAssertTrue(bool(session));if(!session)return;
 HMValidationControls* controls=[view valueForKey:@"validationControls"];
 XCTAssertEqual(controls.numberOfSegments,NSInteger(4));XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(3));
 const float start=session->player()->position.x;
 [(UIButton*)controls.subviews[2] sendActionsForControlEvents:UIControlEventTouchUpInside];
 for(int i=0;i<120;++i){[view sampleOriginalAcceleration];[view advanceOriginalFrame];}
 const float right=session->player()->position.x;XCTAssertGreaterThan(right,start+10);
 [(UIButton*)controls.subviews[0] sendActionsForControlEvents:UIControlEventTouchUpInside];
 for(int i=0;i<90;++i){[view sampleOriginalAcceleration];[view advanceOriginalFrame];}
 XCTAssertLessThan(session->player()->position.x,right-10);
 controls.selectedSegmentIndex=1;[controls sendActionsForControlEvents:UIControlEventValueChanged];
 for(int i=0;i<100;++i)[view sampleOriginalAcceleration];
 XCTAssertEqualWithAccuracy(perfStorage<heavymach::TiltState>(view,"_tilt").y,0,.00001);
 controls.selectedSegmentIndex=2;[controls sendActionsForControlEvents:UIControlEventValueChanged];
 [view gameButton:100 state:2];XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(1));
 [view gameButton:101 state:2];XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(1));
 controls.selectedSegmentIndex=2;[controls sendActionsForControlEvents:UIControlEventValueChanged];
 [view setApplicationActive:NO];XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(1));
 controls.selectedSegmentIndex=3;[controls sendActionsForControlEvents:UIControlEventValueChanged];
 [view setApplicationActive:NO];XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(3));
}
- (void)testValidationKeyboardReleaseAndCancellationClearDirection {
 HMPerformanceView* view=[[HMPerformanceView alloc]initWithFrame:CGRectMake(0,0,912,420)];
 [[view valueForKey:@"frameTimer"]invalidate];[[view valueForKey:@"motionTimer"]invalidate];
 HMDirectionTestPress* press=[HMDirectionTestPress new];press.key=[HMDirectionTestKey new];press.key.keyCode=UIKeyboardHIDUsageKeyboardRightArrow;
 NSSet* presses=[NSSet setWithObject:press];
 HMValidationControls* controls=[view valueForKey:@"validationControls"];
 [view pressesBegan:presses withEvent:nil];[view sampleOriginalAcceleration];
 XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(2));XCTAssertGreaterThan(perfStorage<heavymach::TiltState>(view,"_tilt").y,0);
 [view pressesEnded:presses withEvent:nil];XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(1));
 XCTAssertFalse([[view valueForKey:@"rightPressed"]boolValue]);
 [view pressesBegan:presses withEvent:nil];[view pressesCancelled:presses withEvent:nil];
 XCTAssertEqual(controls.selectedSegmentIndex,NSInteger(1));XCTAssertFalse([[view valueForKey:@"rightPressed"]boolValue]);
 [view setApplicationActive:NO];
}
- (void)testProductionNSTimerTitleMenuAndFiring {
 UIWindowScene* scene=nil;for(UIScene* s in UIApplication.sharedApplication.connectedScenes)if([s isKindOfClass:UIWindowScene.class]){scene=(UIWindowScene*)s;break;}
 XCTAssertNotNil(scene);if(!scene)return;
 UIWindow* old=scene.keyWindow;HMGameView* host=(HMGameView*)old.rootViewController.view;[host setApplicationActive:NO];
 for(NSString* name in @[@"title",@"menu",@"firing",@"empty_menu"]){@autoreleasepool{
  HMProductionTimerView* view=[[HMProductionTimerView alloc]initWithFrame:scene.coordinateSpace.bounds];
  auto& save=perfStorage<heavymach::SaveData>(view,"_save");save={};
  if(![name isEqual:@"empty_menu"])for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);
  if([name isEqual:@"menu"]||[name isEqual:@"empty_menu"])[view refreshScreen:3];
  if([name isEqual:@"firing"]){[view startSlot:0 mode:0 mission:0 stage:0];view.firing=YES;}
  UIWindow* window=[[UIWindow alloc]initWithWindowScene:scene];UIViewController* vc=[UIViewController new];vc.view=view;window.rootViewController=vc;[window makeKeyAndVisible];[view layoutIfNeeded];
  view.timerDone=[self expectationWithDescription:name];[self waitForExpectations:@[view.timerDone] timeout:20];
  XCTAssertEqual(view->timerWork.size(),size_t(180));
  emit(@{@"phase":@"production_NSTimer_180_frames",@"scene":name,@"frame_work":metrics(view->timerWork),@"callback_intervals":metrics(view->timerIntervals),@"renderer":[[view valueForKey:@"glRenderer"]diagnostics]});
  emit(@{@"phase":@"presentation_cost_breakdown",@"scene":name,@"copy":metrics(view->copyTimes),@"quad":metrics(view->quadTimes),@"swap":metrics(view->swapTimes),@"wait_and_restore":metrics(view->waitTimes)});
  if([name isEqual:@"empty_menu"]){
   // This is an in-process unit test. Read our GL buffer without requesting
   // XCTest UI automation authorization or modifying the presentation path.
   HMGLRenderer* renderer=[view valueForKey:@"glRenderer"];UIImage* image=[renderer snapshot];XCTAssertNotNil(image);
   XCTAttachment* capture=[XCTAttachment attachmentWithImage:image];capture.name=@"native-empty-menu-logical";capture.lifetime=XCTAttachmentLifetimeKeepAlways;[self addAttachment:capture];
  }
  XCTAssertEqualObjects([[view valueForKey:@"glRenderer"]diagnostics][@"lastError"],@"");
  [view setApplicationActive:NO];[[view valueForKey:@"frameTimer"]invalidate];[[view valueForKey:@"motionTimer"]invalidate];window.hidden=YES;window.rootViewController=nil;
 }}
 [old makeKeyAndVisible];[host setApplicationActive:YES];
}
- (void)testCompatibilityRandomSequence {
 HMPerformanceView* view=[[HMPerformanceView alloc] initWithFrame:CGRectMake(0,0,480,320)];
 [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
 [view setValue:@1 forKey:@"randomState"];
 const int32_t expected[]={270369,67634689,499951813,307599695,251205585};
 for(int32_t value:expected)XCTAssertEqual([view nextOriginalRandom],value);
 [view setValue:@0 forKey:@"randomState"];XCTAssertEqual([view nextOriginalRandom],270369);
 [view setApplicationActive:NO];
}
- (void)testSimulatorArrowsUseOriginalFiftyHzFilteredAcceleration {
 HMPerformanceView* view=[[HMPerformanceView alloc] initWithFrame:CGRectMake(0,0,912,420)];
 [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
 [(CMMotionManager*)[view valueForKey:@"motion"] stopAccelerometerUpdates];
 [view setValue:@YES forKey:@"rightPressed"];
 [view sampleOriginalAcceleration];
 auto& tilt=perfStorage<heavymach::TiltState>(view,"_tilt");
 XCTAssertEqual(tilt.y,float(double(.2f)*double(.1f)));
 [view setValue:@NO forKey:@"rightPressed"];[view sampleOriginalAcceleration];
 XCTAssertEqual(tilt.y,float(double(float(double(.2f)*double(.1f)))*(1.0-double(.1f))));
 [view setApplicationActive:NO];
}
- (void)testProductionTimerAndFullScreenTouchViewport {
 HMPerformanceView* view=[[HMPerformanceView alloc] initWithFrame:CGRectMake(0,0,912,420)];
 NSTimer* timer=[view valueForKey:@"frameTimer"];
 XCTAssertTrue([timer isKindOfClass:NSTimer.class]);
 // A one-shot timer exposes timeInterval=0; the guest deadline is stored separately.
 XCTAssertGreaterThan([[view valueForKey:@"nextTimerDue"] doubleValue],CACurrentMediaTime());
 XCTAssertEqualWithAccuracy(view.contentScaleFactor,UIScreen.mainScreen.nativeScale,1e-9);
 XCTAssertEqualObjects(((CAEAGLLayer*)view.layer).drawableProperties[kEAGLDrawablePropertyColorFormat],kEAGLColorFormatRGB565);
 [view layoutSubviews];
 auto& viewport=perfStorage<heavymach::Viewport>(view,"_viewport");
 XCTAssertEqualWithAccuracy(viewport.x,141,1e-9);
 XCTAssertEqualWithAccuracy(viewport.y,0,1e-9);
 auto point=viewport.logical(141,420);XCTAssertTrue(point.has_value());
 XCTAssertEqual(point->first,0);XCTAssertEqual(point->second,0);
 [view setApplicationActive:NO];double paused=[[view valueForKey:@"time"] doubleValue];
 [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:.05]];
 XCTAssertEqual([[view valueForKey:@"time"] doubleValue],paused);
 [view setApplicationActive:YES];
 [[NSRunLoop mainRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:.1]];
 double ticks=([[view valueForKey:@"time"] doubleValue]-paused)/0.01666666753590107;
 XCTAssertGreaterThan(ticks,0);XCTAssertEqualWithAccuracy(ticks,std::round(ticks),1e-8);
 [view setApplicationActive:NO];[timer invalidate];
}
- (void)testOriginalRenderSceneAdvancesExactlyOncePerFrame {
 // Original IPA 0x21e0 renderScene: g_time += 0.01666666753590107,
 // then one GameMain tick and one drawVisual, including after delayed timers.
 const double originalStep=0.01666666753590107;
 for(NSString* mode in @[@"ideal60",@"timestamp_jitter",@"missed_refresh"]){
  HMPerformanceView* view=[[HMPerformanceView alloc] initWithFrame:CGRectMake(0,0,912,420)];
  [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
  [(CMMotionManager*)[view valueForKey:@"motion"] stopAccelerometerUpdates];
  XCTAssertNil([view valueForKey:@"loadError"]);
  HMPerformanceFrame* frame=[HMPerformanceFrame new];double timestamp=100;int mismatches=0;
  for(int n=0;n<600;++n){
   timestamp+=1.0/60.0;
   if([mode isEqualToString:@"missed_refresh"]&&n%20==0)timestamp+=1.0/60.0;
   frame.timestamp=timestamp+([mode isEqualToString:@"timestamp_jitter"]?(n%2?.0003:-.0003):0);
   double before=[[view valueForKey:@"time"] doubleValue];
   [view frame:(CADisplayLink*)frame];
   double advance=[[view valueForKey:@"time"] doubleValue]-before;
   if(std::abs(advance-originalStep)>1e-8)++mismatches;
  }
  NSLog(@"HM_TICK_COMPARISON mode=%@ callbacks=600 wrong_tick_counts=%d",mode,mismatches);
  XCTAssertEqual(mismatches,0,@"Original renderScene must advance exactly once for each rendered frame (%@)",mode);
  [view setApplicationActive:NO];
 }
}
- (void)testActualOffscreenTitleMenusAndFiringFrameCosts {
 // This measures real UIKit rendering without attaching a view or injecting UI
 // input. Audio playback is excluded; actual HMMedia image/raster code is used.
 for(NSString* scene in @[@"title_animation",@"slot_preview",@"stage01_firing"]){@autoreleasepool{
  double setupStart=CACurrentMediaTime();
  HMPerformanceView* view=[[HMPerformanceView alloc] initWithFrame:CGRectMake(0,0,912,420)];
  [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
  [(CMMotionManager*)[view valueForKey:@"motion"] stopAccelerometerUpdates];
  NSString* loadError=[view valueForKey:@"loadError"];
  XCTAssertNil(loadError,@"Actual resource load must succeed: %@",loadError);if(loadError)continue;
  HMPerformanceSilentMedia* media=[HMPerformanceSilentMedia new];
  [view setValue:media forKey:@"media"];
  // The menus and HUD retain their original media; replace those references too.
  [[view valueForKey:@"menus"] setValue:media forKey:@"media"];
  [[view valueForKey:@"hud"] setValue:media forKey:@"media"];
  auto& save=perfStorage<heavymach::SaveData>(view,"_save");save={};
  for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);
  [view setValue:@11 forKey:@"randomState"]; // Repeatable gameplay stream after constructors' timestamp seed.
  if([scene isEqualToString:@"slot_preview"])[view refreshScreen:3];
  if([scene isEqualToString:@"stage01_firing"]){
   [view startSlot:0 mode:0 mission:0 stage:0];
   auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
   XCTAssertTrue(bool(session));if(!session)continue;
   // Public gameplay touch API, without UIKit input injection.
   session->touch({470,160},0);
  }
  [view layoutSubviews];
  UIGraphicsImageRendererFormat* format=[UIGraphicsImageRendererFormat defaultFormat];
  format.scale=3;format.opaque=YES;format.preferredRange=UIGraphicsImageRendererFormatRangeStandard;
  UIGraphicsImageRenderer* target=[[UIGraphicsImageRenderer alloc] initWithSize:CGSizeMake(912,420) format:format];
  HMPerformanceFrame* frame=[HMPerformanceFrame new];frame.timestamp=1;
  std::vector<double> ticks,draws,total,cpuTicks,cpuDraws;
  NSUInteger maxBullets=0,maxRenderItems=0;double setupMs=(CACurrentMediaTime()-setupStart)*1000;
  for(int n=0;n<121;++n){@autoreleasepool{


   double t0=CACurrentMediaTime(),c0=cpuMilliseconds();
   if([scene isEqualToString:@"stage01_firing"]){
    auto& firing=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
    if(firing)firing->touch({470,160},1);
   }
   [view frame:(CADisplayLink*)frame];
   double c1=cpuMilliseconds(),t1=CACurrentMediaTime();
   UIImage* image=[target imageWithActions:^(UIGraphicsImageRendererContext* context){[view drawRect:view.bounds];}];
   double c2=cpuMilliseconds(),t2=CACurrentMediaTime();
   XCTAssertTrue(image.CGImage!=nullptr);
   auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
   if(session){maxBullets=std::max(maxBullets,session->bulletCount());maxRenderItems=std::max(maxRenderItems,session->renderItems().size());}
   double tick=(t1-t0)*1000,draw=(t2-t1)*1000;
   if(n==0)emit(@{@"scene":scene,@"phase":@"first_frame",@"setup_ms":@(setupMs),@"tick_wall_ms":@(tick),@"draw_wall_ms":@(draw),@"total_wall_ms":@(tick+draw),@"tick_process_cpu_ms":@(c1-c0),@"draw_process_cpu_ms":@(c2-c1)});
   else {ticks.push_back(tick);draws.push_back(draw);total.push_back(tick+draw);cpuTicks.push_back(c1-c0);cpuDraws.push_back(c2-c1);}
  }}
  emit(@{@"scene":scene,@"phase":@"warm_120_frames",@"tick_wall":metrics(ticks),@"draw_wall":metrics(draws),@"total_wall":metrics(total),@"tick_process_cpu":metrics(cpuTicks),@"draw_process_cpu":metrics(cpuDraws),@"weapon_fire_sound_events":@(media.weaponFireCalls),@"max_bullets":@(maxBullets),@"max_render_items":@(maxRenderItems),@"suppressed_sound_calls":@(media.soundCalls),@"audio_playback":@"excluded",@"budget_ms":@(1000.0/60),@"resolution":@"logical480x320 -> view912x420pt -> bitmap2736x1260 scale3",@"safe_area":@"zero (offscreen view, full viewport)",@"cpu_scope":@"process (includes other threads)",@"draw_scope":@"actual drawRect plus offscreen target allocation/composition",@"cold_scope":@"fresh media cache; process/OS cache not reset"});
  if([scene isEqualToString:@"stage01_firing"]){XCTAssertGreaterThan(maxBullets,NSUInteger(0),@"Firing workload must produce real bullets");XCTAssertGreaterThan(media.weaponFireCalls,NSUInteger(0),@"Real firing must emit weapon sound events");}
  [view setApplicationActive:NO];
 }}
}
- (void)testActualOpenGLTitleMenusAndFiringFrameCosts {
 // This measures real UIKit rendering without attaching a view or injecting UI
 // input. Audio playback is excluded; actual HMMedia image/raster code is used.
 for(NSString* scene in @[@"title_animation",@"slot_preview",@"stage01_firing"]){@autoreleasepool{
  double setupStart=CACurrentMediaTime();
  HMPerformanceView* view=[[HMPerformanceView alloc] initWithFrame:CGRectMake(0,0,912,420)];
  [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];
  [(CMMotionManager*)[view valueForKey:@"motion"] stopAccelerometerUpdates];
  NSString* loadError=[view valueForKey:@"loadError"];
  XCTAssertNil(loadError,@"Actual resource load must succeed: %@",loadError);if(loadError)continue;
  HMPerformanceSilentMedia* media=[HMPerformanceSilentMedia new];
  [view setValue:media forKey:@"media"];
  // The menus and HUD retain their original media; replace those references too.
  [[view valueForKey:@"menus"] setValue:media forKey:@"media"];
  [[view valueForKey:@"hud"] setValue:media forKey:@"media"];
  auto& save=perfStorage<heavymach::SaveData>(view,"_save");save={};
  for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);
  [view setValue:@11 forKey:@"randomState"]; // Repeatable gameplay stream after constructors' timestamp seed.
  if([scene isEqualToString:@"slot_preview"])[view refreshScreen:3];
  if([scene isEqualToString:@"stage01_firing"]){
   [view startSlot:0 mode:0 mission:0 stage:0];
   auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
   XCTAssertTrue(bool(session));if(!session)continue;
   // Public gameplay touch API, without UIKit input injection.
   session->touch({470,160},0);
  }
  [view layoutSubviews];
  HMGLRenderer* gpu=[view valueForKey:@"glRenderer"];XCTAssertNotNil(gpu);if(!gpu)continue;
  HMPerformanceFrame* frame=[HMPerformanceFrame new];frame.timestamp=1;
  std::vector<double> ticks,draws,total,cpuTicks,cpuDraws;
  NSUInteger maxBullets=0,maxRenderItems=0;double setupMs=(CACurrentMediaTime()-setupStart)*1000;
  for(int n=0;n<121;++n){@autoreleasepool{


   double t0=CACurrentMediaTime(),c0=cpuMilliseconds();
   if([scene isEqualToString:@"stage01_firing"]){
    auto& firing=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
    if(firing)firing->touch({470,160},1);
   }
   [view frame:(CADisplayLink*)frame];
   double c1=cpuMilliseconds(),t1=CACurrentMediaTime();
   XCTAssertTrue([gpu beginFrame]);media.glRenderer=gpu;
   [view drawLogicalFrame];[gpu finish]; // GPU completion included, not just submission.
   media.glRenderer=nil;[gpu endFrame];
   double c2=cpuMilliseconds(),t2=CACurrentMediaTime();
   if(n==0){
    UIImage* image=[gpu snapshot];XCTAssertTrue(image.CGImage!=nullptr);
    XCTAttachment* attachment=[XCTAttachment attachmentWithImage:image];attachment.name=[scene stringByAppendingString:@"-OpenGL"];attachment.lifetime=XCTAttachmentLifetimeKeepAlways;[self addAttachment:attachment];
   }
   auto& session=perfStorage<std::unique_ptr<heavymach::GameSession>>(view,"_session");
   if(session){maxBullets=std::max(maxBullets,session->bulletCount());maxRenderItems=std::max(maxRenderItems,session->renderItems().size());}
   double tick=(t1-t0)*1000,draw=(t2-t1)*1000;
   if(n==0)emit(@{@"scene":scene,@"phase":@"first_frame",@"setup_ms":@(setupMs),@"tick_wall_ms":@(tick),@"draw_wall_ms":@(draw),@"total_wall_ms":@(tick+draw),@"tick_process_cpu_ms":@(c1-c0),@"draw_process_cpu_ms":@(c2-c1)});
   else {ticks.push_back(tick);draws.push_back(draw);total.push_back(tick+draw);cpuTicks.push_back(c1-c0);cpuDraws.push_back(c2-c1);}
  }}
  emit(@{@"scene":scene,@"phase":@"warm_120_frames",@"tick_wall":metrics(ticks),@"draw_wall":metrics(draws),@"total_wall":metrics(total),@"tick_process_cpu":metrics(cpuTicks),@"draw_process_cpu":metrics(cpuDraws),@"weapon_fire_sound_events":@(media.weaponFireCalls),@"max_bullets":@(maxBullets),@"max_render_items":@(maxRenderItems),@"suppressed_sound_calls":@(media.soundCalls),@"audio_playback":@"excluded",@"budget_ms":@(1000.0/60),@"resolution":@"logical480x320 GPU framebuffer",@"safe_area":@"zero (offscreen view, full viewport)",@"cpu_scope":@"process (includes other threads)",@"draw_scope":@"actual drawLogicalFrame OpenGL ES1.1 + glFinish (GPU completion), no drawable presentation/readback",@"cold_scope":@"fresh media cache; process/OS cache not reset"});
  if([scene isEqualToString:@"stage01_firing"]){XCTAssertGreaterThan(maxBullets,NSUInteger(0),@"Firing workload must produce real bullets");XCTAssertGreaterThan(media.weaponFireCalls,NSUInteger(0),@"Real firing must emit weapon sound events");}
  NSLog(@"HM_GL_DIAGNOSTICS %@",[gpu diagnostics]);
  XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
  XCTAssertLessThan([metrics(draws)[@"p95_ms"] doubleValue],1000.0/60.0,@"Simulator render budget; not iPhone FPS certification");
  [view setApplicationActive:NO];
 }}
}
- (void)testOnscreenOpenGLCadenceAudioAndResume {
 UIWindowScene* scene=nil;for(UIScene* candidate in UIApplication.sharedApplication.connectedScenes)if([candidate isKindOfClass:UIWindowScene.class]){scene=(UIWindowScene*)candidate;break;}
 XCTAssertNotNil(scene);if(!scene)return;
 UIWindow* oldWindow=scene.keyWindow;HMGameView* host=[oldWindow.rootViewController.view isKindOfClass:HMGameView.class]?(HMGameView*)oldWindow.rootViewController.view:nil;
 [host setApplicationActive:NO];
 for(NSString* name in @[@"title_animation",@"slot_preview",@"stage01_firing"]){@autoreleasepool{
  HMPerformanceView* view=[[HMPerformanceView alloc] initWithFrame:scene.coordinateSpace.bounds];
  [[view valueForKey:@"frameTimer"] invalidate];[[view valueForKey:@"motionTimer"] invalidate];[(CMMotionManager*)[view valueForKey:@"motion"] stopAccelerometerUpdates];
  HMPerformanceAudibleMedia* audible=[HMPerformanceAudibleMedia new];XCTAssertTrue([audible prepareOriginalSounds]);XCTAssertEqual([[audible.audio diagnostics][@"bufferCount"] intValue],31);[view setValue:audible forKey:@"media"];[[view valueForKey:@"menus"] setValue:audible forKey:@"media"];[[view valueForKey:@"hud"] setValue:audible forKey:@"media"];
  auto& save=perfStorage<heavymach::SaveData>(view,"_save");save={};for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);[view setValue:@11 forKey:@"randomState"];
  if([name isEqualToString:@"slot_preview"])[view refreshScreen:3];
  if([name isEqualToString:@"stage01_firing"])[view startSlot:0 mode:0 mission:0 stage:0];
  UIWindow* window=[[UIWindow alloc] initWithWindowScene:scene];UIViewController* controller=[UIViewController new];controller.view=view;window.rootViewController=controller;[window makeKeyAndVisible];[view layoutIfNeeded];
  HMDisplayProbe* probe=[HMDisplayProbe new];probe.view=view;probe.firing=[name isEqualToString:@"stage01_firing"];probe.done=[self expectationWithDescription:name];
  probe.link=[CADisplayLink displayLinkWithTarget:probe selector:@selector(step:)];probe.link.preferredFrameRateRange=CAFrameRateRangeMake(60,60,60);[probe.link addToRunLoop:NSRunLoop.mainRunLoop forMode:NSRunLoopCommonModes];
  [self waitForExpectations:@[probe.done] timeout:15];[probe.link invalidate];probe.link=nil;
  XCTAssertEqual(probe->work.size(),size_t(180));
  HMGLRenderer* gpu=[view valueForKey:@"glRenderer"];
  emit(@{@"phase":@"onscreen_180_frames_after_30_warmup",@"scene":name,@"audio_playback":@"enabled",@"frame_work":metrics(probe->work),@"first_30_frame_work":metrics(probe->coldWork),@"callback_intervals":metrics(probe->intervals),@"renderer":[gpu diagnostics],@"audio_backend":[audible.audio diagnostics],@"max_bullets":@(probe->maxBullets),@"scope":@"simulator real display link + EAGL presentation; callback cadence is not physical scanout"});
  XCTAssertEqualObjects([audible.audio diagnostics][@"error"],@"");if(probe.firing)XCTAssertGreaterThan([[audible.audio diagnostics][@"startCount"] intValue],0);
  if(!audible->soundTimes.empty())emit(@{@"phase":@"audio_call_cost",@"scene":name,@"sound_calls":metrics(audible->soundTimes)});
  XCTAssertLessThan([metrics(probe->work)[@"p95_ms"] doubleValue],1000.0/60.0,@"Full simulator frame budget with audio");
  XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");if(probe.firing)XCTAssertGreaterThan(probe->maxBullets,NSUInteger(0));
  XCTAttachment* shot=[XCTAttachment attachmentWithImage:[gpu snapshot]];shot.name=[name stringByAppendingString:@"-presented-logical"];shot.lifetime=XCTAttachmentLifetimeKeepAlways;[self addAttachment:shot];
  NSUInteger before=[[gpu diagnostics][@"frames"] unsignedIntegerValue];[view layoutSubviews];[view drawRect:view.bounds];XCTAssertEqual([[gpu diagnostics][@"frames"] unsignedIntegerValue],before,@"Layout and UIKit redraw must not add a game draw outside the original timer");[view setApplicationActive:NO];[view frame:(CADisplayLink*)[HMPerformanceFrame new]];XCTAssertEqual([[gpu diagnostics][@"frames"] unsignedIntegerValue],before);
  [view setApplicationActive:YES];[view frame:nil];XCTAssertGreaterThan([[gpu diagnostics][@"frames"] unsignedIntegerValue],before);[view setApplicationActive:NO];
  window.hidden=YES;window.rootViewController=nil;probe.view=nil;
 }}
 [oldWindow makeKeyAndVisible];[host setApplicationActive:YES];
}

@end
