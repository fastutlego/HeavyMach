// Generated from original ARMv6 Objective-C metadata. Declarations only; no implementations.
#pragma once

#import "OriginalTypes.h"

// Original class 0x273e0; ARM32 instance size 16.
@interface HeavyMachAppDelegate : NSObject {
@public
    UIWindow * window; // ARM32 offset 4, size 4.
    EAGLView * glView; // ARM32 offset 8, size 4.
    NSTimer * _timer; // ARM32 offset 12, size 4.
}
// Original 0x234c (0x234d raw IMP); v12@0:4@8
- (void)setWindow:(id)arg0;
// Original 0x2340 (0x2341 raw IMP); @8@0:4
- (id)window;
// Original 0x2320 (0x2321 raw IMP); v12@0:4@8
- (void)setGlView:(id)arg0;
// Original 0x2314 (0x2315 raw IMP); @8@0:4
- (id)glView;
// Original 0x22c8 (0x22c9 raw IMP); v20@0:4{CGPoint=ff}8i16
- (void)onTap:(CGPoint)arg0 State:(int32_t)arg1;
// Original 0x2260 (0x2261 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x225e (0x225f raw IMP); v12@0:4@8
- (void)applicationDidBecomeActive:(id)arg0;
// Original 0x225c (0x225d raw IMP); v12@0:4@8
- (void)applicationWillResignActive:(id)arg0;
// Original 0x21e0 (0x21e1 raw IMP); v8@0:4
- (void)renderScene;
// Original 0x20b8 (0x20b9 raw IMP); v12@0:4@8
- (void)applicationDidFinishLaunching:(id)arg0;
@end

// Original class 0x27574; ARM32 instance size 92.
@interface EAGLView : UIView {
@public
    int32_t _backingWidth; // ARM32 offset 32, size 4.
    int32_t _backingHeight; // ARM32 offset 36, size 4.
    EAGLContext * _context; // ARM32 offset 40, size 4.
    NSString * _format; // ARM32 offset 44, size 4.
    uint32_t _viewRenderbuffer; // ARM32 offset 48, size 4.
    uint32_t _viewFramebuffer; // ARM32 offset 52, size 4.
    uint32_t _depthRenderbuffer; // ARM32 offset 56, size 4.
    uint32_t _depthFormat; // ARM32 offset 60, size 4.
    CGSize _size; // ARM32 offset 64, size 8.
    int8_t _autoresize; // ARM32 offset 72, size 1.
    int8_t _hasBeenCurrent; // ARM32 offset 73, size 1.
    id<EAGLViewDelegate> _delegate; // ARM32 offset 76, size 4.
    NSTimer * animationTimer; // ARM32 offset 80, size 4.
    double animationInterval; // ARM32 offset 84, size 8.
}
// Original 0x2ec0 (0x2ec1 raw IMP); v12@0:4@8
- (void)setDelegate:(id)arg0;
// Original 0x2eb4 (0x2eb5 raw IMP); @8@0:4
- (id)delegate;
// Original 0x2ea8 (0x2ea9 raw IMP); v12@0:4c8
- (void)setAutoresizesSurface:(int8_t)arg0;
// Original 0x2e9c (0x2e9d raw IMP); c8@0:4
- (int8_t)autoresizesSurface;
// Original 0x2e88 (0x2e89 raw IMP); {CGSize=ff}8@0:4
- (CGSize)surfaceSize;
// Original 0x2e7c (0x2e7d raw IMP); I8@0:4
- (uint32_t)framebuffer;
// Original 0x2e70 (0x2e71 raw IMP); @8@0:4
- (id)pixelFormat;
// Original 0x2e64 (0x2e65 raw IMP); I8@0:4
- (uint32_t)depthFormat;
// Original 0x2e58 (0x2e59 raw IMP); @8@0:4
- (id)context;
// Original 0x2d44 (0x2d45 raw IMP); v16@0:4@8@12
- (void)touchesEnded:(id)arg0 withEvent:(id)arg1;
// Original 0x2c30 (0x2c31 raw IMP); v16@0:4@8@12
- (void)touchesMoved:(id)arg0 withEvent:(id)arg1;
// Original 0x2b1c (0x2b1d raw IMP); v16@0:4@8@12
- (void)touchesBegan:(id)arg0 withEvent:(id)arg1;
// Original 0x2a54 (0x2a55 raw IMP); {CGRect={CGPoint=ff}{CGSize=ff}}24@0:4{CGRect={CGPoint=ff}{CGSize=ff}}8
- (CGRect)convertRectFromViewToSurface:(CGRect)arg0;
// Original 0x29c8 (0x29c9 raw IMP); {CGPoint=ff}16@0:4{CGPoint=ff}8
- (CGPoint)convertPointFromViewToSurface:(CGPoint)arg0;
// Original 0x2920 (0x2921 raw IMP); v8@0:4
- (void)swapBuffers;
// Original 0x28ec (0x28ed raw IMP); v8@0:4
- (void)clearCurrentContext;
// Original 0x28bc (0x28bd raw IMP); c8@0:4
- (int8_t)isCurrentContext;
// Original 0x287c (0x287d raw IMP); v8@0:4
- (void)setCurrentContext;
// Original 0x2858 (0x2859 raw IMP); v12@0:4c8
- (void)setAutoresizesEAGLSurface:(int8_t)arg0;
// Original 0x27dc (0x27dd raw IMP); v8@0:4
- (void)layoutSubviews;
// Original 0x278c (0x278d raw IMP); v8@0:4
- (void)dealloc;
// Original 0x2658 (0x2659 raw IMP); @12@0:4@8
- (id)initWithCoder:(id)arg0;
// Original 0x25a0 (0x25a1 raw IMP); v8@0:4
- (void)_destroySurface;
// Original 0x2388 (0x2389 raw IMP); c8@0:4
- (int8_t)_createSurface;
// Original 0x236c (0x236d raw IMP); #8@0:4
+ (Class)layerClass;
@end

// Original class 0x28148; ARM32 instance size 12.
@interface Collider : NSObject {
@public
    Vector2 * _ABMin; // ARM32 offset 4, size 4.
    Vector2 * _ABMax; // ARM32 offset 8, size 4.
}
// Original 0x6324 (0x6325 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x27f44; ARM32 instance size 28.
@interface ColVector : NSObject {
@public
    Vector2 * _origin; // ARM32 offset 4, size 4.
    Vector2 * _dir; // ARM32 offset 8, size 4.
    Vector2 * _normal; // ARM32 offset 12, size 4.
    float _length; // ARM32 offset 16, size 4.
    ColVector * _prev; // ARM32 offset 20, size 4.
    ColVector * _next; // ARM32 offset 24, size 4.
}
// Original 0x6318 (0x6319 raw IMP); @8@0:4
- (id)origin;
// Original 0x630c (0x630d raw IMP); @8@0:4
- (id)dir;
// Original 0x6300 (0x6301 raw IMP); @8@0:4
- (id)normal;
// Original 0x62f4 (0x62f5 raw IMP); f8@0:4
- (float)length;
// Original 0x62e8 (0x62e9 raw IMP); @8@0:4
- (id)prev;
// Original 0x62dc (0x62dd raw IMP); @8@0:4
- (id)next;
// Original 0x61bc (0x61bd raw IMP); v8@0:4
- (void)draw;
// Original 0x6010 (0x6011 raw IMP); @20@0:4@8@12f16
- (id)checkCollide:(id)arg0 Vel:(id)arg1 ColRadius:(float)arg2;
// Original 0x5e18 (0x5e19 raw IMP); @20@0:4@8@12f16
- (id)checkCollide:(id)arg0 Dir:(id)arg1 ColRadius:(float)arg2;
// Original 0x5c34 (0x5c35 raw IMP); B16@0:4@8@12
- (bool)checkCollide:(id)arg0 Vel:(id)arg1;
// Original 0x5a88 (0x5a89 raw IMP); @16@0:4@8f12
- (id)checkCollide:(id)arg0 ColRadius:(float)arg1;
// Original 0x5a7c (0x5a7d raw IMP); v12@0:4@8
- (void)setNext:(id)arg0;
// Original 0x5a70 (0x5a71 raw IMP); v12@0:4@8
- (void)setPrev:(id)arg0;
// Original 0x59a0 (0x59a1 raw IMP); v16@0:4@8@12
- (void)setOrigin:(id)arg0 Second:(id)arg1;
// Original 0x5944 (0x5945 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x5918 (0x5919 raw IMP); @16@0:4@8@12
- (id)initWithOrigin:(id)arg0 Second:(id)arg1;
// Original 0x5874 (0x5875 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x28220; ARM32 instance size 36.
@interface Texture2D : NSObject {
@public
    uint32_t _name; // ARM32 offset 4, size 4.
    CGSize _size; // ARM32 offset 8, size 8.
    uint32_t _width; // ARM32 offset 16, size 4.
    uint32_t _height; // ARM32 offset 20, size 4.
    int32_t _format; // ARM32 offset 24, size 4.
    float _maxS; // ARM32 offset 28, size 4.
    float _maxT; // ARM32 offset 32, size 4.
}
// Original 0x660c (0x660d raw IMP); {CGSize=ff}8@0:4
- (CGSize)contentSize;
// Original 0x6600 (0x6601 raw IMP); i8@0:4
- (int32_t)pixelFormat;
// Original 0x65f4 (0x65f5 raw IMP); I8@0:4
- (uint32_t)width;
// Original 0x65e8 (0x65e9 raw IMP); I8@0:4
- (uint32_t)height;
// Original 0x65dc (0x65dd raw IMP); I8@0:4
- (uint32_t)name;
// Original 0x65d0 (0x65d1 raw IMP); f8@0:4
- (float)maxS;
// Original 0x65c4 (0x65c5 raw IMP); f8@0:4
- (float)maxT;
// Original 0x652c (0x652d raw IMP); @8@0:4
- (id)description;
// Original 0x64f0 (0x64f1 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x634c (0x634d raw IMP); @32@0:4r^v8i12I16I20{CGSize=ff}24
- (id)initWithData:(void *)arg0 pixelFormat:(int32_t)arg1 pixelsWide:(uint32_t)arg2 pixelsHigh:(uint32_t)arg3 contentSize:(CGSize)arg4;
@end

// Original class 0x284a0; ARM32 instance size 72.
@interface Tile2D : Texture2D {
@public
    int32_t _splitX; // ARM32 offset 36, size 4.
    int32_t _splitY; // ARM32 offset 40, size 4.
    int32_t _tileCnt; // ARM32 offset 44, size 4.
    float _u; // ARM32 offset 48, size 4.
    float _v; // ARM32 offset 52, size 4.
    float _w; // ARM32 offset 56, size 4.
    float _h; // ARM32 offset 60, size 4.
    float _tileWidth; // ARM32 offset 64, size 4.
    float _tileHeight; // ARM32 offset 68, size 4.
}
// Original 0x7244 (0x7245 raw IMP); f8@0:4
- (float)tileWidth;
// Original 0x7238 (0x7239 raw IMP); f8@0:4
- (float)tileHeight;
// Original 0x7208 (0x7209 raw IMP); v24@0:4i8f12f16f20
- (void)drawTile:(int32_t)arg0 x:(float)arg1 y:(float)arg2 rotate:(float)arg3;
// Original 0x71d0 (0x71d1 raw IMP); v28@0:4i8f12f16f20f24
- (void)drawTile:(int32_t)arg0 x:(float)arg1 y:(float)arg2 scaleX:(float)arg3 scaleY:(float)arg4;
// Original 0x719c (0x719d raw IMP); v32@0:4i8f12f16f20f24f28
- (void)drawTile:(int32_t)arg0 x:(float)arg1 y:(float)arg2 scaleX:(float)arg3 scaleY:(float)arg4 rotate:(float)arg5;
// Original 0x7168 (0x7169 raw IMP); v28@0:4i8f12f16i20f24
- (void)drawTile:(int32_t)arg0 x:(float)arg1 y:(float)arg2 blendType:(int32_t)arg3 alpha:(float)arg4;
// Original 0x7134 (0x7135 raw IMP); v20@0:4i8f12f16
- (void)drawTile:(int32_t)arg0 x:(float)arg1 y:(float)arg2;
// Original 0x6f28 (0x6f29 raw IMP); v40@0:4i8f12f16i20f24f28f32f36
- (void)drawTile:(int32_t)arg0 x:(float)arg1 y:(float)arg2 blendType:(int32_t)arg3 alpha:(float)arg4 scaleX:(float)arg5 scaleY:(float)arg6 rotate:(float)arg7;
// Original 0x6d9c (0x6d9d raw IMP); v16@0:4i8i12
- (void)tileSplitX:(int32_t)arg0 splitY:(int32_t)arg1;
@end

