#pragma once
#import <TargetConditionals.h>
#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <QuartzCore/QuartzCore.h>
#if TARGET_OS_OSX
#import <AppKit/AppKit.h>
static inline NSColor *HMBlackColor(){return NSColor.blackColor;}
static inline NSColor *HMWhiteColor(){return NSColor.whiteColor;}
static inline NSFont *HMSystemFont(CGFloat size){return [NSFont systemFontOfSize:size];}
typedef NSImage HMImage;
typedef NSView HMNativeGameView;
@interface NSImage (HMImageAccess)
@property(nonatomic,readonly) CGImageRef CGImage;
+ (instancetype)imageWithCGImage:(CGImageRef)image;
- (void)drawInRect:(CGRect)rect blendMode:(CGBlendMode)mode alpha:(CGFloat)alpha;
@end
static inline CGContextRef HMCurrentGraphicsContext(void){return NSGraphicsContext.currentContext.CGContext;}
#else
#import <UIKit/UIKit.h>
static inline UIColor *HMBlackColor(){return UIColor.blackColor;}
static inline UIColor *HMWhiteColor(){return UIColor.whiteColor;}
static inline UIFont *HMSystemFont(CGFloat size){return [UIFont systemFontOfSize:size];}
typedef UIImage HMImage;
typedef UIView HMNativeGameView;
static inline CGContextRef HMCurrentGraphicsContext(void){return UIGraphicsGetCurrentContext();}
#endif

static inline NSValue *HMSizeValue(CGSize size){return [NSValue valueWithBytes:&size objCType:@encode(CGSize)];}
static inline CGSize HMValueSize(NSValue *value){CGSize size=CGSizeZero;[value getValue:&size size:sizeof(size)];return size;}
