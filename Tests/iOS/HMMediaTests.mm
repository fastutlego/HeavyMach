#import <XCTest/XCTest.h>
#import "../../Sources/iOS/HMMedia.h"
#import "../../Sources/iOS/HMGLRenderer.h"
#include <array>
#include <vector>

// This is the actual HMMedia -> CPU raster -> UIImage/CG compositor bridge.
// Only resource loading is replaced. No app window or screenshot is used.
@interface HMTinyTextureMedia : HMMedia
@property(nonatomic,strong) UIImage *fixtureImage;
@end
@implementation HMTinyTextureMedia
- (UIImage *)image:(NSString *)name { return self.fixtureImage; }
@end

namespace {
struct Probe {int x,y,rgb;};
// Directly transcribed from build/render-reference/manifest.json probes for
// alpha_midpoint and alpha_vertex_additive (GL_REPLACE, Apple M4 Pro actual GL).
// Additional 8 edge values are from those cases' original .rgba readbacks.
constexpr std::array<Probe,13> probes{{
    {239,159,100},{240,160,64},{241,160,36},{244,160,0},{240,164,0},
    {232,160,64},{231,160,0},{247,160,36},{248,160,0},
    {240,156,64},{240,155,0},{240,163,64},{240,164,0}
}};
UIImage *tinyImage() {
    const uint8_t pixels[]={255,255,255,255,0,0,0,0};
    CFDataRef data=CFDataCreate(kCFAllocatorDefault,pixels,sizeof pixels);
    CGDataProviderRef provider=CGDataProviderCreateWithCFData(data);
    CGColorSpaceRef color=CGColorSpaceCreateDeviceRGB();
    CGImageRef image=CGImageCreate(2,1,8,32,8,color,
        CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(kCGImageAlphaPremultipliedLast),
        provider,nullptr,false,kCGRenderingIntentDefault);
    UIImage *result=image?[UIImage imageWithCGImage:image scale:1 orientation:UIImageOrientationUp]:nil;
    if(image)CGImageRelease(image);
    CGColorSpaceRelease(color);CGDataProviderRelease(provider);CFRelease(data);
    return result;
}
// Draw output into explicitly RGBA8 storage so renderer-specific BGRA layouts
// are not interpreted as RGBA. CGImage storage rows are top-to-bottom.
std::vector<uint8_t> readRGBA(UIImage *image) {
    std::vector<uint8_t> pixels(480*320*4);
    CGColorSpaceRef color=CGColorSpaceCreateDeviceRGB();
    CGContextRef bitmap=CGBitmapContextCreate(pixels.data(),480,320,8,480*4,color,
        CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(kCGImageAlphaPremultipliedLast));
    CGColorSpaceRelease(color);
    if(!bitmap)return {};
    CGContextSetBlendMode(bitmap,kCGBlendModeCopy);
    CGContextSetInterpolationQuality(bitmap,kCGInterpolationNone);
    CGContextDrawImage(bitmap,CGRectMake(0,0,480,320),image.CGImage);
    CGContextRelease(bitmap);
    return pixels;
}
}

@interface HMMediaTests : XCTestCase
@end
@implementation HMMediaTests
- (void)testOpenGLMatchesIndependentOriginalAlphaProbes {
 for(NSNumber* blend in @[@0,@2]) {
  HMTinyTextureMedia* media=[HMTinyTextureMedia new];media.fixtureImage=tinyImage();
  HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;
  XCTAssertTrue([gpu beginFrame]);media.glRenderer=gpu;
  [media setCameraOffset:CGPointMake(7,-3)];
  [media draw:@"tiny" x:247.5 y:157.5 columns:1 rows:1 tile:0 scaleX:8 scaleY:8 rotation:0 alpha:.5 blend:blend.integerValue];
  [gpu finish];media.glRenderer=nil;[gpu endFrame];
  auto pixels=readRGBA([gpu snapshot]);XCTAssertEqual(pixels.size(),size_t(480*320*4));
  for(auto p:probes)for(int c=0;c<3;++c)XCTAssertEqual(int(pixels[(size_t(319-p.y)*480+p.x)*4+c]),p.rgb,@"Original actual-GL probe at %d,%d",p.x,p.y);
  XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
 }
}