// Original class 0x28668; ARM32 instance size 12.
@interface Vector2 : NSObject {
@public
    float _x; // ARM32 offset 4, size 4.
    float _y; // ARM32 offset 8, size 4.
}
// Original 0x7694 (0x7695 raw IMP); v12@0:4f8
- (void)setX:(float)arg0;
// Original 0x7688 (0x7689 raw IMP); f8@0:4
- (float)x;
// Original 0x767c (0x767d raw IMP); v12@0:4f8
- (void)setY:(float)arg0;
// Original 0x7670 (0x7671 raw IMP); f8@0:4
- (float)y;
// Original 0x7634 (0x7635 raw IMP); v12@0:4@8
- (void)copyFrom:(id)arg0;
// Original 0x75d4 (0x75d5 raw IMP); v8@0:4
- (void)normalize;
// Original 0x759c (0x759d raw IMP); f8@0:4
- (float)angle;
// Original 0x7568 (0x7569 raw IMP); f8@0:4
- (float)squereLength;
// Original 0x7530 (0x7531 raw IMP); f8@0:4
- (float)length;
// Original 0x74c8 (0x74c9 raw IMP); f12@0:4@8
- (float)crossProduct:(id)arg0;
// Original 0x7460 (0x7461 raw IMP); f12@0:4@8
- (float)dotProduct:(id)arg0;
// Original 0x743c (0x743d raw IMP); v8@0:4
- (void)reverse;
// Original 0x7408 (0x7409 raw IMP); v12@0:4f8
- (void)div:(float)arg0;
// Original 0x73d4 (0x73d5 raw IMP); v12@0:4f8
- (void)mul:(float)arg0;
// Original 0x7378 (0x7379 raw IMP); v12@0:4@8
- (void)sub:(id)arg0;
// Original 0x731c (0x731d raw IMP); v12@0:4@8
- (void)add:(id)arg0;
// Original 0x72d4 (0x72d5 raw IMP); v16@0:4f8f12
- (void)setAngle:(float)arg0 Len:(float)arg1;
// Original 0x72bc (0x72bd raw IMP); v16@0:4f8f12
- (void)setX:(float)arg0 Y:(float)arg1;
// Original 0x7294 (0x7295 raw IMP); @12@0:4@8
- (id)initFromVector:(id)arg0;
// Original 0x7250 (0x7251 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x28868; ARM32 instance size 76.
@interface QobBase : NSObject {
@public
    bool _visual; // ARM32 offset 4, size 1.
    int32_t _zLayer; // ARM32 offset 8, size 4.
    int32_t _zOrder; // ARM32 offset 12, size 4.
    int32_t _uiState; // ARM32 offset 16, size 4.
    QobCamera * _cam; // ARM32 offset 20, size 4.
    Vector2 * _pos; // ARM32 offset 24, size 4.
    bool _fixedPos; // ARM32 offset 28, size 1.
    float _alpha; // ARM32 offset 32, size 4.
    float _scaleX; // ARM32 offset 36, size 4.
    float _scaleY; // ARM32 offset 40, size 4.
    float _rotate; // ARM32 offset 44, size 4.
    bool _show; // ARM32 offset 48, size 1.
    bool _uiObject; // ARM32 offset 49, size 1.
    bool _remove; // ARM32 offset 50, size 1.
    bool _isInScreen; // ARM32 offset 51, size 1.
    CGRect _boundRect; // ARM32 offset 52, size 16.
    QobBase * _parent; // ARM32 offset 68, size 4.
    NSMutableArray * _childArray; // ARM32 offset 72, size 4.
}
// Original 0x82ac (0x82ad raw IMP); v12@0:4B8
- (void)setVisual:(bool)arg0;
// Original 0x82a0 (0x82a1 raw IMP); B8@0:4
- (bool)visual;
// Original 0x8294 (0x8295 raw IMP); v12@0:4i8
- (void)setLayer:(int32_t)arg0;
// Original 0x8288 (0x8289 raw IMP); i8@0:4
- (int32_t)layer;
// Original 0x827c (0x827d raw IMP); v12@0:4i8
- (void)setZ:(int32_t)arg0;
// Original 0x8270 (0x8271 raw IMP); i8@0:4
- (int32_t)z;
// Original 0x8264 (0x8265 raw IMP); B8@0:4
- (bool)removeThis;
// Original 0x8258 (0x8259 raw IMP); @8@0:4
- (id)pos;
// Original 0x824c (0x824d raw IMP); v12@0:4f8
- (void)setAlpha:(float)arg0;
// Original 0x8240 (0x8241 raw IMP); f8@0:4
- (float)alpha;
// Original 0x8234 (0x8235 raw IMP); v12@0:4f8
- (void)setScaleX:(float)arg0;
// Original 0x8228 (0x8229 raw IMP); f8@0:4
- (float)scaleX;
// Original 0x821c (0x821d raw IMP); v12@0:4f8
- (void)setScaleY:(float)arg0;
// Original 0x8210 (0x8211 raw IMP); f8@0:4
- (float)scaleY;
// Original 0x8204 (0x8205 raw IMP); v12@0:4f8
- (void)setRotate:(float)arg0;
// Original 0x81f8 (0x81f9 raw IMP); f8@0:4
- (float)rotate;
// Original 0x81c4 (0x81c5 raw IMP); {CGRect={CGPoint=ff}{CGSize=ff}}8@0:4
- (CGRect)boundRect;
// Original 0x81b8 (0x81b9 raw IMP); B8@0:4
- (bool)isInScreen;
// Original 0x81b4 (0x81b5 raw IMP); B20@0:4{CGPoint=ff}8i16
- (bool)onTap:(CGPoint)arg0 State:(int32_t)arg1;
// Original 0x8164 (0x8165 raw IMP); v12@0:4B8
- (void)setUiReceiver:(bool)arg0;
// Original 0x8158 (0x8159 raw IMP); v12@0:4@8
- (void)setParent:(id)arg0;
// Original 0x814c (0x814d raw IMP); v12@0:4i8
- (void)setZOrder:(int32_t)arg0;
// Original 0x80d8 (0x80d9 raw IMP); v8@0:4
- (void)remove;
// Original 0x80bc (0x80bd raw IMP); @12@0:4i8
- (id)childAtIndex:(int32_t)arg0;
// Original 0x80a0 (0x80a1 raw IMP); i8@0:4
- (int32_t)childCount;
// Original 0x800c (0x800d raw IMP); v12@0:4@8
- (void)addChild:(id)arg0;
// Original 0x7e18 (0x7e19 raw IMP); B12@0:4@8
- (bool)isCollideBound:(id)arg0;
// Original 0x7d78 (0x7d79 raw IMP); B16@0:4f8f12
- (bool)isInPtX:(float)arg0 Y:(float)arg1;
// Original 0x7c78 (0x7c79 raw IMP); B8@0:4
- (bool)isInCam;
// Original 0x7c1c (0x7c1d raw IMP); v12@0:4@8
- (void)setCam:(id)arg0;
// Original 0x7be8 (0x7be9 raw IMP); B8@0:4
- (bool)isShow;
// Original 0x7bdc (0x7bdd raw IMP); v12@0:4B8
- (void)setShow:(bool)arg0;
// Original 0x7b00 (0x7b01 raw IMP); f8@0:4
- (float)screenPosY;
// Original 0x7a28 (0x7a29 raw IMP); f8@0:4
- (float)screenPosX;
// Original 0x7a1c (0x7a1d raw IMP); @8@0:4
- (id)getPos;
// Original 0x79d4 (0x79d5 raw IMP); f8@0:4
- (float)posY;
// Original 0x798c (0x798d raw IMP); f8@0:4
- (float)posX;
// Original 0x7958 (0x7959 raw IMP); v16@0:4f8f12
- (void)setPosX:(float)arg0 Y:(float)arg1;
// Original 0x7940 (0x7941 raw IMP); v12@0:4f8
- (void)setScale:(float)arg0;
// Original 0x7932 (0x7933 raw IMP); v12@0:4B8
- (void)setFixedPos:(bool)arg0;
// Original 0x7930 (0x7931 raw IMP); v8@0:4
- (void)draw;
// Original 0x7830 (0x7831 raw IMP); v8@0:4
- (void)tick;
// Original 0x77e4 (0x77e5 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x76a0 (0x76a1 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x28cf8; ARM32 instance size 124.
@interface QobButton : QobBase {
@public
    Tile2D * _tile; // ARM32 offset 76, size 4.
    bool _active; // ARM32 offset 80, size 1.
    int16_t _tileNo; // ARM32 offset 82, size 2.
    int16_t _releaseTileNo; // ARM32 offset 84, size 2.
    int16_t _deactiveTileNo; // ARM32 offset 86, size 2.
    uint32_t _id; // ARM32 offset 88, size 4.
    double _btnClickTime; // ARM32 offset 92, size 8.
    double _btnReleaseTime; // ARM32 offset 100, size 8.
    float _pushDepth; // ARM32 offset 108, size 4.
    float _pushDelay; // ARM32 offset 112, size 4.
    float _releaseHeight; // ARM32 offset 116, size 4.
    float _releaseDelay; // ARM32 offset 120, size 4.
}
// Original 0x8afc (0x8afd raw IMP); @8@0:4
- (id)tile;
// Original 0x8af0 (0x8af1 raw IMP); v12@0:4I8
- (void)setButtonId:(uint32_t)arg0;
// Original 0x8ae4 (0x8ae5 raw IMP); I8@0:4
- (uint32_t)buttonId;
// Original 0x8ad8 (0x8ad9 raw IMP); v12@0:4f8
- (void)setPushDepth:(float)arg0;
// Original 0x8acc (0x8acd raw IMP); f8@0:4
- (float)pushDepth;
// Original 0x8ac0 (0x8ac1 raw IMP); v12@0:4f8
- (void)setPushDelay:(float)arg0;
// Original 0x8ab4 (0x8ab5 raw IMP); f8@0:4
- (float)pushDelay;
// Original 0x8aa8 (0x8aa9 raw IMP); v12@0:4f8
- (void)setReleaseHeight:(float)arg0;
// Original 0x8a9c (0x8a9d raw IMP); f8@0:4
- (float)releaseHeight;
// Original 0x8a90 (0x8a91 raw IMP); v12@0:4f8
- (void)setReleaseDelay:(float)arg0;
// Original 0x8a84 (0x8a85 raw IMP); f8@0:4
- (float)releaseDelay;
// Original 0x87f8 (0x87f9 raw IMP); B20@0:4{CGPoint=ff}8i16
- (bool)onTap:(CGPoint)arg0 State:(int32_t)arg1;
// Original 0x850c (0x850d raw IMP); v8@0:4
- (void)draw;
// Original 0x84d8 (0x84d9 raw IMP); v8@0:4
- (void)tick;
// Original 0x8494 (0x8495 raw IMP); v8@0:4
- (void)setReleaseAction;
// Original 0x8488 (0x8489 raw IMP); v12@0:4s8
- (void)setDeactiveTileNo:(int16_t)arg0;
// Original 0x847c (0x847d raw IMP); v12@0:4s8
- (void)setReleaseTileNo:(int16_t)arg0;
// Original 0x8470 (0x8471 raw IMP); v12@0:4s8
- (void)setDefaultTileNo:(int16_t)arg0;
// Original 0x8464 (0x8465 raw IMP); v12@0:4B8
- (void)setActive:(bool)arg0;
// Original 0x8440 (0x8441 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x841c (0x841d raw IMP); @20@0:4@8s12I16
- (id)initWithTile:(id)arg0 TileNo:(int16_t)arg1 ID:(uint32_t)arg2;
// Original 0x82b8 (0x82b9 raw IMP); @16@0:4@8s12
- (id)initWithTile:(id)arg0 TileNo:(int16_t)arg1;
@end

