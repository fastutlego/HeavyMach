#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
#include "../Core/Trail.hpp"

// Owns an ES 1.1 context. Use on one thread; bracket drawing with begin/endFrame.
@interface HMGLRenderer : NSObject
// Resize between frames. Invalid/unsupported sizes leave the previous canvas intact.
- (BOOL)setLogicalWidth:(CGFloat)width height:(CGFloat)height;
// Final drawing transform: local * scale + offset. Clip uses absolute y-up canvas coordinates.
- (void)setDrawingOffset:(CGPoint)offset;
- (void)setDrawingScale:(CGFloat)scale;
- (void)setClipRect:(CGRect)rect;
- (void)resetClip;
- (BOOL)beginFrame;
- (BOOL)prepareImage:(UIImage *)image name:(NSString *)name rgb565:(BOOL)rgb565;
- (BOOL)hasTexture:(NSString *)name rgb565:(BOOL)rgb565;
- (void)releaseTextureScope:(NSString *)scope;
- (void)endFrame;
- (void)drawImage:(UIImage *)image name:(NSString *)name columns:(NSInteger)columns rows:(NSInteger)rows tile:(NSInteger)tile position:(CGPoint)position scale:(CGSize)scale rotation:(CGFloat)rotation alpha:(CGFloat)alpha blend:(NSInteger)blend rgb565:(BOOL)rgb565;
// Presentation-only seam: reflect a narrow edge OUTSIDE the original rectangle
// and fade toward the generated scenery. Original pixels/rectangle stay untouched.
- (void)drawEdgeContinuation:(NSString*)name position:(CGPoint)position scale:(CGFloat)scale rotation:(CGFloat)rotation width:(CGFloat)width rgb565:(BOOL)rgb565;
// Background joins only: clamp the half-texel border to image pixels without
// changing interior UVs, original textures, or the default sprite sampler.
- (void)drawJoinedBackground:(NSString*)name position:(CGPoint)position scale:(CGFloat)scale rotation:(CGFloat)rotation rgb565:(BOOL)rgb565;
- (void)drawTrail:(const heavymach::TrailMesh&)mesh image:(UIImage *)image name:(NSString *)name rgb565:(BOOL)rgb565 cameraOffset:(CGPoint)offset;
// viewport uses UIKit points measured in layer.bounds, with a top-left origin.
- (BOOL)presentToLayer:(CAEAGLLayer *)layer viewport:(CGRect)viewport;
// Diagnostic readback only; never required by the presentation path.
- (UIImage *)snapshot;
- (void)finish;
- (NSDictionary *)diagnostics;
@end
