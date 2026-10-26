#import "HMPlatform.h"
#if TARGET_OS_OSX
@implementation NSImage (HMImageAccess)
- (CGImageRef)CGImage {return [self CGImageForProposedRect:nullptr context:nil hints:nil];}
+ (instancetype)imageWithCGImage:(CGImageRef)image {return [[self alloc] initWithCGImage:image size:NSMakeSize(CGImageGetWidth(image),CGImageGetHeight(image))];}
- (void)drawInRect:(CGRect)rect blendMode:(CGBlendMode)mode alpha:(CGFloat)alpha {
 CGContextRef ctx=HMCurrentGraphicsContext();if(!ctx)return;
 CGContextSaveGState(ctx);CGContextSetBlendMode(ctx,mode);CGContextSetAlpha(ctx,alpha);
 CGContextDrawImage(ctx,rect,self.CGImage);CGContextRestoreGState(ctx);
}
@end
#endif
