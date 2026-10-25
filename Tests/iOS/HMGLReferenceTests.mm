// Independent actual CGL oracle: tools/original_gl_reference.mm.
// Expected samples extracted from existing build/render-reference/*.rgba;
// never generated from the new renderer. RGB error <=1 permits driver rounding.
#import <XCTest/XCTest.h>
#import "../../Sources/iOS/HMGLRenderer.h"
#import "../../Sources/iOS/HMMedia.h"
#import <OpenGLES/EAGL.h>
#include <limits>
#include <vector>
#include <algorithm>
namespace {
UIImage* fixture(int type){
 std::vector<uint8_t> p;int w,h;bool rgb=type==2;
 if(type==3){w=16;h=16;p.assign(w*h*4,255);}
 else if(type==0){w=2;h=1;p={255,255,255,255,0,0,0,0};}
 else if(type==1){w=3;h=3;p={255,0,0,255,0,128,0,128,0,0,64,64,255,255,0,255,128,0,128,128,0,255,255,255,64,64,64,64,255,255,255,255,0,0,0,0};}
 else{w=3;h=1;p={123,45,67,255,17,93,181,255,255,255,255,255};}
 NSData* data=[NSData dataWithBytes:p.data() length:p.size()];auto provider=CGDataProviderCreateWithCFData((__bridge CFDataRef)data);auto cs=CGColorSpaceCreateDeviceRGB();
 auto image=CGImageCreate(w,h,8,32,w*4,cs,CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(rgb?kCGImageAlphaNoneSkipLast:kCGImageAlphaPremultipliedLast),provider,nullptr,false,kCGRenderingIntentDefault);
 UIImage* result=[UIImage imageWithCGImage:image];CGImageRelease(image);CGColorSpaceRelease(cs);CGDataProviderRelease(provider);return result;
}
std::vector<uint8_t> bytes(UIImage* image){std::vector<uint8_t> p(480*320*4);auto cs=CGColorSpaceCreateDeviceRGB();auto ctx=CGBitmapContextCreate(p.data(),480,320,8,480*4,cs,CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(kCGImageAlphaPremultipliedLast));CGColorSpaceRelease(cs);CGContextSetBlendMode(ctx,kCGBlendModeCopy);CGContextDrawImage(ctx,CGRectMake(0,0,480,320),image.CGImage);CGContextRelease(ctx);return p;}
std::vector<uint8_t> canvasBytes(UIImage* image){
 const size_t w=CGImageGetWidth(image.CGImage),h=CGImageGetHeight(image.CGImage);
 std::vector<uint8_t> p(w*h*4);auto cs=CGColorSpaceCreateDeviceRGB();
 auto ctx=CGBitmapContextCreate(p.data(),w,h,8,w*4,cs,CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(kCGImageAlphaPremultipliedLast));CGColorSpaceRelease(cs);
 CGContextSetBlendMode(ctx,kCGBlendModeCopy);CGContextDrawImage(ctx,CGRectMake(0,0,w,h),image.CGImage);CGContextRelease(ctx);return p;
}
int redAt(const std::vector<uint8_t>& pixels,int width,int x,int y){return pixels[(size_t(319-y)*width+x)*4];}
struct Sample{int x,y,r,g,b;};
}
@interface HMCanvasFixtureMedia:HMMedia
@end
@implementation HMCanvasFixtureMedia
- (UIImage *)image:(NSString *)name {return fixture(3);}
@end
@interface HMGLReferenceTests:XCTestCase
@end
@implementation HMGLReferenceTests
// Oracle SHA256 2f453a61c375d1240d7197d736f915ad2b2cf08e841dd169551beccfe75761bb
- (void)test_alpha_midpoint {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(0);
 [gpu drawImage:im name:@"fixture" columns:1 rows:1 tile:0 position:CGPointMake(240.5,160.5) scale:CGSizeMake(8,8) rotation:0 alpha:1 blend:0 rgb565:NO];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,100,100,100},{240,160,64,64,64},{241,160,36,36,36},{244,160,0,0,0},{240,164,0,0,0},{232,156,64,64,64},{235,156,195,195,195},{238,156,143,143,143},{241,156,36,36,36},{245,156,4,4,4},{232,157,64,64,64},{235,157,195,195,195},{238,157,143,143,143},{241,157,36,36,36},{245,157,4,4,4},{232,158,64,64,64},{235,158,195,195,195},{238,158,143,143,143},{241,158,36,36,36},{245,158,4,4,4},{232,159,64,64,64},{235,159,195,195,195},{238,159,143,143,143},{241,159,36,36,36},{245,159,4,4,4},{232,160,64,64,64},{235,160,195,195,195},{238,160,143,143,143},{245,160,4,4,4},{232,161,64,64,64},{235,161,195,195,195},{238,161,143,143,143},{241,161,36,36,36},{245,161,4,4,4},{232,162,64,64,64},{235,162,195,195,195},{238,162,143,143,143},{241,162,36,36,36},{245,162,4,4,4},{232,163,64,64,64},{235,163,195,195,195},{238,163,143,143,143},{241,163,36,36,36},{245,163,4,4,4}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