// Original class 0x28fbc; ARM32 instance size 88.
@interface QobImage : QobBase {
@public
    Tile2D * _tile; // ARM32 offset 76, size 4.
    int16_t _tileNo; // ARM32 offset 80, size 2.
    int32_t _blendType; // ARM32 offset 84, size 4.
}
// Original 0x8da8 (0x8da9 raw IMP); v12@0:4i8
- (void)setBlendType:(int32_t)arg0;
// Original 0x8d9c (0x8d9d raw IMP); i8@0:4
- (int32_t)blendType;
// Original 0x8cf0 (0x8cf1 raw IMP); v8@0:4
- (void)draw;
// Original 0x8ce4 (0x8ce5 raw IMP); v12@0:4s8
- (void)setTileNo:(int16_t)arg0;
// Original 0x8c2c (0x8c2d raw IMP); v16@0:4@8s12
- (void)setTile:(id)arg0 tileNo:(int16_t)arg1;
// Original 0x8c04 (0x8c05 raw IMP); f8@0:4
- (float)tileWidth;
// Original 0x8b08 (0x8b09 raw IMP); @16@0:4@8s12
- (id)initWithTile:(id)arg0 tileNo:(int16_t)arg1;
@end

// Original class 0x29104; ARM32 instance size 357.
@interface QobImageFont : QobBase {
@public
    Tile2D * _tile; // ARM32 offset 76, size 4.
    int32_t _num; // ARM32 offset 80, size 4.
    int32_t _numCnt; // ARM32 offset 84, size 4.
    int32_t _tileList[64]; // ARM32 offset 88, size 256.
    float _alignRate; // ARM32 offset 344, size 4.
    float _pitch; // ARM32 offset 348, size 4.
    int32_t _blendType; // ARM32 offset 352, size 4.
    bool _boundCheck; // ARM32 offset 356, size 1.
}
// Original 0x9210 (0x9211 raw IMP); i8@0:4
- (int32_t)num;
// Original 0x9204 (0x9205 raw IMP); v12@0:4f8
- (void)setPitch:(float)arg0;
// Original 0x91f8 (0x91f9 raw IMP); f8@0:4
- (float)pitch;
// Original 0x91ec (0x91ed raw IMP); v12@0:4i8
- (void)setBlendType:(int32_t)arg0;
// Original 0x91e0 (0x91e1 raw IMP); i8@0:4
- (int32_t)blendType;
// Original 0x9054 (0x9055 raw IMP); v8@0:4
- (void)draw;
// Original 0x9048 (0x9049 raw IMP); v12@0:4B8
- (void)setBoundCheck:(bool)arg0;
// Original 0x8f80 (0x8f81 raw IMP); v12@0:4*8
- (void)setText:(char *)arg0;
// Original 0x8ef0 (0x8ef1 raw IMP); v12@0:4i8
- (void)setNumber:(int32_t)arg0;
// Original 0x8ee4 (0x8ee5 raw IMP); v12@0:4f8
- (void)setAlignRate:(float)arg0;
// Original 0x8db4 (0x8db5 raw IMP); @12@0:4@8
- (id)initWithTile:(id)arg0;
@end

