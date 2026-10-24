#import <XCTest/XCTest.h>
#import "../../Sources/iOS/HMMedia.h"
#import "../../Sources/iOS/HMGLRenderer.h"
#import "../../Sources/iOS/HMGameView.h"
#import "../../Sources/iOS/HMGameHUD.h"
@interface HMGameView(ResourceLifecycle)
- (void)startSlot:(int)slot mode:(int)mode mission:(int)mission stage:(int)stage;
- (void)refreshScreen:(int)screen;
@end
@interface HMResourceView:HMGameView
@end
@implementation HMResourceView
- (void)persistSave {}
@end
@interface HMResourceLifecycleTests:XCTestCase
@end
@implementation HMResourceLifecycleTests
- (void)testScopedTexturesReleasePixelsAndDoNotDecodeAgainEachFrame {
 HMMedia* media=[HMMedia new];HMGLRenderer* gl=[HMGLRenderer new];media.textureRenderer=gl;
 CGSize shared=[media imageSize:@"Main_BG.png" scope:@"shared"];
 CGSize world=[media imageSize:@"Main_BG.png" scope:@"world"];
 XCTAssertTrue(CGSizeEqualToSize(shared,world));XCTAssertGreaterThan(shared.width,0);
 XCTAssertEqual([[gl diagnostics][@"textureCount"] intValue],2);
 XCTAssertEqual([[media cacheDiagnostics][@"decodedBytes"] intValue],0);
 XCTAssertEqual([[media cacheDiagnostics][@"decodedImages"] intValue],0);
 NSUInteger decodes=[[media cacheDiagnostics][@"decodeCount"] unsignedIntegerValue];
 media.resourceScope=@"world";media.glRenderer=gl;
 for(int i=0;i<20;++i){XCTAssertTrue([gl beginFrame]);[media draw:@"Main_BG.png" x:240 y:160];[gl endFrame];}
 XCTAssertEqual([[media cacheDiagnostics][@"decodeCount"] unsignedIntegerValue],decodes);
 [media releaseScope:@"world"];
 XCTAssertEqual([[gl diagnostics][@"textureCount"] intValue],1);
 XCTAssertEqual([[media cacheDiagnostics][@"metadataCount"] intValue],1);
 XCTAssertTrue([gl hasTexture:@"shared|Main_BG.png" rgb565:NO]);
 XCTAssertFalse([gl hasTexture:@"world|Main_BG.png" rgb565:NO]);
 [media imageSize:@"Main_BG.png" scope:@"world"];
 XCTAssertEqual([[media cacheDiagnostics][@"decodeCount"] unsignedIntegerValue],decodes+1);
 XCTAssertEqualObjects([gl diagnostics][@"lastError"],@"");
}
- (void)testRepeatedMapAndMenuTransitionsReleaseOnlyPrivateResources {
 HMResourceView* view=[[HMResourceView alloc]initWithFrame:CGRectMake(0,0,480,320)];[view setApplicationActive:NO];
 HMMedia* media=[view valueForKey:@"media"];HMGLRenderer* gl=[view valueForKey:@"glRenderer"];
 XCTAssertTrue([gl hasTexture:@"shared|Loading_Top.png" rgb565:NO]);
 NSUInteger previous=0;
 for(int i=0;i<6;++i){
  [view startSlot:0 mode:0 mission:0 stage:0];XCTAssertNil([view valueForKey:@"loadError"]);
  NSArray* worldKeys=[[media cacheDiagnostics][@"keys"] filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"SELF BEGINSWITH %@",@"world|"]];XCTAssertGreaterThan(worldKeys.count,NSUInteger(0));
  [view refreshScreen:3];
  NSArray* remaining=[[media cacheDiagnostics][@"keys"] filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"SELF BEGINSWITH %@",@"world|"]];XCTAssertEqual(remaining.count,NSUInteger(0));
  NSUInteger count=[[gl diagnostics][@"textureCount"] unsignedIntegerValue];if(i)XCTAssertEqual(count,previous);previous=count;
  XCTAssertEqual([[media cacheDiagnostics][@"decodedBytes"] unsignedIntegerValue],NSUInteger(0));
  XCTAssertTrue([gl hasTexture:@"shared|Loading_Top.png" rgb565:NO]);
 }
 XCTAssertEqualObjects([gl diagnostics][@"lastError"],@"");
}
// Exercise the real result-screen consumer, not just resource existence.
- (void)testResultScreensRenderOriginalJPEGIllustrations {
 HMMedia* media=[HMMedia new];HMGLRenderer* gl=[HMGLRenderer new];
 media.textureRenderer=gl;media.glRenderer=gl;
 HMGameHUD* hud=[[HMGameHUD alloc]initWithMedia:media];
 NSMutableArray<NSString*>* assets=[NSMutableArray arrayWithObject:@"StageClear.jpg"];
 for(int mission=1;mission<=7;++mission){
  [assets addObject:[NSString stringWithFormat:@"GameLoading_%02d.jpg",mission]];
  [assets addObject:[NSString stringWithFormat:@"GameLoading_%02dE.jpg",mission]];
 }
 auto pixels=[](UIImage* image){
  std::vector<uint8_t> data(480*320*4);auto color=CGColorSpaceCreateDeviceRGB();
  auto context=CGBitmapContextCreate(data.data(),480,320,8,480*4,color,CGBitmapInfo(kCGBitmapByteOrder32Big)|kCGImageAlphaPremultipliedLast);
  CGColorSpaceRelease(color);CGContextDrawImage(context,CGRectMake(0,0,480,320),image.CGImage);CGContextRelease(context);return data;
 };
 for(NSString* asset in assets){@autoreleasepool{
  // Original 0xb8f0/0xbf74: JPEG at (240,160). Sample away from typography/buttons.
  XCTAssertTrue([gl beginFrame]);[media draw:asset x:240 y:160];[gl finish];[gl endFrame];
  auto expected=pixels([gl snapshot]);
  heavymach::StagePresentation presentation;presentation.step=2;presentation.alpha=1;presentation.scale=1;presentation.rotation=0;
  presentation.loadingAsset=asset.UTF8String;presentation.missionEnd=[asset containsString:@"E.jpg"];
  presentation.typoFrame=[asset isEqualToString:@"StageClear.jpg"]?1:presentation.missionEnd?0:4;
  HMGameHUDSnapshot state;state.presentation=&presentation;state.paused=true;
  XCTAssertTrue([gl beginFrame]);[hud draw:state];[gl finish];[gl endFrame];UIImage* result=[gl snapshot];
  auto actual=pixels(result);size_t differences=0,lit=0;
  for(int y=90;y<230;++y)for(int x=40;x<440;++x)for(int c=0;c<3;++c){auto i=(y*480+x)*4+c;if(expected[i])++lit;if(actual[i]!=expected[i])++differences;}
  XCTAssertGreaterThan(lit,size_t(10000),@"Original illustration %@ must contain image pixels",asset);
  XCTAssertEqual(differences,size_t(0),@"Result screen lost original illustration %@",asset);
  if([asset isEqualToString:@"GameLoading_01E.jpg"]){XCTAttachment* capture=[XCTAttachment attachmentWithImage:result];capture.name=@"Mission 1 clear illustration";capture.lifetime=XCTAttachmentLifetimeKeepAlways;[self addAttachment:capture];}
  XCTAssertEqualObjects([gl diagnostics][@"lastError"],@"");
 }}
}
@end