// Oracle SHA256 648fb10c646ac1ac1e241e5a31e936f1ca8b4502bc4e1ee53edc3b9e2475511c
- (void)test_alpha_vertex_additive {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(0);
 [gpu drawImage:im name:@"fixture" columns:1 rows:1 tile:0 position:CGPointMake(240.5,160.5) scale:CGSizeMake(8,8) rotation:0 alpha:0.5 blend:2 rgb565:NO];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,100,100,100},{240,160,64,64,64},{241,160,36,36,36},{244,160,0,0,0},{240,164,0,0,0},{232,156,64,64,64},{235,156,195,195,195},{238,156,143,143,143},{241,156,36,36,36},{245,156,4,4,4},{232,157,64,64,64},{235,157,195,195,195},{238,157,143,143,143},{241,157,36,36,36},{245,157,4,4,4},{232,158,64,64,64},{235,158,195,195,195},{238,158,143,143,143},{241,158,36,36,36},{245,158,4,4,4},{232,159,64,64,64},{235,159,195,195,195},{238,159,143,143,143},{241,159,36,36,36},{245,159,4,4,4},{232,160,64,64,64},{235,160,195,195,195},{238,160,143,143,143},{245,160,4,4,4},{232,161,64,64,64},{235,161,195,195,195},{238,161,143,143,143},{241,161,36,36,36},{245,161,4,4,4},{232,162,64,64,64},{235,162,195,195,195},{238,162,143,143,143},{241,162,36,36,36},{245,162,4,4,4},{232,163,64,64,64},{235,163,195,195,195},{238,163,143,143,143},{241,163,36,36,36},{245,163,4,4,4}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
// Oracle SHA256 5171a77fafdbc876df1eac5e9cb658ec2d5e69f1c13a8019d24f0e18596a5ff7
- (void)test_atlas_neighbor_padding {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(1);
 [gpu drawImage:im name:@"fixture" columns:2 rows:2 tile:0 position:CGPointMake(240.25,160.75) scale:CGSizeMake(12,9) rotation:0 alpha:1 blend:0 rgb565:NO];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,196,87,8},{240,160,170,66,8},{241,160,146,65,10},{244,160,85,59,15},{240,164,112,21,0},{231,154,69,65,0},{238,154,228,205,12},{245,154,107,53,55},{234,155,151,126,0},{241,155,166,119,31},{248,155,62,18,53},{237,156,249,180,2},{244,156,109,66,39},{233,157,121,73,0},{240,157,179,110,18},{247,157,59,37,38},{236,158,224,112,0},{243,158,115,70,25},{232,159,93,37,0},{246,159,60,51,24},{235,160,187,52,0},{242,160,124,63,12},{231,161,69,12,0},{238,161,218,47,2},{245,161,62,57,10},{234,162,151,8,0},{241,162,137,43,2},{248,162,12,61,3},{237,163,221,2,0},{244,163,63,48,0},{233,164,83,0,0},{247,164,15,44,0},{236,165,117,0,0},{243,165,47,26,0},{232,166,35,0,0},{239,166,70,8,0},{246,166,13,23,0}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
