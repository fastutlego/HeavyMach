#import <UIKit/UIKit.h>
#include "../Core/Trail.hpp"
@class HMGLRenderer, HMOpenALAudio;
// Exact touchHLE decoder output: top-down premultiplied RGBA8, width*4 stride.
// Nil for images not loaded through HMMedia. Dimensions are image.CGImage dimensions.
FOUNDATION_EXPORT NSData *HMOriginalImageRGBA(UIImage *image);
@interface HMMedia : NSObject
// Non-nil only while submitting a GPU frame. Nil retains the CPU comparison path.
@property(nonatomic,strong) HMGLRenderer *glRenderer;
@property(nonatomic,weak) HMGLRenderer *textureRenderer;
@property(nonatomic,copy) NSString *resourceScope;
- (CGSize)imageSize:(NSString *)name;
- (CGSize)imageSize:(NSString *)name scope:(NSString *)scope;
- (void)releaseScope:(NSString *)scope;
- (NSDictionary *)cacheDiagnostics;
@property(nonatomic,strong,readonly) HMOpenALAudio *audio;
- (void)setCameraOffset:(CGPoint)offset;
- (void)setDrawingOffset:(CGPoint)offset;
- (void)setDrawingScale:(CGFloat)scale;
// Absolute y-up canvas clip; CPU raster remains a 480x320 comparison path.
- (void)setClipRect:(CGRect)rect;
- (void)resetClip;
- (void)drawEdgeContinuation:(NSString*)name x:(CGFloat)x y:(CGFloat)y scale:(CGFloat)scale rotation:(CGFloat)rotation width:(CGFloat)width;
- (void)drawJoinedBackground:(NSString*)name x:(CGFloat)x y:(CGFloat)y scale:(CGFloat)scale rotation:(CGFloat)rotation;
- (void)drawTrail:(const heavymach::TrailMesh&)mesh;
- (UIImage *)image:(NSString *)name;
// Called during init, before the first title frame. Repeated calls reuse voices.
- (BOOL)prepareOriginalSounds;
- (void)sound:(NSString *)name;
- (void)sound:(NSString *)name pan:(float)pan;
- (void)draw:(NSString *)name x:(CGFloat)x y:(CGFloat)y;
- (void)draw:(NSString *)name x:(CGFloat)x y:(CGFloat)y columns:(NSInteger)columns rows:(NSInteger)rows tile:(NSInteger)tile scaleX:(CGFloat)sx scaleY:(CGFloat)sy rotation:(CGFloat)r alpha:(CGFloat)a;
- (void)draw:(NSString *)name x:(CGFloat)x y:(CGFloat)y columns:(NSInteger)columns rows:(NSInteger)rows tile:(NSInteger)tile scaleX:(CGFloat)sx scaleY:(CGFloat)sy rotation:(CGFloat)r alpha:(CGFloat)a blend:(NSInteger)blend;
@end