- (void)checkBlend:(NSInteger)blend alpha:(CGFloat)alpha offset:(CGPoint)offset {
    HMTinyTextureMedia *media=[HMTinyTextureMedia new];
    media.fixtureImage=tinyImage();
    XCTAssertNotNil(media.fixtureImage);
    [media setCameraOffset:offset];
    UIGraphicsImageRendererFormat *format=[UIGraphicsImageRendererFormat defaultFormat];
    format.scale=1;format.opaque=YES;format.preferredRange=UIGraphicsImageRendererFormatRangeStandard;
    UIGraphicsImageRenderer *renderer=[[UIGraphicsImageRenderer alloc] initWithSize:CGSizeMake(480,320) format:format];
    UIImage *frame=[renderer imageWithActions:^(UIGraphicsImageRendererContext *context) {
        CGContextSetRGBFillColor(context.CGContext,0,0,0,1);
        CGContextFillRect(context.CGContext,CGRectMake(0,0,480,320));
        [media draw:@"tiny-premult-rgba" x:240.5+offset.x y:160.5+offset.y
            columns:1 rows:1 tile:0 scaleX:8 scaleY:8 rotation:0 alpha:alpha blend:blend];
    }];
    XCTAssertNotNil(frame);
    XCTAssertEqual(CGImageGetWidth(frame.CGImage),size_t(480));
    XCTAssertEqual(CGImageGetHeight(frame.CGImage),size_t(320));
    auto pixels=readRGBA(frame);
    XCTAssertEqual(pixels.size(),size_t(480*320*4));
    if(pixels.size()!=480*320*4)return;
    for(auto p:probes) {
        // GL probe coordinates are +Y-up; CGImage output storage is top-down.
        size_t i=(size_t(319-p.y)*480+p.x)*4;
        for(int channel=0;channel<3;++channel)
            XCTAssertEqualWithAccuracy(int(pixels[i+channel]),p.rgb,1,
                @"GL probe (%d,%d), channel %d, blend %ld, alpha %.2f, offset (%.2f,%.2f)",
                p.x,p.y,channel,(long)blend,double(alpha),double(offset.x),double(offset.y));
        // Opaque native framebuffer deliberately does not reproduce GL alpha;
        // compare RGB contribution and require native opacity instead.
        XCTAssertEqual(pixels[i+3],uint8_t(255));
    }
}
- (void)testNormalMatchesActualGLMidpointAndEightEdges {
    [self checkBlend:0 alpha:1 offset:CGPointZero];
}
- (void)testAdditiveMatchesActualGLWithHalfVertexAlpha {
    [self checkBlend:2 alpha:.5 offset:CGPointZero];
}
- (void)testCameraOffsetAppliedBeforeRasterization {
    // Draw at (241,161) minus camera (.5,.5) reproduces the GL baseline.
    [self checkBlend:0 alpha:1 offset:CGPointMake(.5,.5)];
}
- (void)testZeroVertexAlphaKeepsRGBATextureVisible {
    // GL_REPLACE replaces incoming alpha. An alpha<=0 early return is wrong.
    [self checkBlend:0 alpha:0 offset:CGPointZero];
    [self checkBlend:2 alpha:0 offset:CGPointZero];
}
@end

#import "../../Sources/iOS/HMGameHUD.h"
#include "../JourneyUpgradeHost.hpp"

