#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#include "../Core/Trail.hpp"

// Owns an ES 1.1 context. Use on one thread; bracket drawing with begin/endFrame.
@interface HMGLRenderer : NSObject
- (BOOL)beginFrame;
- (BOOL)prepareImage:(UIImage *)image name:(NSString *)name rgb565:(BOOL)rgb565;
- (BOOL)hasTexture:(NSString *)name rgb565:(BOOL)rgb565;
- (void)releaseTextureScope:(NSString *)scope;
- (void)endFrame;
- (void)drawImage:(UIImage *)image name:(NSString *)name columns:(NSInteger)columns rows:(NSInteger)rows tile:(NSInteger)tile position:(CGPoint)position scale:(CGSize)scale rotation:(CGFloat)rotation alpha:(CGFloat)alpha blend:(NSInteger)blend rgb565:(BOOL)rgb565;
- (void)drawTrail:(const heavymach::TrailMesh&)mesh image:(UIImage *)image name:(NSString *)name rgb565:(BOOL)rgb565 cameraOffset:(CGPoint)offset;
// viewport uses UIKit points measured in layer.bounds, with a top-left origin.
- (BOOL)presentToLayer:(CAEAGLLayer *)layer viewport:(CGRect)viewport;
// Diagnostic readback only; never required by the presentation path.
- (UIImage *)snapshot;
- (void)finish;
- (NSDictionary *)diagnostics;
@end