// Original class 0x29308; ARM32 instance size 138.
@interface QobParticle : QobImage {
@public
    Vector2 * _vel; // ARM32 offset 88, size 4.
    Vector2 * _velTick; // ARM32 offset 92, size 4.
    float _g; // ARM32 offset 96, size 4.
    float _radius; // ARM32 offset 100, size 4.
    float _rotVel; // ARM32 offset 104, size 4.
    float _scaleVel; // ARM32 offset 108, size 4.
    float _velAdd; // ARM32 offset 112, size 4.
    float _liveTime; // ARM32 offset 116, size 4.
    float _startTime; // ARM32 offset 120, size 4.
    ColVector * _col; // ARM32 offset 124, size 4.
    float _groundPos; // ARM32 offset 128, size 4.
    int32_t _aniFrame; // ARM32 offset 132, size 4.
    bool _tileAni; // ARM32 offset 136, size 1.
    bool _selfRemove; // ARM32 offset 137, size 1.
}
// Original 0x9c8c (0x9c8d raw IMP); @8@0:4
- (id)pos;
// Original 0x9c80 (0x9c81 raw IMP); @8@0:4
- (id)vel;
// Original 0x9c74 (0x9c75 raw IMP); v12@0:4f8
- (void)setG:(float)arg0;
// Original 0x9c68 (0x9c69 raw IMP); f8@0:4
- (float)g;
// Original 0x9c5c (0x9c5d raw IMP); v12@0:4f8
- (void)setRadius:(float)arg0;
// Original 0x9c50 (0x9c51 raw IMP); f8@0:4
- (float)radius;
// Original 0x9c44 (0x9c45 raw IMP); v12@0:4f8
- (void)setRotVel:(float)arg0;
// Original 0x9c38 (0x9c39 raw IMP); f8@0:4
- (float)rotVel;
// Original 0x9c2c (0x9c2d raw IMP); v12@0:4f8
- (void)setScaleVel:(float)arg0;
// Original 0x9c20 (0x9c21 raw IMP); f8@0:4
- (float)scaleVel;
// Original 0x9c14 (0x9c15 raw IMP); v12@0:4f8
- (void)setGroundPos:(float)arg0;
// Original 0x9c08 (0x9c09 raw IMP); f8@0:4
- (float)groundPos;
// Original 0x9bfc (0x9bfd raw IMP); v12@0:4f8
- (void)setVelAdd:(float)arg0;
// Original 0x9bf0 (0x9bf1 raw IMP); f8@0:4
- (float)velAdd;
// Original 0x9bbc (0x9bbd raw IMP); v16@0:4f8f12
- (void)setVelX:(float)arg0 Y:(float)arg1;
// Original 0x9b88 (0x9b89 raw IMP); v16@0:4f8f12
- (void)setPosX:(float)arg0 Y:(float)arg1;
// Original 0x9b7c (0x9b7d raw IMP); v12@0:4@8
- (void)setColVector:(id)arg0;
// Original 0x9a14 (0x9a15 raw IMP); v12@0:4@8
- (void)setBound:(id)arg0;
// Original 0x9a08 (0x9a09 raw IMP); v12@0:4B8
- (void)setSelfRemove:(bool)arg0;
// Original 0x968c (0x968d raw IMP); v8@0:4
- (void)processFly;
// Original 0x95dc (0x95dd raw IMP); v8@0:4
- (void)draw;
// Original 0x94a8 (0x94a9 raw IMP); v8@0:4
- (void)tick;
// Original 0x945c (0x945d raw IMP); v8@0:4
- (void)dealloc;
// Original 0x942c (0x942d raw IMP); v8@0:4
- (void)start;
// Original 0x9420 (0x9421 raw IMP); v12@0:4f8
- (void)setLiveTime:(float)arg0;
// Original 0x9408 (0x9409 raw IMP); v12@0:4i8
- (void)setTileAni:(int32_t)arg0;
// Original 0x92b4 (0x92b5 raw IMP); v8@0:4
- (void)reset;
// Original 0x921c (0x921d raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x29664; ARM32 instance size 124.
@interface GobBullet : QobImage {
@public
    bool _toPlayer; // ARM32 offset 88, size 1.
    WeaponInfo * _info; // ARM32 offset 92, size 4.
    Vector2 * _vel; // ARM32 offset 96, size 4.
    Vector2 * _velTick; // ARM32 offset 100, size 4.
    float _startTime; // ARM32 offset 104, size 4.
    float _effectRegenTime; // ARM32 offset 108, size 4.
    float _effectDelay; // ARM32 offset 112, size 4.
    bool _flameShow; // ARM32 offset 116, size 1.
    GobTrailEffect * _smokeTrail; // ARM32 offset 120, size 4.
}
// Original 0xab04 (0xab05 raw IMP); v8@0:4
- (void)draw;
// Original 0xab00 (0xab01 raw IMP); B8@0:4
- (bool)isInCam;
// Original 0xa444 (0xa445 raw IMP); v8@0:4
- (void)tick;
// Original 0xa198 (0xa199 raw IMP); v12@0:4B8
- (void)setCollide:(bool)arg0;
// Original 0x9dc4 (0x9dc5 raw IMP); v24@0:4@8f12f16f20
- (void)setFire:(id)arg0 angle:(float)arg1 x:(float)arg2 y:(float)arg3;
// Original 0x9db8 (0x9db9 raw IMP); v12@0:4B8
- (void)setToPlayer:(bool)arg0;
// Original 0x9d6c (0x9d6d raw IMP); v8@0:4
- (void)dealloc;
// Original 0x9c9c (0x9c9d raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x299f8; ARM32 instance size 528.
@interface GobWorld : QobBase {
@public
    ResMgr_Tile * _tileResMgr; // ARM32 offset 76, size 4.
    int16_t _gameState; // ARM32 offset 80, size 2.
    float _changeStateTime; // ARM32 offset 84, size 4.
    QobBase * _camObject; // ARM32 offset 88, size 4.
    QobBase * _objectBase; // ARM32 offset 92, size 4.
    QobBase * _listEnemy; // ARM32 offset 96, size 4.
    QobBase * _listMapObject; // ARM32 offset 100, size 4.
    QobParticle * _particleBuffer[64]; // ARM32 offset 104, size 256.
    GobHvM_Player * _hvMach; // ARM32 offset 360, size 4.
    QobImage * _imgBG; // ARM32 offset 364, size 4.
    NSMutableArray * _colList; // ARM32 offset 368, size 4.
    double _gameTime; // ARM32 offset 372, size 8.
    bool _pause; // ARM32 offset 380, size 1.
    float _accelX; // ARM32 offset 384, size 4.
    float _accelY; // ARM32 offset 388, size 4.
    float _accelZ; // ARM32 offset 392, size 4.
    float _backLine; // ARM32 offset 396, size 4.
    float _frontLine; // ARM32 offset 400, size 4.
    float _left; // ARM32 offset 404, size 4.
    float _top; // ARM32 offset 408, size 4.
    float _right; // ARM32 offset 412, size 4.
    float _bottom; // ARM32 offset 416, size 4.
    float _centerX; // ARM32 offset 420, size 4.
    float _centerY; // ARM32 offset 424, size 4.
    float _bgScale; // ARM32 offset 428, size 4.
    TLoadMapInfo _loadMapInfo; // ARM32 offset 432, size 40.
    float _colDivSize; // ARM32 offset 472, size 4.
    TColVectorList * _listColVector; // ARM32 offset 476, size 4.
    int16_t _listColVecCnt; // ARM32 offset 480, size 2.
    float _objectRegenPos; // ARM32 offset 484, size 4.
    int32_t _objectRegenSize; // ARM32 offset 488, size 4.
    int32_t _objectRegenCnt; // ARM32 offset 492, size 4.
    TObjectRegenInfo * _listObjectRegen; // ARM32 offset 496, size 4.
    int32_t _bossCount; // ARM32 offset 500, size 4.
    int32_t _bossEncount; // ARM32 offset 504, size 4.
    int32_t _enemyRegenSize; // ARM32 offset 508, size 4.
    int32_t _enemyRegenCnt; // ARM32 offset 512, size 4.
    TEnemyRegenInfo * _listEnemyRegen; // ARM32 offset 516, size 4.
    int32_t _survivalRegenSeed; // ARM32 offset 520, size 4.
    float _timeItemRegen; // ARM32 offset 524, size 4.
}
// Original 0xe9bc (0xe9bd raw IMP); d8@0:4
- (double)time;
// Original 0xe9b0 (0xe9b1 raw IMP); v12@0:4B8
- (void)setPause:(bool)arg0;
// Original 0xe9a4 (0xe9a5 raw IMP); B8@0:4
- (bool)pause;
// Original 0xe998 (0xe999 raw IMP); @8@0:4
- (id)myMach;
// Original 0xe98c (0xe98d raw IMP); v12@0:4i8
- (void)setBossCount:(int32_t)arg0;
// Original 0xe980 (0xe981 raw IMP); i8@0:4
- (int32_t)bossCount;
// Original 0xe974 (0xe975 raw IMP); v12@0:4i8
- (void)setBossEncount:(int32_t)arg0;
// Original 0xe968 (0xe969 raw IMP); i8@0:4
- (int32_t)bossEncount;
// Original 0xe95c (0xe95d raw IMP); f8@0:4
- (float)backLine;
// Original 0xe950 (0xe951 raw IMP); f8@0:4
- (float)frontLine;
// Original 0xe944 (0xe945 raw IMP); f8@0:4
- (float)left;
// Original 0xe938 (0xe939 raw IMP); f8@0:4
- (float)top;
// Original 0xe92c (0xe92d raw IMP); f8@0:4
- (float)right;
// Original 0xe920 (0xe921 raw IMP); f8@0:4
- (float)bottom;
// Original 0xe914 (0xe915 raw IMP); f8@0:4
- (float)bgScale;
// Original 0xe908 (0xe909 raw IMP); @8@0:4
- (id)objectBase;
// Original 0xe7e0 (0xe7e1 raw IMP); v12@0:4@8
- (void)handleButton:(id)arg0;
// Original 0xe7d4 (0xe7d5 raw IMP); @8@0:4
- (id)tileResMgr;
// Original 0xe798 (0xe799 raw IMP); @12@0:4i8
- (id)colVector:(int32_t)arg0;
// Original 0xe77c (0xe77d raw IMP); i8@0:4
- (int32_t)colCount;
// Original 0xe3f4 (0xe3f5 raw IMP); B12@0:4@8
- (bool)openMap:(id)arg0;
// Original 0xe2cc (0xe2cd raw IMP); i16@0:4*8i12
- (int32_t)readChunk:(char *)arg0 Pos:(int32_t)arg1;
// Original 0xe1b4 (0xe1b5 raw IMP); v20@0:4i8*12i16
- (void)readEnemy:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2;
// Original 0xe158 (0xe159 raw IMP); v24@0:4i8*12i16i20
- (void)readEnemyList:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2 Cnt:(int32_t)arg3;
// Original 0xdfa8 (0xdfa9 raw IMP); v20@0:4i8*12i16
- (void)readBGName:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2;
// Original 0xde40 (0xde41 raw IMP); v20@0:4i8*12i16
- (void)readObject:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2;
// Original 0xdda8 (0xdda9 raw IMP); v24@0:4i8*12i16i20
- (void)readObjectList:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2 Cnt:(int32_t)arg3;
// Original 0xdbfc (0xdbfd raw IMP); v20@0:4i8*12i16
- (void)readJoint:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2;
// Original 0xdbe4 (0xdbe5 raw IMP); v20@0:4i8*12i16
- (void)readJointList:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2;
// Original 0xda80 (0xda81 raw IMP); v20@0:4i8*12i16
- (void)readMapSize:(int32_t)arg0 Data:(char *)arg1 Size:(int32_t)arg2;
// Original 0xd958 (0xd959 raw IMP); f12@0:4f8
- (float)getMapHeight:(float)arg0;
// Original 0xd92c (0xd92d raw IMP); B20@0:4{CGPoint=ff}8i16
- (bool)onTap:(CGPoint)arg0 State:(int32_t)arg1;
// Original 0xd8dc (0xd8dd raw IMP); @12@0:4f8
- (id)getColListFromX:(float)arg0;
// Original 0xd898 (0xd899 raw IMP); @8@0:4
- (id)getFreeParticle;
// Original 0xd848 (0xd849 raw IMP); v8@0:4
- (void)killBoss;
// Original 0xd824 (0xd825 raw IMP); @12@0:4^{TEnemyRegenInfo=BB[24c]{CGPoint=ff}}8
- (id)addEnemyMach:(TEnemyRegenInfo *)arg0;
// Original 0xd6b8 (0xd6b9 raw IMP); @24@0:4*8f12f16B20
- (id)addEnemyMach:(char *)arg0 X:(float)arg1 Y:(float)arg2 Boss:(bool)arg3;
// Original 0xd474 (0xd475 raw IMP); @16@0:4f8f12
- (id)addMyMachX:(float)arg0 Y:(float)arg1;
// Original 0xd44c (0xd44d raw IMP); @16@0:4f8f12
- (id)checkEnemyBulletCollideX:(float)arg0 Y:(float)arg1;
// Original 0xd3c8 (0xd3c9 raw IMP); @16@0:4f8f12
- (id)checkBulletCollideX:(float)arg0 Y:(float)arg1;
// Original 0xd2f4 (0xd2f5 raw IMP); v12@0:4f8
- (void)removeTrashObject:(float)arg0;
// Original 0xd2d8 (0xd2d9 raw IMP); v8@0:4
- (void)clearTileResMgr;
// Original 0xd27c (0xd27d raw IMP); v8@0:4
- (void)clearAllObject;
// Original 0xd06a (0xd06b raw IMP); v8@0:4
- (void)processSurvivalMode;
// Original 0xd068 (0xd069 raw IMP); v8@0:4
- (void)processScenarioMode;
// Original 0xcd00 (0xcd01 raw IMP); v8@0:4
- (void)processObjectRegen;
// Original 0xc8b8 (0xc8b9 raw IMP); v8@0:4
- (void)processCamera;
// Original 0xc830 (0xc831 raw IMP); v8@0:4
- (void)processMyMach;
// Original 0xc228 (0xc229 raw IMP); v8@0:4
- (void)processStageClear;
// Original 0xbf74 (0xbf75 raw IMP); v8@0:4
- (void)setGameOver;
// Original 0xb8f0 (0xb8f1 raw IMP); v8@0:4
- (void)setNextStage;
// Original 0xb8c4 (0xb8c5 raw IMP); v8@0:4
- (void)removeLoadingScreen;
// Original 0xb840 (0xb841 raw IMP); v8@0:4
- (void)setSurvival;
// Original 0xb780 (0xb781 raw IMP); v16@0:4i8i12
- (void)setMission:(int32_t)arg0 Stage:(int32_t)arg1;
// Original 0xb754 (0xb755 raw IMP); v12@0:4s8
- (void)setGameState:(int16_t)arg0;
// Original 0xb658 (0xb659 raw IMP); v8@0:4
- (void)tick;
// Original 0xb55c (0xb55d raw IMP); v16@0:4@8@12
- (void)accelerometer:(id)arg0 didAccelerate:(id)arg1;
// Original 0xb484 (0xb485 raw IMP); v8@0:4
- (void)dealloc;
// Original 0xb0a4 (0xb0a5 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2a0e0; ARM32 instance size 8.
@interface ResMgr_Tile : NSObject {
@public
    NSMutableDictionary * _dictTile; // ARM32 offset 4, size 4.
}
// Original 0xec64 (0xec65 raw IMP); v12@0:4@8
- (void)removeTile:(id)arg0;
// Original 0xec48 (0xec49 raw IMP); v8@0:4
- (void)removeAllTiles;
// Original 0xeb14 (0xeb15 raw IMP); @12@0:4@8
- (id)getTile:(id)arg0;
// Original 0xead8 (0xead9 raw IMP); v8@0:4
- (void)dealloc;
// Original 0xea7c (0xea7d raw IMP); @8@0:4
- (id)init;
// Original 0xe9cc (0xe9cd raw IMP); @8@0:4
+ (id)sharedMgr;
@end

// Original class 0x2a1e4; ARM32 instance size 108.
@interface QobCamera : QobBase {
@public
    CGSize _halfScr; // ARM32 offset 76, size 8.
    CGPoint _offset; // ARM32 offset 84, size 8.
    float _quakeStartTime; // ARM32 offset 92, size 4.
    float _quakeDelay; // ARM32 offset 96, size 4.
    float _quakeSize; // ARM32 offset 100, size 4.
    float _quakePitch; // ARM32 offset 104, size 4.
}
// Original 0xef8c (0xef8d raw IMP); {CGSize=ff}8@0:4
- (CGSize)halfScr;
// Original 0xef5c (0xef5d raw IMP); {CGPoint=ff}8@0:4
- (CGPoint)offset;
// Original 0xeef8 (0xeef9 raw IMP); v20@0:4f8i12f16
- (void)setQuake:(float)arg0 Count:(int32_t)arg1 Delay:(float)arg2;
// Original 0xeeb8 (0xeeb9 raw IMP); v16@0:4f8f12
- (void)setScreenWidth:(float)arg0 Height:(float)arg1;
// Original 0xed80 (0xed81 raw IMP); v8@0:4
- (void)tick;
// Original 0xed5c (0xed5d raw IMP); v8@0:4
- (void)dealloc;
// Original 0xecf8 (0xecf9 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2a38c; ARM32 instance size 140.
@interface GuiTitle : QobBase {
@public
    ResMgr_Tile * _tileResMgr; // ARM32 offset 76, size 4.
    uint32_t _sfx[3]; // ARM32 offset 80, size 12.
    CGPoint _camPos; // ARM32 offset 92, size 8.
    QobImage * _imgBG; // ARM32 offset 100, size 4.
    QobImage * _imgTitle; // ARM32 offset 104, size 4.
    QobImage * _imgEpisode; // ARM32 offset 108, size 4.
    QobImage * _imgStartGame; // ARM32 offset 112, size 4.
    float _bgScale; // ARM32 offset 116, size 4.
    float _bgRotate; // ARM32 offset 120, size 4.
    float _titleScale; // ARM32 offset 124, size 4.
    float _episodeScale; // ARM32 offset 128, size 4.
    bool _playTitleInSnd; // ARM32 offset 132, size 1.
    bool _playEpisodeInSnd; // ARM32 offset 133, size 1.
    bool _playStartGameSnd; // ARM32 offset 134, size 1.
    float _startTime; // ARM32 offset 136, size 4.
}
// Original 0xf934 (0xf935 raw IMP); B20@0:4{CGPoint=ff}8i16
- (bool)onTap:(CGPoint)arg0 State:(int32_t)arg1;
// Original 0xf464 (0xf465 raw IMP); v8@0:4
- (void)processTitle;
// Original 0xf414 (0xf415 raw IMP); v8@0:4
- (void)tick;
// Original 0xf3c8 (0xf3c9 raw IMP); v8@0:4
- (void)dealloc;
// Original 0xefbc (0xefbd raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2a588; ARM32 instance size 40.
@interface QobManager : NSObject {
@public
    NSMutableArray * _uiReceiverArray; // ARM32 offset 4, size 4.
    QVisualLayer * _visualLayer[7]; // ARM32 offset 8, size 28.
    QobCamera * _cam; // ARM32 offset 36, size 4.
}
// Original 0xfc18 (0xfc19 raw IMP); B20@0:4{CGPoint=ff}8i16
- (bool)onTap:(CGPoint)arg0 State:(int32_t)arg1;
// Original 0xfbfc (0xfbfd raw IMP); v12@0:4@8
- (void)removeUiReceiver:(id)arg0;
// Original 0xfbe0 (0xfbe1 raw IMP); v12@0:4@8
- (void)addUiReceiver:(id)arg0;
// Original 0xfbac (0xfbad raw IMP); v8@0:4
- (void)drawVisual;
// Original 0xfb7c (0xfb7d raw IMP); v16@0:4@8i12
- (void)addVisual:(id)arg0 Layer:(int32_t)arg1;
// Original 0xfb70 (0xfb71 raw IMP); v12@0:4@8
- (void)setMainCam:(id)arg0;
// Original 0xfae4 (0xfae5 raw IMP); v8@0:4
- (void)dealloc;
// Original 0xfa58 (0xfa59 raw IMP); @8@0:4
- (id)init;
// Original 0xf9a8 (0xf9a9 raw IMP); @8@0:4
+ (id)sharedMgr;
@end

