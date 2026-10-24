#import <XCTest/XCTest.h>
#import <CoreMotion/CoreMotion.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <objc/runtime.h>
#import "../../Sources/iOS/HMGameView.h"
#import "../../Sources/iOS/HMGLRenderer.h"
#define HM_VALIDATION 1
#include "../../Sources/Core/GameSession.hpp"
#include "../../Sources/Core/Viewport.hpp"
#include <algorithm>
#include <vector>
#include <stdexcept>

@interface HMGameView (PresentationComparison)
- (void)frame:(CADisplayLink*)link;
- (void)refreshScreen:(int)screen;
- (void)startSlot:(int)slot mode:(int)mode mission:(int)mission stage:(int)stage;
@end
namespace {
template<class T>T& storage(id o,Class cls,const char* name){
 Ivar i=class_getInstanceVariable(cls,name);if(!i)throw std::runtime_error(name);
 return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>((__bridge void*)o)+ivar_getOffset(i));
}
NSDictionary* distribution(std::vector<double> v){
 if(v.empty())return @{@"samples":@0};std::sort(v.begin(),v.end());double sum=0;int late=0;
 for(double n:v){sum+=n;if(n>25)++late;}
 return @{@"samples":@(v.size()),@"mean_ms":@(sum/v.size()),@"p95_ms":@(v[(v.size()*95+99)/100-1]),@"max_ms":@(v.back()),@"over25ms":@(late)};
}
}
@interface HMComparisonView:HMGameView
@property(nonatomic)BOOL fullViewport,linear,skipFrame,lowResolution;
@end
@implementation HMComparisonView
- (void)persistSave {}
- (void)frame:(CADisplayLink*)link {
 if(self.skipFrame)return;
 if(self.lowResolution)self.layer.contentsScale=1;
 if(self.fullViewport)storage<heavymach::Viewport>(self,HMGameView.class,"_viewport")=heavymach::Viewport::fit(0,0,self.bounds.size.width,self.bounds.size.height);
 [super frame:link];
}
@end
@interface HMComparisonProbe:NSObject {
@public std::vector<double> wall,display,work,latency;int count;double previousWall,previousDisplay,previousWork;NSUInteger bullets;
}
@property(nonatomic,strong)HMComparisonView* view;
@property(nonatomic,strong)CADisplayLink* link;
@property(nonatomic,strong)XCTestExpectation* done;
@property(nonatomic,strong)NSMutableArray* lateEvents;
@property(nonatomic)BOOL firing;
@end
@implementation HMComparisonProbe
- (void)step:(CADisplayLink*)link {
 double now=CACurrentMediaTime();
 if(self.firing&&!self.view.skipFrame){auto& s=storage<std::unique_ptr<heavymach::GameSession>>(self.view,HMGameView.class,"_session");s->touch({470,160},1);bullets=std::max(bullets,s->bulletCount());}
 double begin=CACurrentMediaTime();[self.view frame:link];double cost=(CACurrentMediaTime()-begin)*1000;
 if(count>=30){
  wall.push_back((now-previousWall)*1000);display.push_back((link.timestamp-previousDisplay)*1000);work.push_back(cost);latency.push_back((now-link.timestamp)*1000);
  if((now-previousWall)*1000>25|| (link.timestamp-previousDisplay)*1000>25)
   [self.lateEvents addObject:@{@"frame":@(count),@"wall_gap_ms":@((now-previousWall)*1000),@"display_timestamp_gap_ms":@((link.timestamp-previousDisplay)*1000),@"previous_work_ms":@(previousWork),@"current_work_ms":@(cost),@"dispatch_after_timestamp_ms":@((now-link.timestamp)*1000)}];
 }
 previousWall=now;previousDisplay=link.timestamp;previousWork=cost;
 if(++count==390){[link invalidate];self.link=nil;[self.done fulfill];}
}
@end
@interface HMPresentationComparisonTests:XCTestCase
@end
@implementation HMPresentationComparisonTests
- (void)testPresentationDifferencesAndNoRenderControl {
 UIWindowScene* scene=nil;for(UIScene* s in UIApplication.sharedApplication.connectedScenes)if([s isKindOfClass:UIWindowScene.class]){scene=(UIWindowScene*)s;break;}
 XCTAssertNotNil(scene);if(!scene)return;
 UIWindow* old=scene.keyWindow;HMGameView* host=[old.rootViewController.view isKindOfClass:HMGameView.class]?(HMGameView*)old.rootViewController.view:nil;[host setApplicationActive:NO];
 NSArray* variants=@[@"baseline",@"full_viewport",@"linear_filter",@"full_plus_linear",@"scale1_load_probe",@"no_render_control"];
 NSString* filter=NSProcessInfo.processInfo.environment[@"HM_AB_VARIANTS"];if(filter.length)variants=[filter componentsSeparatedByString:@","];
 for(NSString* sceneName in @[@"slot_preview",@"stage01_firing"]){
  // Reverse the order in the second scene to expose simple run-order effects.
  NSArray* ordered=[sceneName isEqualToString:@"slot_preview"]?variants:variants.reverseObjectEnumerator.allObjects;
  for(NSString* variant in ordered){@autoreleasepool{
   HMComparisonView* view=[[HMComparisonView alloc]initWithFrame:scene.coordinateSpace.bounds];[[view valueForKey:@"frameTimer"]invalidate];[[view valueForKey:@"motionTimer"]invalidate];[(CMMotionManager*)[view valueForKey:@"motion"]stopAccelerometerUpdates];XCTAssertNil([view valueForKey:@"loadError"]);
   view.fullViewport=[variant isEqualToString:@"full_viewport"]||[variant isEqualToString:@"full_plus_linear"];
   view.linear=[variant isEqualToString:@"linear_filter"]||[variant isEqualToString:@"full_plus_linear"];
   view.skipFrame=[variant isEqualToString:@"no_render_control"];
   view.lowResolution=[variant isEqualToString:@"scale1_load_probe"];
   auto& save=storage<heavymach::SaveData>(view,HMGameView.class,"_save");save={};for(auto& slot:save.slots)heavymach::GlobalInfo(slot).selectSlot(1);[view setValue:@11 forKey:@"randomState"];
   if([sceneName isEqualToString:@"slot_preview"])[view refreshScreen:3];else [view startSlot:0 mode:0 mission:0 stage:0];
   UIWindow* window=[[UIWindow alloc]initWithWindowScene:scene];UIViewController* vc=[UIViewController new];vc.view=view;window.rootViewController=vc;[window makeKeyAndVisible];[view layoutIfNeeded];
   HMGLRenderer* gpu=[view valueForKey:@"glRenderer"];XCTAssertTrue([gpu beginFrame]);
   GLuint texture=storage<GLuint>(gpu,HMGLRenderer.class,"_sceneTexture");glBindTexture(GL_TEXTURE_2D,texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,view.linear?GL_LINEAR:GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,view.linear?GL_LINEAR:GL_NEAREST);[gpu endFrame];
   HMComparisonProbe* probe=[HMComparisonProbe new];probe.view=view;probe.lateEvents=[NSMutableArray new];probe.firing=[sceneName isEqualToString:@"stage01_firing"];probe.done=[self expectationWithDescription:variant];probe.link=[CADisplayLink displayLinkWithTarget:probe selector:@selector(step:)];probe.link.preferredFrameRateRange=CAFrameRateRangeMake(60,60,60);[probe.link addToRunLoop:NSRunLoop.mainRunLoop forMode:NSRunLoopCommonModes];
   [self waitForExpectations:@[probe.done]timeout:20];[probe.link invalidate];probe.link=nil;
   auto viewport=storage<heavymach::Viewport>(view,HMGameView.class,"_viewport");
   NSDictionary* result=@{@"variant":variant,@"scene":sceneName,@"wall_intervals":distribution(probe->wall),@"display_timestamp_intervals":distribution(probe->display),@"frame_work":distribution(probe->work),@"dispatch_after_timestamp":distribution(probe->latency),@"late_event_count":@(probe.lateEvents.count),@"late_events":[probe.lateEvents subarrayWithRange:NSMakeRange(0,MIN(probe.lateEvents.count,NSUInteger(8)))],@"drawable_width":[gpu valueForKey:@"displayWidth"],@"drawable_height":[gpu valueForKey:@"displayHeight"],@"viewport_points":@[@(viewport.x),@(viewport.y),@(480*viewport.scale),@(320*viewport.scale)],@"renderer":[gpu diagnostics],@"max_bullets":@(probe->bullets),@"audio":@"real audio enabled; no_render_control skips all frame work",@"scope":@"test-only one-variable presentation experiment; not complete compatibility-host reproduction"};
   NSData* json=[NSJSONSerialization dataWithJSONObject:result options:NSJSONWritingSortedKeys error:nil];NSLog(@"HM_PRESENT_AB %@",[[NSString alloc]initWithData:json encoding:NSUTF8StringEncoding]);
   XCTAssertEqual(probe->work.size(),size_t(360));XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");if(probe.firing&&!view.skipFrame)XCTAssertGreaterThan(probe->bullets,NSUInteger(0));
   [view setApplicationActive:NO];window.hidden=YES;window.rootViewController=nil;probe.view=nil;
  }}
 }
 [old makeKeyAndVisible];[host setApplicationActive:YES];
}
@end