// Oracle SHA256 48df75590ab250f27c83fe61854be27f973d5fd6b393d46e85f8244a24302816
- (void)test_atlas_flip_rotate {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(1);
 [gpu drawImage:im name:@"fixture" columns:2 rows:2 tile:3 position:CGPointMake(240.25,160.75) scale:CGSizeMake(-9,6) rotation:0.3700000047683716 alpha:0.6499999761581421 blend:0 rgb565:NO];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,16,17,18},{240,160,27,41,44},{241,160,44,58,64},{244,160,100,96,119},{240,164,6,171,176},{242,155,45,45,45},{239,156,15,15,15},{242,156,64,64,64},{237,157,3,3,3},{240,157,32,32,32},{243,157,119,119,119},{237,158,2,2,2},{240,158,36,36,36},{243,158,131,130,132},{238,159,5,5,5},{241,159,53,57,60},{244,159,127,121,137},{236,160,0,1,1},{239,160,14,24,26},{242,160,62,73,84},{245,160,117,101,132},{234,161,0,2,2},{237,161,0,14,14},{240,161,23,53,58},{243,161,66,81,103},{246,161,102,73,118},{235,162,0,12,12},{238,162,0,49,49},{241,162,30,83,95},{244,162,66,80,115},{247,162,84,40,97},{236,163,0,34,34},{239,163,1,107,107},{242,163,36,110,133},{245,163,61,69,118},{235,164,0,30,30},{238,164,0,105,105},{241,164,18,163,177},{236,165,0,59,59},{239,165,0,184,184}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
// Oracle SHA256 0168bfb052daaafc03668e157ad4aa58121426387169310cef1e2069b47c18c1
- (void)test_atlas_minification {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(1);
 [gpu drawImage:im name:@"fixture" columns:1 rows:1 tile:0 position:CGPointMake(240.5,160.5) scale:CGSizeMake(0.699999988079071,0.699999988079071) rotation:0.20000000298023224 alpha:1 blend:0 rgb565:NO];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,0,0,0},{240,160,64,0,64},{241,160,0,47,47},{244,160,0,0,0},{240,164,0,0,0},{240,159,57,57,57},{239,160,92,66,0},{240,161,0,14,3}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
// Oracle SHA256 b58a2d19bee09a1c713b3e1650ba8e2bee9be6a8ec6a305290567e55db6e91e8
- (void)test_rgb565_padding {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(2);
 [gpu drawImage:im name:@"fixture" columns:1 rows:1 tile:0 position:CGPointMake(240.5,160.5) scale:CGSizeMake(8,8) rotation:0 alpha:1 blend:0 rgb565:YES];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,30,87,167},{240,160,16,93,181},{241,160,46,113,190},{244,160,136,174,218},{240,164,0,0,0},{228,156,62,22,33},{234,156,97,57,95},{240,156,16,93,181},{246,156,195,215,236},{228,157,62,22,33},{234,157,97,57,95},{240,157,16,93,181},{246,157,195,215,236},{228,158,62,22,33},{234,158,97,57,95},{240,158,16,93,181},{246,158,195,215,236},{228,159,62,22,33},{234,159,97,57,95},{240,159,16,93,181},{246,159,195,215,236},{228,160,62,22,33},{234,160,97,57,95},{246,160,195,215,236},{228,161,62,22,33},{234,161,97,57,95},{240,161,16,93,181},{246,161,195,215,236},{228,162,62,22,33},{234,162,97,57,95},{240,162,16,93,181},{246,162,195,215,236},{228,163,62,22,33},{234,163,97,57,95},{240,163,16,93,181},{246,163,195,215,236}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