// Original class 0x2a6fc; ARM32 instance size 1244.
@interface GlobalInfo : NSObject {
@public
    NSMutableDictionary * _dictPartsInfo; // ARM32 offset 4, size 4.
    NSMutableDictionary * _dictMachInfo; // ARM32 offset 8, size 4.
    NSMutableDictionary * _dictWeaponInfo; // ARM32 offset 12, size 4.
    int32_t _gameMode; // ARM32 offset 16, size 4.
    int32_t _mission; // ARM32 offset 20, size 4.
    int32_t _stage; // ARM32 offset 24, size 4.
    int32_t _levelUpExp; // ARM32 offset 28, size 4.
    int32_t _levelExp; // ARM32 offset 32, size 4.
    int16_t _selWeapon; // ARM32 offset 36, size 2.
    TWeaponItem * _selWeaponItem; // ARM32 offset 40, size 4.
    int32_t _stageCnt[10]; // ARM32 offset 44, size 40.
    int32_t _stageIdx[50]; // ARM32 offset 84, size 200.
    uint32_t _sfxID[15]; // ARM32 offset 284, size 60.
    TItemInfo _regenItemInfo[32]; // ARM32 offset 344, size 896.
    TSaveSlotData * _slotData; // ARM32 offset 1240, size 4.
}
// Original 0x11338 (0x11339 raw IMP); v12@0:4i8
- (void)setGameMode:(int32_t)arg0;
// Original 0x1132c (0x1132d raw IMP); i8@0:4
- (int32_t)gameMode;
// Original 0x11320 (0x11321 raw IMP); v12@0:4i8
- (void)setMission:(int32_t)arg0;
// Original 0x11314 (0x11315 raw IMP); i8@0:4
- (int32_t)mission;
// Original 0x11308 (0x11309 raw IMP); v12@0:4i8
- (void)setStage:(int32_t)arg0;
// Original 0x112fc (0x112fd raw IMP); i8@0:4
- (int32_t)stage;
// Original 0x112f0 (0x112f1 raw IMP); v12@0:4^{TSaveSlotData=iiBiiiiiifiCCCCi[32{TWeaponItem=[24c]i}]s[64i]}8
- (void)setSlotData:(TSaveSlotData *)arg0;
// Original 0x112e4 (0x112e5 raw IMP); ^{TSaveSlotData=iiBiiiiiifiCCCCi[32{TWeaponItem=[24c]i}]s[64i]}8@0:4
- (TSaveSlotData *)slotData;
// Original 0x112d8 (0x112d9 raw IMP); s8@0:4
- (int16_t)selWeapon;
// Original 0x112cc (0x112cd raw IMP); ^{TWeaponItem=[24c]i}8@0:4
- (TWeaponItem *)selWeaponItem;
// Original 0x112b0 (0x112b1 raw IMP); @12@0:4@8
- (id)getWeaponInfo:(id)arg0;
// Original 0x111f8 (0x111f9 raw IMP); B12@0:4@8
- (bool)openWeaponInfo:(id)arg0;
// Original 0x10e00 (0x10e01 raw IMP); i16@0:4*8i12
- (int32_t)readWeaponChunk:(char *)arg0 Pos:(int32_t)arg1;
// Original 0x10de4 (0x10de5 raw IMP); @12@0:4@8
- (id)getMachInfo:(id)arg0;
// Original 0x10d28 (0x10d29 raw IMP); B12@0:4@8
- (bool)openMachInfo:(id)arg0;
// Original 0x10c58 (0x10c59 raw IMP); i20@0:4*8i12i16
- (int32_t)readMachChunk:(char *)arg0 Pos:(int32_t)arg1 Length:(int32_t)arg2;
// Original 0x10c3c (0x10c3d raw IMP); @12@0:4@8
- (id)getPartsInfo:(id)arg0;
// Original 0x10b84 (0x10b85 raw IMP); B12@0:4@8
- (bool)openPartsInfo:(id)arg0;
// Original 0x10a54 (0x10a55 raw IMP); i16@0:4*8i12
- (int32_t)readPartsChunk:(char *)arg0 Pos:(int32_t)arg1;
// Original 0x10a0c (0x10a0d raw IMP); v8@0:4
- (void)updateLastStage;
// Original 0x109d4 (0x109d5 raw IMP); i8@0:4
- (int32_t)getCurrentStage;
// Original 0x109bc (0x109bd raw IMP); i12@0:4i8
- (int32_t)getStageIndex:(int32_t)arg0;
// Original 0x109a4 (0x109a5 raw IMP); i12@0:4i8
- (int32_t)getStageCount:(int32_t)arg0;
// Original 0x10984 (0x10985 raw IMP); ^{TItemInfo=ss[24c]}12@0:4i8
- (TItemInfo *)regenItemInfo:(int32_t)arg0;
// Original 0x10944 (0x10945 raw IMP); v12@0:4i8
- (void)removeWeapon:(int32_t)arg0;
// Original 0x108ec (0x108ed raw IMP); v12@0:4i8
- (void)selectWeapon:(int32_t)arg0;
// Original 0x1082c (0x1082d raw IMP); v16@0:4*8i12
- (void)addWeaponItem:(char *)arg0 Charge:(int32_t)arg1;
// Original 0x106e0 (0x106e1 raw IMP); v12@0:4i8
- (void)addExp:(int32_t)arg0;
// Original 0x106c4 (0x106c5 raw IMP); i8@0:4
- (int32_t)levelUpExp;
// Original 0x105f8 (0x105f9 raw IMP); v8@0:4
- (void)calcLevelUpExp;
// Original 0x105e0 (0x105e1 raw IMP); L12@0:4i8
- (uint32_t)sfxID:(int32_t)arg0;
// Original 0x10584 (0x10585 raw IMP); v8@0:4
- (void)dealloc;
// Original 0xfd44 (0xfd45 raw IMP); @8@0:4
- (id)init;
// Original 0xfc94 (0xfc95 raw IMP); @8@0:4
+ (id)sharedInfo;
@end

// Original class 0x2aaa0; ARM32 instance size 20.
@interface PartsInfo : NSObject {
@public
    NSString * _partsName; // ARM32 offset 4, size 4.
    NSString * _tileName; // ARM32 offset 8, size 4.
    uint8_t _partsType; // ARM32 offset 12, size 1.
    uint8_t _tileCnt; // ARM32 offset 13, size 1.
    uint8_t _socketCnt; // ARM32 offset 14, size 1.
    CGPoint * _socketList; // ARM32 offset 16, size 4.
}
// Original 0x114cc (0x114cd raw IMP); @8@0:4
- (id)name;
// Original 0x114c0 (0x114c1 raw IMP); v12@0:4C8
- (void)setPartsType:(uint8_t)arg0;
// Original 0x114b4 (0x114b5 raw IMP); C8@0:4
- (uint8_t)partsType;
// Original 0x114a8 (0x114a9 raw IMP); v12@0:4C8
- (void)setTileCnt:(uint8_t)arg0;
// Original 0x1149c (0x1149d raw IMP); C8@0:4
- (uint8_t)tileCnt;
// Original 0x11490 (0x11491 raw IMP); C8@0:4
- (uint8_t)socketCnt;
// Original 0x11468 (0x11469 raw IMP); ^{CGPoint=ff}12@0:4C8
- (CGPoint *)getSocket:(uint8_t)arg0;
// Original 0x11438 (0x11439 raw IMP); v20@0:4C8f12f16
- (void)setSocket:(uint8_t)arg0 X:(float)arg1 Y:(float)arg2;
// Original 0x11408 (0x11409 raw IMP); v12@0:4C8
- (void)setSocketCnt:(uint8_t)arg0;
// Original 0x113fc (0x113fd raw IMP); @8@0:4
- (id)getTileName;
// Original 0x11380 (0x11381 raw IMP); v12@0:4*8
- (void)setPartsName:(char *)arg0;
// Original 0x11344 (0x11345 raw IMP); v8@0:4
- (void)dealloc;
@end

