#import <XCTest/XCTest.h>
#import "../../Sources/iOS/HMMedia.h"
#import "../../Sources/iOS/HMGLRenderer.h"
#import "../../Sources/iOS/HMGameView.h"
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
@end