// Oracle SHA256 dcd4e51d9534d32b6412bb0c2276c66d2a3e5d82fe09032c0b0a3a1cf5cb9306
- (void)test_rgb565_vertex_alpha {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(2);
 [gpu drawImage:im name:@"fixture" columns:1 rows:1 tile:0 position:CGPointMake(240.5,160.5) scale:CGSizeMake(8,8) rotation:0 alpha:0.5 blend:0 rgb565:YES];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,15,44,83},{240,160,8,47,90},{241,160,23,57,95},{244,160,68,87,109},{240,164,0,0,0},{228,156,31,11,16},{234,156,48,28,47},{240,156,8,47,90},{246,156,98,107,118},{228,157,31,11,16},{234,157,48,28,47},{240,157,8,47,90},{246,157,98,107,118},{228,158,31,11,16},{234,158,48,28,47},{240,158,8,47,90},{246,158,98,107,118},{228,159,31,11,16},{234,159,48,28,47},{240,159,8,47,90},{246,159,98,107,118},{228,160,31,11,16},{234,160,48,28,47},{246,160,98,107,118},{228,161,31,11,16},{234,161,48,28,47},{240,161,8,47,90},{246,161,98,107,118},{228,162,31,11,16},{234,162,48,28,47},{240,162,8,47,90},{246,162,98,107,118},{228,163,31,11,16},{234,163,48,28,47},{240,163,8,47,90},{246,163,98,107,118}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