// Original class 0x2ac80; ARM32 instance size 128.
@interface GobMachParts : QobBase {
@public
    GobHvMach * _hvMach; // ARM32 offset 76, size 4.
    Tile2D * _tile; // ARM32 offset 80, size 4.
    PartsInfo * _partsInfo; // ARM32 offset 84, size 4.
    GobMachParts * _baseParts; // ARM32 offset 88, size 4.
    uint8_t _baseSocket; // ARM32 offset 92, size 1.
    CGPoint _basePos; // ARM32 offset 96, size 8.
    CGPoint _offset; // ARM32 offset 104, size 8.
    CGPoint _rotPos; // ARM32 offset 112, size 8.
    int32_t _tileNo; // ARM32 offset 120, size 4.
    float _prevPos; // ARM32 offset 124, size 4.
}
// Original 0x11d64 (0x11d65 raw IMP); C8@0:4
- (uint8_t)baseSocket;
// Original 0x11d34 (0x11d35 raw IMP); {CGPoint=ff}8@0:4
- (CGPoint)rotPos;
// Original 0x11a48 (0x11a49 raw IMP); v8@0:4
- (void)draw;
// Original 0x118fc (0x118fd raw IMP); v8@0:4
- (void)refreshRotationPos;
// Original 0x11818 (0x11819 raw IMP); {CGPoint=ff}12@0:4C8
- (CGPoint)getSocketPos:(uint8_t)arg0;
// Original 0x11798 (0x11799 raw IMP); v16@0:4@8C12
- (void)setBaseParts:(id)arg0 Socket:(uint8_t)arg1;
// Original 0x11748 (0x11749 raw IMP); v16@0:4@8C12
- (void)setChildParts:(id)arg0 Socket:(uint8_t)arg1;
// Original 0x1173c (0x1173d raw IMP); @8@0:4
- (id)getPartsInfo;
// Original 0x11554 (0x11555 raw IMP); v12@0:4@8
- (void)setPartsInfo:(id)arg0;
// Original 0x11548 (0x11549 raw IMP); v12@0:4@8
- (void)setHvMach:(id)arg0;
// Original 0x114d8 (0x114d9 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2aea8; ARM32 instance size 200.
@interface GobMachWeapon : GobMachParts {
@public
    WeaponInfo * _weaponInfo; // ARM32 offset 128, size 4.
    CGPoint _muzzlePos; // ARM32 offset 132, size 8.
    CGPoint _fireOffset; // ARM32 offset 140, size 8.
    CGPoint _weaponVec; // ARM32 offset 148, size 8.
    CGPoint _fireTarget; // ARM32 offset 156, size 8.
    float _aimDir; // ARM32 offset 164, size 4.
    float _aimDirDest; // ARM32 offset 168, size 4.
    float _fireTime; // ARM32 offset 172, size 4.
    int32_t _fireCount; // ARM32 offset 176, size 4.
    float _fireDelay; // ARM32 offset 180, size 4.
    bool _isFire; // ARM32 offset 184, size 1.
    bool _isReload; // ARM32 offset 185, size 1.
    int32_t _aiStep; // ARM32 offset 188, size 4.
    float _resetTime; // ARM32 offset 192, size 4.
    float _delayTime; // ARM32 offset 196, size 4.
}
// Original 0x13a24 (0x13a25 raw IMP); {CGPoint=ff}8@0:4
- (CGPoint)muzzlePos;
// Original 0x13874 (0x13875 raw IMP); v8@0:4
- (void)draw;
// Original 0x13530 (0x13531 raw IMP); v8@0:4
- (void)refreshRotationPos;
// Original 0x1335c (0x1335d raw IMP); v8@0:4
- (void)refreshMuzzlePos;
// Original 0x13214 (0x13215 raw IMP); v8@0:4
- (void)fireWeapon;
// Original 0x131f4 (0x131f5 raw IMP); v20@0:4B8f12f16
- (void)setFire:(bool)arg0 X:(float)arg1 Y:(float)arg2;
// Original 0x13168 (0x13169 raw IMP); v12@0:4f8
- (void)setAimDir:(float)arg0;
// Original 0x1304c (0x1304d raw IMP); v12@0:4@8
- (void)setPartsInfo:(id)arg0;
// Original 0x12a40 (0x12a41 raw IMP); v8@0:4
- (void)processAI;
// Original 0x11e64 (0x11e65 raw IMP); v8@0:4
- (void)tick;
// Original 0x11d70 (0x11d71 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2b1c0; ARM32 instance size 16.
@interface SoundManager : NSObject {
@public
    NSMutableDictionary * _dictSound; // ARM32 offset 4, size 4.
    CGPoint _listnerPos; // ARM32 offset 8, size 8.
}
// Original 0x13d2c (0x13d2d raw IMP); v12@0:4@8
- (void)playBGM:(id)arg0;
// Original 0x13d0c (0x13d0d raw IMP); v20@0:4L8f12f16
- (void)play:(uint32_t)arg0 X:(float)arg1 Y:(float)arg2;
// Original 0x13cee (0x13cef raw IMP); v12@0:4L8
- (void)play:(uint32_t)arg0;
// Original 0x13cec (0x13ced raw IMP); v12@0:4L8
- (void)removeSound:(uint32_t)arg0;
// Original 0x13cd0 (0x13cd1 raw IMP); v8@0:4
- (void)removeAllSounds;
// Original 0x13c08 (0x13c09 raw IMP); L12@0:4@8
- (uint32_t)getSound:(id)arg0;
// Original 0x13bfc (0x13bfd raw IMP); v12@0:4f8
- (void)setMaxDistance:(float)arg0;
// Original 0x13bd0 (0x13bd1 raw IMP); v16@0:4f8f12
- (void)setListnerPosX:(float)arg0 Y:(float)arg1;
// Original 0x13b90 (0x13b91 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x13b1c (0x13b1d raw IMP); @8@0:4
- (id)init;
// Original 0x13a6c (0x13a6d raw IMP); @8@0:4
+ (id)sharedMgr;
@end

// Original class 0x2b0fc; ARM32 instance size 8.
@interface SoundID : NSObject {
@public
    uint32_t _soundID; // ARM32 offset 4, size 4.
}
// Original 0x13a60 (0x13a61 raw IMP); v12@0:4L8
- (void)setSoundID:(uint32_t)arg0;
// Original 0x13a54 (0x13a55 raw IMP); L8@0:4
- (uint32_t)soundID;
@end

// Original class 0x2b514; ARM32 instance size 24.
@interface QVisualLayer : NSObject {
@public
    QobBase * * _visualPool; // ARM32 offset 4, size 4.
    bool _zOrderSort; // ARM32 offset 8, size 1.
    int32_t _unitSize; // ARM32 offset 12, size 4.
    int32_t _poolSize; // ARM32 offset 16, size 4.
    int32_t _visualCnt; // ARM32 offset 20, size 4.
}
// Original 0x14f78 (0x14f79 raw IMP); v12@0:4B8
- (void)setZSort:(bool)arg0;
// Original 0x14f6c (0x14f6d raw IMP); B8@0:4
- (bool)zSort;
// Original 0x14f60 (0x14f61 raw IMP); i8@0:4
- (int32_t)visualCnt;
// Original 0x14f10 (0x14f11 raw IMP); v8@0:4
- (void)drawVisual;
// Original 0x14f00 (0x14f01 raw IMP); v8@0:4
- (void)clearAllVisual;
// Original 0x14e8c (0x14e8d raw IMP); v8@0:4
- (void)extendPool;
// Original 0x14e54 (0x14e55 raw IMP); @12@0:4i8
- (id)getObject:(int32_t)arg0;
// Original 0x14dfc (0x14dfd raw IMP); v12@0:4@8
- (void)addObject:(id)arg0;
// Original 0x14c5c (0x14c5d raw IMP); v12@0:4@8
- (void)insertObject:(id)arg0;
// Original 0x14c28 (0x14c29 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x14bc0 (0x14bc1 raw IMP); @12@0:4i8
- (id)initWithUnitSize:(int32_t)arg0;
@end

// Original class 0x2b6f0; ARM32 instance size 68.
@interface WeaponInfo : NSObject {
@public
    int16_t _wpnType; // ARM32 offset 4, size 2.
    int16_t _dmg; // ARM32 offset 6, size 2.
    bool _pierce; // ARM32 offset 8, size 1.
    uint8_t _shotCnt; // ARM32 offset 9, size 1.
    float _shotDly; // ARM32 offset 12, size 4.
    float _delay; // ARM32 offset 16, size 4.
    float _spd; // ARM32 offset 20, size 4.
    float _spdRand; // ARM32 offset 24, size 4.
    float _aimRand; // ARM32 offset 28, size 4.
    float _g; // ARM32 offset 32, size 4.
    int16_t _charge; // ARM32 offset 36, size 2.
    float _liveTm; // ARM32 offset 40, size 4.
    bool _rotWpn; // ARM32 offset 44, size 1.
    uint8_t _uiTile; // ARM32 offset 45, size 1.
    Tile2D * _bulletTile; // ARM32 offset 48, size 4.
    int16_t _bulletTileNo; // ARM32 offset 52, size 2.
    int16_t _effectType; // ARM32 offset 54, size 2.
    Tile2D * _effectTile; // ARM32 offset 56, size 4.
    int16_t _effectTileNo; // ARM32 offset 60, size 2.
    int16_t _shell; // ARM32 offset 62, size 2.
    uint32_t _fireSFX; // ARM32 offset 64, size 4.
}
// Original 0x15194 (0x15195 raw IMP); v12@0:4s8
- (void)setWpnType:(int16_t)arg0;
// Original 0x15188 (0x15189 raw IMP); s8@0:4
- (int16_t)wpnType;
// Original 0x1517c (0x1517d raw IMP); v12@0:4s8
- (void)setDmg:(int16_t)arg0;
// Original 0x15170 (0x15171 raw IMP); s8@0:4
- (int16_t)dmg;
// Original 0x15164 (0x15165 raw IMP); v12@0:4B8
- (void)setPierce:(bool)arg0;
// Original 0x15158 (0x15159 raw IMP); B8@0:4
- (bool)pierce;
// Original 0x1514c (0x1514d raw IMP); v12@0:4C8
- (void)setShotCnt:(uint8_t)arg0;
// Original 0x15140 (0x15141 raw IMP); C8@0:4
- (uint8_t)shotCnt;
// Original 0x15134 (0x15135 raw IMP); v12@0:4f8
- (void)setShotDly:(float)arg0;
// Original 0x15128 (0x15129 raw IMP); f8@0:4
- (float)shotDly;
// Original 0x1511c (0x1511d raw IMP); v12@0:4f8
- (void)setDelay:(float)arg0;
// Original 0x15110 (0x15111 raw IMP); f8@0:4
- (float)delay;
// Original 0x15104 (0x15105 raw IMP); v12@0:4f8
- (void)setSpd:(float)arg0;
// Original 0x150f8 (0x150f9 raw IMP); f8@0:4
- (float)spd;
// Original 0x150ec (0x150ed raw IMP); v12@0:4f8
- (void)setSpdRand:(float)arg0;
// Original 0x150e0 (0x150e1 raw IMP); f8@0:4
- (float)spdRand;
// Original 0x150d4 (0x150d5 raw IMP); v12@0:4f8
- (void)setAimRand:(float)arg0;
// Original 0x150c8 (0x150c9 raw IMP); f8@0:4
- (float)aimRand;
// Original 0x150bc (0x150bd raw IMP); v12@0:4f8
- (void)setG:(float)arg0;
// Original 0x150b0 (0x150b1 raw IMP); f8@0:4
- (float)g;
// Original 0x150a4 (0x150a5 raw IMP); v12@0:4s8
- (void)setCharge:(int16_t)arg0;
// Original 0x15098 (0x15099 raw IMP); s8@0:4
- (int16_t)charge;
// Original 0x1508c (0x1508d raw IMP); v12@0:4f8
- (void)setLiveTm:(float)arg0;
// Original 0x15080 (0x15081 raw IMP); f8@0:4
- (float)liveTm;
// Original 0x15074 (0x15075 raw IMP); v12@0:4B8
- (void)setRotWpn:(bool)arg0;
// Original 0x15068 (0x15069 raw IMP); B8@0:4
- (bool)rotWpn;
// Original 0x1505c (0x1505d raw IMP); v12@0:4C8
- (void)setUiTile:(uint8_t)arg0;
// Original 0x15050 (0x15051 raw IMP); C8@0:4
- (uint8_t)uiTile;
// Original 0x15044 (0x15045 raw IMP); @8@0:4
- (id)bulletTile;
// Original 0x15038 (0x15039 raw IMP); v12@0:4s8
- (void)setBulletTileNo:(int16_t)arg0;
// Original 0x1502c (0x1502d raw IMP); s8@0:4
- (int16_t)bulletTileNo;
// Original 0x15020 (0x15021 raw IMP); v12@0:4s8
- (void)setEffectType:(int16_t)arg0;
// Original 0x15014 (0x15015 raw IMP); s8@0:4
- (int16_t)effectType;
// Original 0x15008 (0x15009 raw IMP); @8@0:4
- (id)effectTile;
// Original 0x14ffc (0x14ffd raw IMP); v12@0:4s8
- (void)setEffectTileNo:(int16_t)arg0;
// Original 0x14ff0 (0x14ff1 raw IMP); s8@0:4
- (int16_t)effectTileNo;
// Original 0x14fe4 (0x14fe5 raw IMP); v12@0:4s8
- (void)setShell:(int16_t)arg0;
// Original 0x14fd8 (0x14fd9 raw IMP); s8@0:4
- (int16_t)shell;
// Original 0x14fcc (0x14fcd raw IMP); L8@0:4
- (uint32_t)fireSFX;
// Original 0x14fc0 (0x14fc1 raw IMP); v12@0:4L8
- (void)setFireSFX:(uint32_t)arg0;
// Original 0x14f9c (0x14f9d raw IMP); v20@0:4s8@12s16
- (void)setEffectType:(int16_t)arg0 Tile:(id)arg1 TileNo:(int16_t)arg2;
// Original 0x14f84 (0x14f85 raw IMP); v16@0:4@8s12
- (void)setBulletTile:(id)arg0 TileNo:(int16_t)arg1;
@end