@interface HMUpgradeLifecycleTests : XCTestCase
@end
@implementation HMUpgradeLifecycleTests
- (void)testRepeatedGaugeEventsAdvanceWithoutDrawAndStaleDrawCannotResetThem {
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:nil];HMGameHUDSnapshot stale;
    [hud setValue:@1 forKey:@"hp"];[hud setHPGauge:.5f];[hud setReloadGauge:0];
    [hud tick:1];
    XCTAssertEqual([[hud valueForKey:@"hp"] floatValue],.9375f);
    XCTAssertEqual([[hud valueForKey:@"gear"] floatValue],.15f);
    [hud draw:stale];[hud setReloadGauge:0];[hud tick:2];
    XCTAssertEqual([[hud valueForKey:@"hp"] floatValue],.8828125f);
    XCTAssertEqual([[hud valueForKey:@"gear"] floatValue],.3f);
    XCTAssertEqualWithAccuracy([[hud valueForKey:@"gearVelocity"] floatValue],.1425f,.000001);
}
- (void)assertHUD:(HMGameHUD *)hud matches:(const JourneyUpgradeHost&)model {
    XCTAssertEqualWithAccuracy([[hud valueForKey:@"upgradeX"] floatValue],model.x,0.00001);
    XCTAssertEqual([[hud valueForKey:@"upgradeStep"] intValue],model.step);
    XCTAssertEqual([[hud valueForKey:@"upgradeVisible"] boolValue],model.visible);
}
- (void)testOpeningConfirmationAndClosingMatchRealHUDFrameOrder {
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:nil];
    __block int callbacks=0;hud.upgradeClosed=^{++callbacks;};
    JourneyUpgradeHost model;heavymach::SaveSlot slot;HMGameHUDSnapshot snapshot;snapshot.slot=&slot;
    [self assertHUD:hud matches:model];
    // World level-up event arrives before this same frame's GuiGame tick.
    [hud openUpgrade:slot];model.open();
    [hud tick:1.0/60];XCTAssertFalse(model.tick());
    [self assertHUD:hud matches:model];XCTAssertEqual(model.x,-144.f);
    // Deliberately stale snapshot must neither close nor reset the panel.
    [hud draw:snapshot];[self assertHUD:hud matches:model];
    int frame=1;
    while(!model.readyForConfirmation()&&frame<60){++frame;[hud tick:frame/60.0];XCTAssertFalse(model.tick());[self assertHUD:hud matches:model];}
    XCTAssertEqual(frame,28);XCTAssertTrue(model.readyForConfirmation());
    const float displayedX=model.x;
    // Input confirmation is delivered before the next World/HUD tick.
    [hud closeUpgrade];model.close();
    [self assertHUD:hud matches:model];XCTAssertEqual(model.x,displayedX);
    int closingFrames=0;
    while(model.step&&closingFrames<60){
        const float before=model.x;++frame;++closingFrames;
        [hud tick:frame/60.0];bool closed=model.tick();[self assertHUD:hud matches:model];
        if(closingFrames==1)XCTAssertEqualWithAccuracy(model.x,displayedX+(660-displayedX)/5,.00001);
        XCTAssertEqual(closed,model.x>659.f);XCTAssertEqual(callbacks,closed?1:0);
        if(closed){XCTAssertLessThanOrEqual(before,659.f);XCTAssertGreaterThan(model.x,659.f);}
        // Intermittent drawing cannot advance or delay the lifecycle.
        if(closingFrames%3==0){[hud draw:snapshot];[self assertHUD:hud matches:model];}
    }
    XCTAssertEqual(closingFrames,28);XCTAssertEqual(callbacks,1);
    for(int i=0;i<5;++i){[hud tick:++frame/60.0];XCTAssertFalse(model.tick());}
    [self assertHUD:hud matches:model];XCTAssertEqual(callbacks,1);
}
- (void)testGameStartDoesNotResetOpeningOrClosingHUD {
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:nil];JourneyUpgradeHost model;heavymach::SaveSlot slot;
    [hud openUpgrade:slot];model.open();
    for(int i=1;i<=5;++i){[hud tick:i/60.0];model.tick();}
    const float openX=model.x;[hud setGameStart:5];[self assertHUD:hud matches:model];XCTAssertEqual(model.x,openX);
    [hud closeUpgrade];model.close();[hud tick:6];model.tick();
    const float closeX=model.x;[hud setGameStart:7];[self assertHUD:hud matches:model];XCTAssertEqual(model.x,closeX);XCTAssertEqual(model.step,2);
}
- (void)testRepeatedOpenResetsEvenWithoutDrawOrVisibilityChange {
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:nil];JourneyUpgradeHost model;heavymach::SaveSlot slot;
    __block int callbacks=0;hud.upgradeClosed=^{++callbacks;};
    [hud openUpgrade:slot];model.open();
    for(int i=1;i<=4;++i){[hud tick:i/60.0];model.tick();}
    // Original setUpgrade removes/recreates an already-open panel too.
    [hud openUpgrade:slot];model.open();[self assertHUD:hud matches:model];XCTAssertEqual(model.x,-240.f);
    [hud tick:5.0/60];XCTAssertFalse(model.tick());[self assertHUD:hud matches:model];XCTAssertEqual(model.x,-144.f);
    [hud closeUpgrade];model.close();[hud tick:6.0/60];model.tick();
    [hud openUpgrade:slot];model.open();[self assertHUD:hud matches:model];XCTAssertEqual(model.x,-240.f);
    [hud tick:7.0/60];XCTAssertFalse(model.tick());
    [self assertHUD:hud matches:model];XCTAssertEqual(model.x,-144.f);XCTAssertEqual(callbacks,0);
}
@end