// Oracle SHA256 2f453a61c375d1240d7197d736f915ad2b2cf08e841dd169551beccfe75761bb
- (void)test_trail_rgba_passthrough {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu beginFrame]);
 UIImage* im=fixture(0);
 heavymach::TrailMesh mesh;mesh.tile="fixture";mesh.translation={240.5f,160.5f};
 for(int i=0;i<4;++i){heavymach::TrailVertex v;v.position={float(i%2?8:-8),float(i/2?4:-4)};v.uv={float(i%2),float(i/2?0:1)};v.color={.2f+.2f*i,.8f-.2f*i,.3f,.1f+.2f*i};mesh.vertices.push_back(v);}
 [gpu drawTrail:mesh image:im name:@"fixture" rgb565:NO cameraOffset:CGPointZero];
 [gpu finish];[gpu endFrame];auto actual=bytes([gpu snapshot]);
 const Sample samples[]={{239,159,100,100,100},{240,160,64,64,64},{241,160,36,36,36},{244,160,0,0,0},{240,164,0,0,0},{232,156,64,64,64},{235,156,195,195,195},{238,156,143,143,143},{241,156,36,36,36},{245,156,4,4,4},{232,157,64,64,64},{235,157,195,195,195},{238,157,143,143,143},{241,157,36,36,36},{245,157,4,4,4},{232,158,64,64,64},{235,158,195,195,195},{238,158,143,143,143},{241,158,36,36,36},{245,158,4,4,4},{232,159,64,64,64},{235,159,195,195,195},{238,159,143,143,143},{241,159,36,36,36},{245,159,4,4,4},{232,160,64,64,64},{235,160,195,195,195},{238,160,143,143,143},{245,160,4,4,4},{232,161,64,64,64},{235,161,195,195,195},{238,161,143,143,143},{241,161,36,36,36},{245,161,4,4,4},{232,162,64,64,64},{235,162,195,195,195},{238,162,143,143,143},{241,162,36,36,36},{245,162,4,4,4},{232,163,64,64,64},{235,163,195,195,195},{238,163,143,143,143},{241,163,36,36,36},{245,163,4,4,4}};
 for(auto p:samples){int expected[]={p.r,p.g,p.b};for(int c=0;c<3;++c)XCTAssertLessThanOrEqual(abs(int(actual[(size_t(319-p.y)*480+p.x)*4+c])-expected[c]),1,@"Original GL sample %d,%d channel%d",p.x,p.y,c);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
- (void)testWideCanvasPreservesCentrePixelsAndShowsBothNewEdges {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;
 UIImage* im=fixture(3);
 XCTAssertTrue([gpu beginFrame]);
 [gpu drawImage:im name:@"solid" columns:1 rows:1 tile:0 position:CGPointMake(240,160) scale:CGSizeMake(1,1) rotation:0 alpha:1 blend:0 rgb565:NO];
 [gpu endFrame];auto original=canvasBytes([gpu snapshot]);
 XCTAssertTrue([gpu setLogicalWidth:640 height:320]);XCTAssertTrue([gpu beginFrame]);
 for(double x:{20.,320.,620.})[gpu drawImage:im name:@"solid" columns:1 rows:1 tile:0 position:CGPointMake(x,160) scale:CGSizeMake(1,1) rotation:0 alpha:1 blend:0 rgb565:NO];
 [gpu endFrame];UIImage* shot=[gpu snapshot];XCTAssertEqual(CGImageGetWidth(shot.CGImage),640u);XCTAssertEqual(CGImageGetHeight(shot.CGImage),320u);auto wide=canvasBytes(shot);
 for(int y=0;y<320;++y)XCTAssertTrue(std::equal(original.begin()+size_t(y)*480*4,original.begin()+size_t(y+1)*480*4,wide.begin()+(size_t(y)*640+80)*4),@"Original centre row %d",y);
 XCTAssertEqual(redAt(wide,640,20,160),255);XCTAssertEqual(redAt(wide,640,620,160),255);
 XCTAssertEqualObjects([gpu diagnostics][@"textureUploads"],@1);XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
- (void)testFractionalCanvasResizeReusesTexturesAndPreservesExternalContext {
 EAGLContext* previous=[EAGLContext currentContext];EAGLContext* external=[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
 XCTAssertTrue([EAGLContext setCurrentContext:external]);
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu){[EAGLContext setCurrentContext:previous];return;}
 XCTAssertEqual([EAGLContext currentContext],external);
 XCTAssertTrue([gpu prepareImage:fixture(3) name:@"solid" rgb565:NO]);
 XCTAssertTrue([gpu setLogicalWidth:693.333333 height:320]);
 XCTAssertEqual([EAGLContext currentContext],external);
 auto diagnostics=[gpu diagnostics];XCTAssertEqualWithAccuracy([diagnostics[@"logicalWidth"] doubleValue],693.333333,.000001);XCTAssertEqualObjects(diagnostics[@"bufferWidth"],@694);
 NSNumber* allocations=diagnostics[@"sceneReallocations"];
 for(int i=0;i<20;++i){XCTAssertTrue([gpu setLogicalWidth:693.5 height:320]);XCTAssertTrue([gpu beginFrame]);XCTAssertFalse([gpu setLogicalWidth:640 height:320]);[gpu endFrame];XCTAssertEqual([EAGLContext currentContext],external);}
 XCTAssertEqualObjects([gpu diagnostics][@"sceneReallocations"],allocations);
 XCTAssertTrue([gpu hasTexture:@"solid" rgb565:NO]);XCTAssertEqualObjects([gpu diagnostics][@"textureUploads"],@1);
 for(double invalid:{0.,-1.,1e12,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()})XCTAssertFalse([gpu setLogicalWidth:invalid height:320]);
 XCTAssertEqualObjects([gpu diagnostics][@"bufferWidth"],@694);
 XCTAssertEqual([EAGLContext currentContext],external);
 UIImage* shot=[gpu snapshot];XCTAssertEqual(CGImageGetWidth(shot.CGImage),694u);XCTAssertEqual([EAGLContext currentContext],external);
 XCTAssertTrue([gpu setLogicalWidth:480 height:320]);XCTAssertEqualObjects([gpu diagnostics][@"sceneReallocations"],@([allocations unsignedIntegerValue]+1));
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");[EAGLContext setCurrentContext:previous];
}
- (void)testScaledOffsetHUDClipMatchesSpriteAndTrail {
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu setLogicalWidth:640 height:320]);
 [gpu setDrawingOffset:CGPointMake(100,20)];[gpu setDrawingScale:.5];[gpu setClipRect:CGRectMake(220,92,12,16)];
 XCTAssertTrue([gpu beginFrame]);
 [gpu drawImage:fixture(3) name:@"solid" columns:1 rows:1 tile:0 position:CGPointMake(240,160) scale:CGSizeMake(4,4) rotation:0 alpha:1 blend:0 rgb565:NO];
 [gpu endFrame];auto sprite=canvasBytes([gpu snapshot]);
 XCTAssertEqual(redAt(sprite,640,219,100),0);XCTAssertEqual(redAt(sprite,640,220,100),255);XCTAssertEqual(redAt(sprite,640,231,100),255);XCTAssertEqual(redAt(sprite,640,232,100),0);XCTAssertEqual(redAt(sprite,640,224,91),0);XCTAssertEqual(redAt(sprite,640,224,108),0);
 heavymach::TrailMesh mesh;mesh.translation={250,165};
 for(int i=0;i<4;++i){heavymach::TrailVertex v;v.position={float(i%2?32:-32),float(i/2?32:-32)};v.uv={float(i%2),float(i/2?0:1)};v.color={1,1,1,1};mesh.vertices.push_back(v);}
 XCTAssertTrue([gpu beginFrame]);[gpu drawTrail:mesh image:nil name:@"solid" rgb565:NO cameraOffset:CGPointMake(10,5)];[gpu endFrame];auto trail=canvasBytes([gpu snapshot]);XCTAssertTrue(sprite==trail);
 [gpu resetClip];XCTAssertTrue([gpu beginFrame]);[gpu drawTrail:mesh image:nil name:@"solid" rgb565:NO cameraOffset:CGPointMake(10,5)];[gpu endFrame];auto unclipped=canvasBytes([gpu snapshot]);XCTAssertEqual(redAt(unclipped,640,210,100),255);
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
- (void)testMediaTransformsAndClipsCPUAndGPUHUD {
 HMCanvasFixtureMedia* media=[HMCanvasFixtureMedia new];
 [media setDrawingOffset:CGPointMake(100,20)];[media setDrawingScale:.5];[media setClipRect:CGRectMake(220,92,12,16)];
 UIGraphicsBeginImageContextWithOptions(CGSizeMake(640,320),YES,1);[[UIColor blackColor] setFill];UIRectFill(CGRectMake(0,0,640,320));
 [media draw:@"solid" x:240 y:160 columns:1 rows:1 tile:0 scaleX:4 scaleY:4 rotation:0 alpha:1];
 auto cpu=canvasBytes(UIGraphicsGetImageFromCurrentImageContext());UIGraphicsEndImageContext();
 HMGLRenderer* gpu=[HMGLRenderer new];XCTAssertNotNil(gpu);if(!gpu)return;XCTAssertTrue([gpu setLogicalWidth:640 height:320]);
 media.glRenderer=gpu;XCTAssertTrue([gpu beginFrame]);[media draw:@"solid" x:240 y:160 columns:1 rows:1 tile:0 scaleX:4 scaleY:4 rotation:0 alpha:1];[gpu endFrame];auto gl=canvasBytes([gpu snapshot]);
 for(int y=90;y<110;++y)for(int x=218;x<234;++x){int expected=(x>=220&&x<232&&y>=92&&y<108)?255:0;XCTAssertEqual(redAt(cpu,640,x,y),expected);XCTAssertEqual(redAt(gl,640,x,y),expected);}
 XCTAssertEqualObjects([gpu diagnostics][@"lastError"],@"");
}
@end