// Original class 0x2bc28; ARM32 instance size 276.
@interface GuiGame : QobBase {
@public
    QobBase * _baseSubMenu; // ARM32 offset 76, size 4.
    QobButton * _subBtn[3]; // ARM32 offset 80, size 12.
    int32_t _menuStep; // ARM32 offset 92, size 4.
    QobBase * _baseMissionText; // ARM32 offset 96, size 4.
    QobImage * _imgMission; // ARM32 offset 100, size 4.
    QobImageFont * _imgFntStage; // ARM32 offset 104, size 4.
    QobImage * _imgHPGauge; // ARM32 offset 108, size 4.
    QobImage * _imgReloadGauge; // ARM32 offset 112, size 4.
    QobImage * _imgGear; // ARM32 offset 116, size 4.
    QobImage * _imgLight; // ARM32 offset 120, size 4.
    QobImage * _imgBlueLight; // ARM32 offset 124, size 4.
    QobImage * _imgExpGauge; // ARM32 offset 128, size 4.
    QobImageFont * _imgFontLevel; // ARM32 offset 132, size 4.
    QobImageFont * _imgFontCharge; // ARM32 offset 136, size 4.
    QobImage * _imgSelWeapon; // ARM32 offset 140, size 4.
    QobImage * _imgSelBox; // ARM32 offset 144, size 4.
    QobImage * _imgWeapons[5]; // ARM32 offset 148, size 20.
    float _timeChangeWeapon; // ARM32 offset 168, size 4.
    float _timeGameStart; // ARM32 offset 172, size 4.
    TPassiveSlot _passiveSlot[4]; // ARM32 offset 176, size 64.
    float _gearRot; // ARM32 offset 240, size 4.
    float _reloadTime; // ARM32 offset 244, size 4.
    float _hpGauge; // ARM32 offset 248, size 4.
    float _hpDest; // ARM32 offset 252, size 4.
    TUpgradeUI _upgradeUI; // ARM32 offset 256, size 20.
}
// Original 0x17c4c (0x17c4d raw IMP); v12@0:4@8
- (void)handleButton:(id)arg0;
// Original 0x178f0 (0x178f1 raw IMP); v8@0:4
- (void)changeWeapon;
// Original 0x178b8 (0x178b9 raw IMP); v12@0:4i8
- (void)setChargeCnt:(int32_t)arg0;
// Original 0x17888 (0x17889 raw IMP); v12@0:4@8
- (void)setWeaponInfo:(id)arg0;
// Original 0x17870 (0x17871 raw IMP); B12@0:4i8
- (bool)isActivePassive:(int32_t)arg0;
// Original 0x1778c (0x1778d raw IMP); v12@0:4*8
- (void)addPassiveItem:(char *)arg0;
// Original 0x176b8 (0x176b9 raw IMP); v8@0:4
- (void)processPassiveSlot;
// Original 0x1767c (0x1767d raw IMP); v8@0:4
- (void)setReloadComplete;
// Original 0x1762c (0x1762d raw IMP); v12@0:4f8
- (void)setReloadGauge:(float)arg0;
// Original 0x17620 (0x17621 raw IMP); v12@0:4f8
- (void)setHPGauge:(float)arg0;
// Original 0x1755c (0x1755d raw IMP); v12@0:4f8
- (void)setExpGauge:(float)arg0;
// Original 0x17280 (0x17281 raw IMP); v12@0:4B8
- (void)refreshUpgradeGauge:(bool)arg0;
// Original 0x16a98 (0x16a99 raw IMP); v8@0:4
- (void)setUpgrade;
// Original 0x169b0 (0x169b1 raw IMP); v8@0:4
- (void)setGameStart;
// Original 0x161d0 (0x161d1 raw IMP); v8@0:4
- (void)tick;
// Original 0x16188 (0x16189 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x151a0 (0x151a1 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2bf8c; ARM32 instance size 120.
@interface GuiSelectSlot : QobBase {
@public
    ResMgr_Tile * _tileResMgr; // ARM32 offset 76, size 4.
    int32_t _sel; // ARM32 offset 80, size 4.
    TSaveSlotData * _pSelSlot; // ARM32 offset 84, size 4.
    QobImage * _imgSelSlot; // ARM32 offset 88, size 4.
    QobImage * _imgDelSlotBG; // ARM32 offset 92, size 4.
    QobBase * _slotBase[3]; // ARM32 offset 96, size 12.
    GobHvMach * _mach[3]; // ARM32 offset 108, size 12.
}
// Original 0x18fc0 (0x18fc1 raw IMP); v12@0:4@8
- (void)handleButton:(id)arg0;
// Original 0x186d8 (0x186d9 raw IMP); v12@0:4i8
- (void)refreshSlot:(int32_t)arg0;
// Original 0x18668 (0x18669 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x18158 (0x18159 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2c0e8; ARM32 instance size 484.
@interface GuiSelectMission : QobBase {
@public
    ResMgr_Tile * _tileResMgr; // ARM32 offset 76, size 4.
    int32_t _topIndex; // ARM32 offset 80, size 4.
    CGPoint _slotPos[10]; // ARM32 offset 84, size 80.
    QobImage * _imgSlot[10]; // ARM32 offset 164, size 40.
    QobButton * _btnMission[10]; // ARM32 offset 204, size 40.
    QobImage * _imgStage[10][6]; // ARM32 offset 244, size 240.
}
// Original 0x1a05c (0x1a05d raw IMP); v12@0:4@8
- (void)handleButton:(id)arg0;
// Original 0x19ea4 (0x19ea5 raw IMP); v12@0:4i8
- (void)setTop:(int32_t)arg0;
// Original 0x19d18 (0x19d19 raw IMP); v8@0:4
- (void)refreshStageImg;
// Original 0x19ba0 (0x19ba1 raw IMP); v8@0:4
- (void)tick;
// Original 0x19b30 (0x19b31 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x192ec (0x192ed raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2c23c; ARM32 instance size 88.
@interface GuiSelectMode : QobBase {
@public
    ResMgr_Tile * _tileResMgr; // ARM32 offset 76, size 4.
    QobButton * _btnScenario; // ARM32 offset 80, size 4.
    QobButton * _btnSurvaval; // ARM32 offset 84, size 4.
}
// Original 0x1a900 (0x1a901 raw IMP); v12@0:4@8
- (void)handleButton:(id)arg0;
// Original 0x1a890 (0x1a891 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x1a55c (0x1a55d raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2c358; ARM32 instance size 131.
@interface GobTrailEffect : QobImage {
@public
    float _blockWidth; // ARM32 offset 88, size 4.
    float _blockLen; // ARM32 offset 92, size 4.
    int32_t _blockCnt; // ARM32 offset 96, size 4.
    float _fadeDelay; // ARM32 offset 100, size 4.
    float _fadeTime; // ARM32 offset 104, size 4.
    CGPoint * _coords; // ARM32 offset 108, size 4.
    CGPoint * _vertices; // ARM32 offset 112, size 4.
    float * _colors; // ARM32 offset 116, size 4.
    TTrailPos * _posArray; // ARM32 offset 120, size 4.
    int32_t _lastPos; // ARM32 offset 124, size 4.
    bool _oddCoord; // ARM32 offset 128, size 1.
    bool _windEffect; // ARM32 offset 129, size 1.
    bool _diffuseEffect; // ARM32 offset 130, size 1.
}
// Original 0x1ba00 (0x1ba01 raw IMP); v12@0:4B8
- (void)setWindEffect:(bool)arg0;
// Original 0x1b9f4 (0x1b9f5 raw IMP); B8@0:4
- (bool)windEffect;
// Original 0x1b9e8 (0x1b9e9 raw IMP); v12@0:4B8
- (void)setDiffuseEffect:(bool)arg0;
// Original 0x1b9dc (0x1b9dd raw IMP); B8@0:4
- (bool)diffuseEffect;
// Original 0x1b8dc (0x1b8dd raw IMP); v8@0:4
- (void)draw;
// Original 0x1b8d8 (0x1b8d9 raw IMP); B8@0:4
- (bool)isInCam;
// Original 0x1b5c0 (0x1b5c1 raw IMP); v8@0:4
- (void)tick;
// Original 0x1b438 (0x1b439 raw IMP); v16@0:4f8f12
- (void)addTrailPosX:(float)arg0 Y:(float)arg1;
// Original 0x1acd0 (0x1acd1 raw IMP); v20@0:4f8f12B16
- (void)addValidPosX:(float)arg0 Y:(float)arg1 IsAdd:(bool)arg2;
// Original 0x1ac60 (0x1ac61 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x1ab60 (0x1ab61 raw IMP); v28@0:4f8f12i16f20f24
- (void)setTrailWidth:(float)arg0 Len:(float)arg1 BlockCnt:(int32_t)arg2 FadeDelay:(float)arg3 FadeTime:(float)arg4;
// Original 0x1ab20 (0x1ab21 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2c59c; ARM32 instance size 20.
@interface QList : NSObject {
@public
    void * * _dataPool; // ARM32 offset 4, size 4.
    int16_t _unitSize; // ARM32 offset 8, size 2.
    int32_t _poolSize; // ARM32 offset 12, size 4.
    int32_t _dataCnt; // ARM32 offset 16, size 4.
}
// Original 0x1bb78 (0x1bb79 raw IMP); i8@0:4
- (int32_t)size;
// Original 0x1bb0c (0x1bb0d raw IMP); v8@0:4
- (void)extendPool;
// Original 0x1bae4 (0x1bae5 raw IMP); ^v12@0:4i8
- (void *)getData:(int32_t)arg0;
// Original 0x1baa0 (0x1baa1 raw IMP); v12@0:4^v8
- (void)addData:(void *)arg0;
// Original 0x1ba6c (0x1ba6d raw IMP); v8@0:4
- (void)dealloc;
// Original 0x1ba0c (0x1ba0d raw IMP); @12@0:4i8
- (id)initWithUnitSize:(int32_t)arg0;
@end

// Original class 0x2c85c; ARM32 instance size 97.
@interface GobItemBox : QobImage {
@public
    Vector2 * _vel; // ARM32 offset 88, size 4.
    Vector2 * _velTick; // ARM32 offset 92, size 4.
    bool _landed; // ARM32 offset 96, size 1.
}
// Original 0x1d6c8 (0x1d6c9 raw IMP); v8@0:4
- (void)tick;
// Original 0x1d67c (0x1d67d raw IMP); v8@0:4
- (void)dealloc;
// Original 0x1d538 (0x1d539 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x278c8; ARM32 instance size 3804.
@interface GameMain : QobBase {
@public
    int32_t _screen; // ARM32 offset 76, size 4.
    int32_t _gameState; // ARM32 offset 80, size 4.
    int32_t _scrChangeState; // ARM32 offset 84, size 4.
    GobWorld * _world; // ARM32 offset 88, size 4.
    GuiGame * _uiGame; // ARM32 offset 92, size 4.
    GuiTitle * _uiTitle; // ARM32 offset 96, size 4.
    GuiSelectMode * _uiSelMode; // ARM32 offset 100, size 4.
    GuiSelectSlot * _uiSelSlot; // ARM32 offset 104, size 4.
    GuiSelectMission * _uiSelMission; // ARM32 offset 108, size 4.
    QobBase * _layerLoading; // ARM32 offset 112, size 4.
    QobImage * _imgLoading[4]; // ARM32 offset 116, size 16.
    CGPoint _loadingPt[4]; // ARM32 offset 132, size 32.
    uint32_t _sfxLoadingOpen; // ARM32 offset 164, size 4.
    uint32_t _sfxLoadingClose; // ARM32 offset 168, size 4.
    TDataFile _saveData; // ARM32 offset 172, size 3624.
    double _startTime; // ARM32 offset 3796, size 8.
}
// Original 0x3fc4 (0x3fc5 raw IMP); i8@0:4
- (int32_t)screen;
// Original 0x3fb8 (0x3fb9 raw IMP); @8@0:4
- (id)getGameUI;
// Original 0x3fac (0x3fad raw IMP); @8@0:4
- (id)getWorld;
// Original 0x3f44 (0x3f45 raw IMP); v8@0:4
- (void)saveDataFile;
// Original 0x3ea0 (0x3ea1 raw IMP); v8@0:4
- (void)openDataFile;
// Original 0x3e7c (0x3e7d raw IMP); ^{TSaveSlotData=iiBiiiiiifiCCCCi[32{TWeaponItem=[24c]i}]s[64i]}12@0:4i8
- (TSaveSlotData *)slotData:(int32_t)arg0;
// Original 0x3b20 (0x3b21 raw IMP); v12@0:4i8
- (void)selectSlot:(int32_t)arg0;
// Original 0x3a1c (0x3a1d raw IMP); v8@0:4
- (void)setScreen_Game;
// Original 0x39d8 (0x39d9 raw IMP); v8@0:4
- (void)setScreen_SelectMission;
// Original 0x3994 (0x3995 raw IMP); v8@0:4
- (void)setScreen_SelectSlot;
// Original 0x3950 (0x3951 raw IMP); v8@0:4
- (void)setScreen_SelectMode;
// Original 0x390c (0x390d raw IMP); v8@0:4
- (void)setScreen_Title;
// Original 0x38b0 (0x38b1 raw IMP); v8@0:4
- (void)refreshScreen;
// Original 0x3890 (0x3891 raw IMP); v12@0:4i8
- (void)changeScreen:(int32_t)arg0;
// Original 0x3778 (0x3779 raw IMP); v8@0:4
- (void)clearAllScreen;
// Original 0x3374 (0x3375 raw IMP); v8@0:4
- (void)tick;
// Original 0x3320 (0x3321 raw IMP); v8@0:4
- (void)dealloc;
// Original 0x2ecc (0x2ecd raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x27bd0; ARM32 instance size 148.
@interface GobHvMach : QobBase {
@public
    QVisualLayer * _layer; // ARM32 offset 76, size 4.
    int16_t _machState; // ARM32 offset 80, size 2.
    uint8_t _machType; // ARM32 offset 82, size 1.
    int32_t _hp; // ARM32 offset 84, size 4.
    float _speed; // ARM32 offset 88, size 4.
    float _moveSpeed; // ARM32 offset 92, size 4.
    GobMachParts * _partsBody; // ARM32 offset 96, size 4.
    GobMachParts * _partsTop; // ARM32 offset 100, size 4.
    GobMachWeapon * _partsWeapon; // ARM32 offset 104, size 4.
    Vector2 * _vel; // ARM32 offset 108, size 4.
    Vector2 * _velTick; // ARM32 offset 112, size 4.
    ColVector * _currentCol; // ARM32 offset 116, size 4.
    ColVector * _prevCol; // ARM32 offset 120, size 4.
    float _colRotate; // ARM32 offset 124, size 4.
    float _boosterVel; // ARM32 offset 128, size 4.
    int16_t _useBooster; // ARM32 offset 132, size 2.
    float _dashVel; // ARM32 offset 136, size 4.
    bool _useEngineBounce; // ARM32 offset 140, size 1.
    float _engineBounce; // ARM32 offset 144, size 4.
}
// Original 0x5868 (0x5869 raw IMP); v12@0:4C8
- (void)setMachType:(uint8_t)arg0;
// Original 0x585c (0x585d raw IMP); C8@0:4
- (uint8_t)machType;
// Original 0x5850 (0x5851 raw IMP); v12@0:4f8
- (void)setSpeed:(float)arg0;
// Original 0x5844 (0x5845 raw IMP); f8@0:4
- (float)speed;
// Original 0x5838 (0x5839 raw IMP); v12@0:4s8
- (void)setState:(int16_t)arg0;
// Original 0x582c (0x582d raw IMP); s8@0:4
- (int16_t)state;
// Original 0x5820 (0x5821 raw IMP); v12@0:4B8
- (void)setUseEngineBounce:(bool)arg0;
// Original 0x5814 (0x5815 raw IMP); B8@0:4
- (bool)useEngineBounce;
// Original 0x5808 (0x5809 raw IMP); f8@0:4
- (float)engineBounce;
// Original 0x57c0 (0x57c1 raw IMP); v8@0:4
- (void)draw;
// Original 0x5704 (0x5705 raw IMP); v12@0:4f8
- (void)setMoveVel:(float)arg0;
// Original 0x55fc (0x55fd raw IMP); v8@0:4
- (void)onDestroy;
// Original 0x54b8 (0x54b9 raw IMP); v12@0:4i8
- (void)onDamage:(int32_t)arg0;
// Original 0x5240 (0x5241 raw IMP); v8@0:4
- (void)processFly;
// Original 0x4c10 (0x4c11 raw IMP); v8@0:4
- (void)processLand;
// Original 0x4b5c (0x4b5d raw IMP); v8@0:4
- (void)processModel;
// Original 0x4ab0 (0x4ab1 raw IMP); v12@0:4@8
- (void)setCurrentCol:(id)arg0;
// Original 0x479c (0x479d raw IMP); v8@0:4
- (void)tick;
// Original 0x428c (0x428d raw IMP); @20@0:4@8@12C16
- (id)addParts:(id)arg0 BaseParts:(id)arg1 BaseSocket:(uint8_t)arg2;
// Original 0x4214 (0x4215 raw IMP); @12@0:4@8
- (id)addParts:(id)arg0;
// Original 0x41f8 (0x41f9 raw IMP); v12@0:4@8
- (void)insertPartsToLayer:(id)arg0;
// Original 0x41b8 (0x41b9 raw IMP); B8@0:4
- (bool)isPlayer;
// Original 0x415c (0x415d raw IMP); v8@0:4
- (void)dealloc;
// Original 0x3fd0 (0x3fd1 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2b31c; ARM32 instance size 180.
@interface GobHvM_Enemy : GobHvMach {
@public
    bool _isBoss; // ARM32 offset 148, size 1.
    int32_t _hpMax; // ARM32 offset 152, size 4.
    float _gaugeSize; // ARM32 offset 156, size 4.
    int32_t _giveItem; // ARM32 offset 160, size 4.
    int32_t _giveExp; // ARM32 offset 164, size 4.
    float _speedDest; // ARM32 offset 168, size 4.
    float _gravityDest; // ARM32 offset 172, size 4.
    QobImage * _imgHpGauge; // ARM32 offset 176, size 4.
}
// Original 0x14bb4 (0x14bb5 raw IMP); v12@0:4B8
- (void)setIsBoss:(bool)arg0;
// Original 0x14ba8 (0x14ba9 raw IMP); B8@0:4
- (bool)isBoss;
// Original 0x14b9c (0x14b9d raw IMP); v12@0:4i8
- (void)setGiveItem:(int32_t)arg0;
// Original 0x14b90 (0x14b91 raw IMP); i8@0:4
- (int32_t)giveItem;
// Original 0x14b84 (0x14b85 raw IMP); v12@0:4i8
- (void)setGiveExp:(int32_t)arg0;
// Original 0x14b78 (0x14b79 raw IMP); i8@0:4
- (int32_t)giveExp;
// Original 0x14918 (0x14919 raw IMP); B12@0:4*8
- (bool)makeMachFromName:(char *)arg0;
// Original 0x147e8 (0x147e9 raw IMP); i20@0:4@8*12i16
- (int32_t)decodeChunk:(id)arg0 Data:(char *)arg1 Pos:(int32_t)arg2;
// Original 0x14778 (0x14779 raw IMP); v12@0:4f8
- (void)setHPGauge:(float)arg0;
// Original 0x14418 (0x14419 raw IMP); v8@0:4
- (void)onDestroy;
// Original 0x143b8 (0x143b9 raw IMP); v12@0:4i8
- (void)onDamage:(int32_t)arg0;
// Original 0x13e04 (0x13e05 raw IMP); v8@0:4
- (void)tick;
// Original 0x13d80 (0x13d81 raw IMP); @8@0:4
- (id)init;
@end

// Original class 0x2c6d8; ARM32 instance size 160.
@interface GobHvM_Player : GobHvMach {
@public
    QobImage * _imgHpGauge; // ARM32 offset 148, size 4.
    float _safeBackLine; // ARM32 offset 152, size 4.
    float _removeBgLine; // ARM32 offset 156, size 4.
}
// Original 0x1d300 (0x1d301 raw IMP); v20@0:4{CGPoint=ff}8i16
- (void)setTap:(CGPoint)arg0 State:(int32_t)arg1;
// Original 0x1d278 (0x1d279 raw IMP); v12@0:4f8
- (void)setHPGauge:(float)arg0;
// Original 0x1d230 (0x1d231 raw IMP); v8@0:4
- (void)onDestroy;
// Original 0x1d140 (0x1d141 raw IMP); v12@0:4i8
- (void)onDamage:(int32_t)arg0;
// Original 0x1d0dc (0x1d0dd raw IMP); v8@0:4
- (void)refreshMoveSpeed;
// Original 0x1cf58 (0x1cf59 raw IMP); v8@0:4
- (void)refreshMaxHp;
// Original 0x1cc70 (0x1cc71 raw IMP); v12@0:4B8
- (void)setBooster:(bool)arg0;
// Original 0x1c64c (0x1c64d raw IMP); v12@0:4@8
- (void)pickupItem:(id)arg0;
// Original 0x1c408 (0x1c409 raw IMP); v8@0:4
- (void)removeCurWeapon;
// Original 0x1c25c (0x1c25d raw IMP); v12@0:4i8
- (void)changeWeapon:(int32_t)arg0;
// Original 0x1bfdc (0x1bfdd raw IMP); v12@0:4i8
- (void)upgradeParts:(int32_t)arg0;
// Original 0x1be84 (0x1be85 raw IMP); @12@0:4@8
- (id)setPartsToMyMach:(id)arg0;
// Original 0x1be0c (0x1be0d raw IMP); v8@0:4
- (void)refreshMyMachLayer;
// Original 0x1bd04 (0x1bd05 raw IMP); v8@0:4
- (void)tick;
// Original 0x1bb84 (0x1bb85 raw IMP); @8@0:4
- (id)init;
@end
