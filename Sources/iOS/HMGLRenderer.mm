#import "HMGLRenderer.h"
#import "HMMedia.h"
#include "HMCompatibilityPixels.hpp"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

namespace {
struct Texture {GLuint id=0;GLsizei width=0,height=0,pw=0,ph=0;};
static GLsizei pot(GLsizei n){GLsizei p=1;while(p<n)p*=2;return p;}
static NSString *glString(GLenum name){const auto p=glGetString(name);return p?[NSString stringWithUTF8String:(const char*)p]:@"unavailable";}
}
@implementation HMGLRenderer {
    EAGLContext *_context,*_previousContext;
    BOOL _active;
    GLuint _sceneFBO,_sceneBuffer,_sceneTexture,_displayFBO,_displayBuffer;
    BOOL _sceneTextureAllocated;
    CGFloat _logicalWidth,_logicalHeight,_drawingScale;
    GLsizei _bufferWidth,_bufferHeight;
    CGPoint _drawingOffset;
    CGRect _clipRect;
    BOOL _hasClip;
    NSUInteger _sceneReallocations;
    double _nextPresentDue;
    GLint _displayWidth,_displayHeight,_displayFormat;
    __weak CAEAGLLayer *_displayLayer;
    CGSize _drawablePointSize;
    CGFloat _drawableScale;
    std::map<std::string,Texture> _textures;
    NSString *_lastError,*_vendor,*_renderer,*_version;
    NSUInteger _drawCalls,_uploads,_frames;
#if HM_VALIDATION
    double _presentStart, _presentCopy, _presentQuad, _presentSwap, _presentWait;
#endif
}
- (void)recordError:(NSString *)message { _lastError=message;NSLog(@"HMGLRenderer: %@",message); }
- (instancetype)init {
    if(!(self=[super init]))return nil;
    _logicalWidth=480;_logicalHeight=320;_bufferWidth=480;_bufferHeight=320;_drawingScale=1;
    _context=[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    if(!_context)return nil;
    EAGLContext *previous=[EAGLContext currentContext];
    if(![EAGLContext setCurrentContext:_context])return nil;
    _vendor=glString(GL_VENDOR);_renderer=glString(GL_RENDERER);_version=glString(GL_VERSION);
    // Match touchHLE EAGL: renderbuffer -> reusable NPOT copy texture -> drawable.
    glGenTextures(1,&_sceneTexture);glBindTexture(GL_TEXTURE_2D,_sceneTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glGenRenderbuffersOES(1,&_sceneBuffer);glBindRenderbufferOES(GL_RENDERBUFFER_OES,_sceneBuffer);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES,GL_RGBA8_OES,_bufferWidth,_bufferHeight);
    _sceneReallocations=1;
    glGenFramebuffersOES(1,&_sceneFBO);glBindFramebufferOES(GL_FRAMEBUFFER_OES,_sceneFBO);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,GL_COLOR_ATTACHMENT0_OES,GL_RENDERBUFFER_OES,_sceneBuffer);
    BOOL ok=glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES)==GL_FRAMEBUFFER_COMPLETE_OES&&glGetError()==GL_NO_ERROR;
    [EAGLContext setCurrentContext:previous];
    if(!ok){[self recordError:@"Logical framebuffer creation failed"];return nil;}
    return self;
}
- (void)dealloc {
    EAGLContext *previous=[EAGLContext currentContext];
    if(_context&&[EAGLContext setCurrentContext:_context]){
        for(auto &entry:_textures)glDeleteTextures(1,&entry.second.id);
        glDeleteTextures(1,&_sceneTexture);glDeleteFramebuffersOES(1,&_sceneFBO);glDeleteRenderbuffersOES(1,&_sceneBuffer);
        glDeleteFramebuffersOES(1,&_displayFBO);glDeleteRenderbuffersOES(1,&_displayBuffer);
        [EAGLContext setCurrentContext:previous==_context?nil:previous];
    }
}
- (BOOL)setLogicalWidth:(CGFloat)width height:(CGFloat)height {
    if(_active||!std::isfinite(width)||!std::isfinite(height)||width<=0||height<=0)return NO;
    if(width==_logicalWidth&&height==_logicalHeight)return YES;
    EAGLContext *previous=[EAGLContext currentContext];
    if(![EAGLContext setCurrentContext:_context])return NO;
    GLint maxTexture=0,maxBuffer=0;glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTexture);glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_OES,&maxBuffer);
    if(std::ceil(width)>std::min(maxTexture,maxBuffer)||std::ceil(height)>std::min(maxTexture,maxBuffer)){
        [EAGLContext setCurrentContext:previous];return NO;
    }
    const GLsizei w=GLsizei(std::ceil(width)),h=GLsizei(std::ceil(height));
    if(w!=_bufferWidth||h!=_bufferHeight){
        // Allocate a replacement first so a failed allocation preserves the old scene.
        GLint previousFBO=0,previousBuffer=0;glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES,&previousFBO);glGetIntegerv(GL_RENDERBUFFER_BINDING_OES,&previousBuffer);
        GLuint buffer=0,fbo=0;glGenRenderbuffersOES(1,&buffer);glBindRenderbufferOES(GL_RENDERBUFFER_OES,buffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES,GL_RGBA8_OES,w,h);
        glGenFramebuffersOES(1,&fbo);glBindFramebufferOES(GL_FRAMEBUFFER_OES,fbo);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,GL_COLOR_ATTACHMENT0_OES,GL_RENDERBUFFER_OES,buffer);
        const BOOL ok=glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES)==GL_FRAMEBUFFER_COMPLETE_OES&&glGetError()==GL_NO_ERROR;
        if(!ok){glDeleteFramebuffersOES(1,&fbo);glDeleteRenderbuffersOES(1,&buffer);glBindFramebufferOES(GL_FRAMEBUFFER_OES,previousFBO);glBindRenderbufferOES(GL_RENDERBUFFER_OES,previousBuffer);[EAGLContext setCurrentContext:previous];return NO;}
        if(previousFBO==GLint(_sceneFBO))previousFBO=fbo;
        if(previousBuffer==GLint(_sceneBuffer))previousBuffer=buffer;
        glDeleteFramebuffersOES(1,&_sceneFBO);glDeleteRenderbuffersOES(1,&_sceneBuffer);
        _sceneFBO=fbo;_sceneBuffer=buffer;_bufferWidth=w;_bufferHeight=h;_sceneTextureAllocated=NO;_sceneReallocations++;
        glBindFramebufferOES(GL_FRAMEBUFFER_OES,previousFBO);glBindRenderbufferOES(GL_RENDERBUFFER_OES,previousBuffer);
    }
    _logicalWidth=width;_logicalHeight=height;
    [EAGLContext setCurrentContext:previous];return YES;
}
- (void)setDrawingOffset:(CGPoint)offset {if(std::isfinite(offset.x)&&std::isfinite(offset.y))_drawingOffset=offset;}
- (void)setDrawingScale:(CGFloat)scale {if(std::isfinite(scale)&&scale>0)_drawingScale=scale;}
- (void)applyClip {
    if(!_hasClip){glDisable(GL_SCISSOR_TEST);return;}
    CGRect r=CGRectIntersection(_clipRect,CGRectMake(0,0,_logicalWidth,_logicalHeight));
    glEnable(GL_SCISSOR_TEST);
    if(CGRectIsNull(r)||CGRectIsEmpty(r)){glScissor(0,0,0,0);return;}
    const double sx=_bufferWidth/_logicalWidth,sy=_bufferHeight/_logicalHeight;
    // Choose pixel centres within the requested logical rectangle.
    const GLint x=GLint(std::ceil(CGRectGetMinX(r)*sx-.5)),y=GLint(std::ceil(CGRectGetMinY(r)*sy-.5));
    const GLint right=GLint(std::ceil(CGRectGetMaxX(r)*sx-.5)),top=GLint(std::ceil(CGRectGetMaxY(r)*sy-.5));
    glScissor(x,y,std::max(0,right-x),std::max(0,top-y));
}
- (void)setClipRect:(CGRect)rect {
    if(!std::isfinite(rect.origin.x)||!std::isfinite(rect.origin.y)||!std::isfinite(rect.size.width)||!std::isfinite(rect.size.height)||rect.size.width<0||rect.size.height<0)rect=CGRectZero;
    _clipRect=rect;_hasClip=YES;if(_active)[self applyClip];
}
- (void)resetClip {_hasClip=NO;if(_active)[self applyClip];}
- (BOOL)beginFrame {
    if(_active){[self recordError:@"Nested beginFrame"];return NO;}
    _previousContext=[EAGLContext currentContext];
    if(![EAGLContext setCurrentContext:_context]){[self recordError:@"Cannot activate ES context"];_previousContext=nil;return NO;}
    _active=YES;_drawCalls=0;
    glBindFramebufferOES(GL_FRAMEBUFFER_OES,_sceneFBO);glViewport(0,0,_bufferWidth,_bufferHeight);
    glDisable(GL_SCISSOR_TEST);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDisable(GL_LIGHTING);glEnable(GL_DITHER);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);[self applyClip];
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrthof(0,_logicalWidth,0,_logicalHeight,-1,1);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();glMatrixMode(GL_TEXTURE);glLoadIdentity();glMatrixMode(GL_MODELVIEW);
    glActiveTexture(GL_TEXTURE0);glClientActiveTexture(GL_TEXTURE0);glEnable(GL_TEXTURE_2D);glEnable(GL_BLEND);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);glEnableClientState(GL_TEXTURE_COORD_ARRAY);glEnableClientState(GL_COLOR_ARRAY);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);glPixelStorei(GL_PACK_ALIGNMENT,1);return YES;
}
- (void)endFrame {
    if(!_active)return;
    GLenum error=glGetError();if(error!=GL_NO_ERROR)[self recordError:[NSString stringWithFormat:@"OpenGL error 0x%x",error]];
    glFlush();[EAGLContext setCurrentContext:_previousContext];_previousContext=nil;_active=NO;_frames++;
}
- (Texture *)textureForImage:(UIImage *)image name:(NSString *)name rgb565:(BOOL)rgb565 {
    if(!name)return nullptr;
    std::string key(name.UTF8String);key+=rgb565?"#565":"#rgba";
    auto found=_textures.find(key);if(found!=_textures.end())return &found->second;
    if(!image.CGImage)return nullptr;
    size_t w=CGImageGetWidth(image.CGImage),h=CGImageGetHeight(image.CGImage);GLint maxSize=0;glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxSize);
    if(!w||!h||w>size_t(maxSize)||h>size_t(maxSize)){[self recordError:@"Invalid texture dimensions"];return nullptr;}
    Texture t;t.width=GLsizei(w);t.height=GLsizei(h);t.pw=pot(t.width);t.ph=pot(t.height);
    std::vector<uint8_t> pixels(size_t(t.pw)*t.ph*4);
    NSData *original=HMOriginalImageRGBA(image);
    if(original.length==w*h*4){
        // touchHLE CGImage pixels are already premultiplied, top row first.
        // Preserve the bytes and zero POT padding without another color conversion.
        const auto *source=static_cast<const uint8_t *>(original.bytes);
        for(size_t y=0;y<h;++y){
            auto *row=pixels.data()+y*size_t(t.pw)*4;
            std::copy_n(source+y*w*4,w*4,row);
            heavymach::compatibilityBitmapPixels(row,w);
        }
    } else {
    CGColorSpaceRef space=CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo flags=CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(rgb565?kCGImageAlphaNoneSkipLast:kCGImageAlphaPremultipliedLast);
    CGContextRef bitmap=CGBitmapContextCreate(pixels.data(),t.pw,t.ph,8,size_t(t.pw)*4,space,flags);CGColorSpaceRelease(space);
    if(!bitmap){[self recordError:@"Texture decode allocation failed"];return nullptr;}
    CGContextClearRect(bitmap,CGRectMake(0,0,t.pw,t.ph));CGContextTranslateCTM(bitmap,0,t.ph-t.height);
    CGContextDrawImage(bitmap,CGRectMake(0,0,t.width,t.height),image.CGImage);CGContextRelease(bitmap);
    }
    glGenTextures(1,&t.id);glBindTexture(GL_TEXTURE_2D,t.id);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    if(rgb565){std::vector<uint16_t> packed(size_t(t.pw)*t.ph);for(size_t i=0;i<packed.size();++i)packed[i]=uint16_t((pixels[i*4]>>3)<<11|(pixels[i*4+1]>>2)<<5|(pixels[i*4+2]>>3));glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,t.pw,t.ph,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,packed.data());}
    else glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,t.pw,t.ph,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
    GLenum error=glGetError();if(error!=GL_NO_ERROR){glDeleteTextures(1,&t.id);[self recordError:[NSString stringWithFormat:@"Texture upload %@ failed 0x%x",name,error]];return nullptr;}
    _uploads++;return &_textures.emplace(key,t).first->second;
}
- (BOOL)hasTexture:(NSString *)name rgb565:(BOOL)rgb565 {
    std::string key(name.UTF8String);key+=rgb565?"#565":"#rgba";return _textures.contains(key);
}
- (BOOL)prepareImage:(UIImage *)image name:(NSString *)name rgb565:(BOOL)rgb565 {
    EAGLContext *previous=[EAGLContext currentContext];
    if(![EAGLContext setCurrentContext:_context])return NO;
    GLint alignment=4;glGetIntegerv(GL_UNPACK_ALIGNMENT,&alignment);glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    BOOL ok=[self textureForImage:image name:name rgb565:rgb565]!=nullptr;
    glPixelStorei(GL_UNPACK_ALIGNMENT,alignment);
    [EAGLContext setCurrentContext:previous];return ok;
}
- (void)releaseTextureScope:(NSString *)scope {
    EAGLContext *previous=[EAGLContext currentContext];
    if(![EAGLContext setCurrentContext:_context])return;
    std::string prefix([[scope stringByAppendingString:@"|"] UTF8String]);
    for(auto i=_textures.begin();i!=_textures.end();) {
        if(i->first.starts_with(prefix)){glDeleteTextures(1,&i->second.id);i=_textures.erase(i);}else ++i;
    }
    [EAGLContext setCurrentContext:previous];
}
- (void)drawImage:(UIImage *)image name:(NSString *)name columns:(NSInteger)columns rows:(NSInteger)rows tile:(NSInteger)tile position:(CGPoint)position scale:(CGSize)scale rotation:(CGFloat)rotation alpha:(CGFloat)alpha blend:(NSInteger)blend rgb565:(BOOL)rgb565 {
    if(!_active||columns<1||rows<1||tile<0||!std::isfinite(position.x)||!std::isfinite(position.y)||!std::isfinite(scale.width)||!std::isfinite(scale.height)||!std::isfinite(rotation)||!std::isfinite(alpha))return;
    Texture *t=[self textureForImage:image name:name rgb565:rgb565];if(!t)return;
    tile=std::min(tile,columns*rows-1);
    GLfloat u=float(t->width)/t->pw/columns,v=float(t->height)/t->ph/rows,hw=t->pw*u*.5f,hh=t->ph*v*.5f;
    GLfloat u0=(tile%columns)*u,v0=(tile/columns)*v;
    GLfloat positions[]={-hw,-hh,hw,-hh,-hw,hh,hw,hh},uv[]={u0,v0+v,u0+u,v0+v,u0,v0,u0+u,v0};
    GLfloat colors[]={1,1,1,float(alpha),1,1,1,float(alpha),1,1,1,float(alpha),1,1,1,float(alpha)};
    glBindTexture(GL_TEXTURE_2D,t->id);
    // Original sprite blend 1 is SRC_COLOR/ONE_MINUS_SRC_ALPHA; unknown values inherit state.
    if(blend==0||blend==3)glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);else if(blend==2)glBlendFunc(GL_SRC_ALPHA,GL_ONE);else if(blend==1)glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_ALPHA);
    glLoadIdentity();glTranslatef(_drawingOffset.x,_drawingOffset.y,0);glScalef(_drawingScale,_drawingScale,1);glTranslatef(position.x,position.y,0);glScalef(scale.width,scale.height,1);glRotatef(rotation*57.29577951308232,0,0,1);
    glVertexPointer(2,GL_FLOAT,0,positions);glTexCoordPointer(2,GL_FLOAT,0,uv);glColorPointer(4,GL_FLOAT,0,colors);glDrawArrays(GL_TRIANGLE_STRIP,0,4);_drawCalls++;
}
- (void)drawTrail:(const heavymach::TrailMesh&)mesh image:(UIImage *)image name:(NSString *)name rgb565:(BOOL)rgb565 cameraOffset:(CGPoint)offset {
    if(!_active||mesh.vertices.size()<3)return;
    Texture *t=[self textureForImage:image name:name rgb565:rgb565];if(!t)return;
    glBindTexture(GL_TEXTURE_2D,t->id);if(mesh.setBlend)glBlendFunc(mesh.sourceFactor,mesh.destinationFactor);
    glLoadIdentity();glTranslatef(_drawingOffset.x,_drawingOffset.y,0);glScalef(_drawingScale,_drawingScale,1);glTranslatef(mesh.translation.x-offset.x,mesh.translation.y-offset.y,0);
    const auto *vertices=mesh.vertices.data();glVertexPointer(2,GL_FLOAT,sizeof(heavymach::TrailVertex),&vertices[0].position.x);
    glTexCoordPointer(2,GL_FLOAT,sizeof(heavymach::TrailVertex),&vertices[0].uv.x);glColorPointer(4,GL_FLOAT,sizeof(heavymach::TrailVertex),vertices[0].color.data());
    glDrawArrays(GL_TRIANGLE_STRIP,0,GLsizei(mesh.vertices.size()));_drawCalls++;
}
- (BOOL)presentToLayer:(CAEAGLLayer *)layer viewport:(CGRect)viewport {
    if(!_active||!layer||CGRectIsEmpty(layer.bounds)||CGRectIsEmpty(viewport))return NO;
    // touchHLE eagl.rs limit_framerate: bounded one-interval slop, default 60 Hz.
    const double now=CACurrentMediaTime(),interval=1.0/60.0;
    double delay=0;
    if(_nextPresentDue==0)_nextPresentDue=now+interval;
    else {
        const double due=_nextPresentDue;
        _nextPresentDue=now>due+interval?due+interval*std::ceil((now-due)/interval):due+interval;
        if(now<due)delay=due-now;
    }

    if(_displayLayer!=layer||!CGSizeEqualToSize(_drawablePointSize,layer.bounds.size)||_drawableScale!=layer.contentsScale){
        glDeleteFramebuffersOES(1,&_displayFBO);glDeleteRenderbuffersOES(1,&_displayBuffer);_displayFBO=0;_displayBuffer=0;_displayLayer=nil;
        glGenRenderbuffersOES(1,&_displayBuffer);glBindRenderbufferOES(GL_RENDERBUFFER_OES,_displayBuffer);
        if(![_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer]){[self recordError:@"Drawable allocation failed"];return NO;}
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,GL_RENDERBUFFER_INTERNAL_FORMAT_OES,&_displayFormat);
        glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,GL_RENDERBUFFER_WIDTH_OES,&_displayWidth);glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,GL_RENDERBUFFER_HEIGHT_OES,&_displayHeight);
        glGenFramebuffersOES(1,&_displayFBO);glBindFramebufferOES(GL_FRAMEBUFFER_OES,_displayFBO);glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,GL_COLOR_ATTACHMENT0_OES,GL_RENDERBUFFER_OES,_displayBuffer);
        if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES)!=GL_FRAMEBUFFER_COMPLETE_OES){[self recordError:@"Drawable framebuffer incomplete"];return NO;}
        _displayLayer=layer;_drawablePointSize=layer.bounds.size;_drawableScale=layer.contentsScale;
    }
#if HM_VALIDATION
    _presentStart=CACurrentMediaTime();
#endif
    GLuint copyFBO=0;glGenFramebuffersOES(1,&copyFBO);glBindFramebufferOES(GL_FRAMEBUFFER_OES,copyFBO);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,GL_COLOR_ATTACHMENT0_OES,GL_RENDERBUFFER_OES,_sceneBuffer);
    glBindTexture(GL_TEXTURE_2D,_sceneTexture);
    if(_sceneTextureAllocated)glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,_bufferWidth,_bufferHeight);
    else {glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,_bufferWidth,_bufferHeight,0);_sceneTextureAllocated=YES;}
    glDeleteFramebuffersOES(1,&copyFBO);
#if HM_VALIDATION
    double afterCopy=CACurrentMediaTime();_presentCopy=(afterCopy-_presentStart)*1000;
#endif
    glDisable(GL_SCISSOR_TEST);glBindFramebufferOES(GL_FRAMEBUFFER_OES,_displayFBO);glViewport(0,0,_displayWidth,_displayHeight);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);
    CGFloat sx=_displayWidth/layer.bounds.size.width,sy=_displayHeight/layer.bounds.size.height;
    glViewport(GLint(std::lround((viewport.origin.x-layer.bounds.origin.x)*sx)),GLint(std::lround((CGRectGetMaxY(layer.bounds)-CGRectGetMaxY(viewport))*sy)),GLsizei(std::lround(viewport.size.width*sx)),GLsizei(std::lround(viewport.size.height*sy)));
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    glMatrixMode(GL_TEXTURE);glPushMatrix();glLoadIdentity();
    const GLboolean dither=glIsEnabled(GL_DITHER);glDisable(GL_DITHER);
    glDisable(GL_BLEND);glDisableClientState(GL_COLOR_ARRAY);glBindTexture(GL_TEXTURE_2D,_sceneTexture);
    GLfloat positions[]={-1,-1,-1,1,1,-1,1,-1,-1,1,1,1},uv[]={0,0,0,1,1,0,1,0,0,1,1,1};
    glVertexPointer(2,GL_FLOAT,0,positions);glTexCoordPointer(2,GL_FLOAT,0,uv);glDrawArrays(GL_TRIANGLES,0,6);
#if HM_VALIDATION
    double afterQuad=CACurrentMediaTime();_presentQuad=(afterQuad-afterCopy)*1000;
#endif
    glBindRenderbufferOES(GL_RENDERBUFFER_OES,_displayBuffer);BOOL ok=[_context presentRenderbuffer:GL_RENDERBUFFER_OES];
#if HM_VALIDATION
    double afterSwap=CACurrentMediaTime();_presentSwap=(afterSwap-afterQuad)*1000;
#endif
    glMatrixMode(GL_TEXTURE);glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);glPopMatrix();
    if(dither)glEnable(GL_DITHER);
    glEnable(GL_BLEND);glEnableClientState(GL_COLOR_ARRAY);glBindFramebufferOES(GL_FRAMEBUFFER_OES,_sceneFBO);glViewport(0,0,_bufferWidth,_bufferHeight);[self applyClip];
    if(delay>0)[NSThread sleepForTimeInterval:delay];
#if HM_VALIDATION
    _presentWait=(CACurrentMediaTime()-afterSwap)*1000;
#endif
    if(!ok)[self recordError:@"Drawable presentation failed"];return ok;
}
- (void)finish {
    EAGLContext *previous=[EAGLContext currentContext];
    if(![EAGLContext setCurrentContext:_context]){[self recordError:@"Cannot activate ES context for finish"];return;}
    glFinish();
    [EAGLContext setCurrentContext:previous];
}
- (UIImage *)snapshot {
    EAGLContext *previous=[EAGLContext currentContext];if(![EAGLContext setCurrentContext:_context])return nil;
    GLint previousFBO=0;glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES,&previousFBO);glBindFramebufferOES(GL_FRAMEBUFFER_OES,_sceneFBO);
    std::vector<uint8_t> bottom(size_t(_bufferWidth)*_bufferHeight*4),top(bottom.size());glReadPixels(0,0,_bufferWidth,_bufferHeight,GL_RGBA,GL_UNSIGNED_BYTE,bottom.data());
    GLenum error=glGetError();glBindFramebufferOES(GL_FRAMEBUFFER_OES,previousFBO);[EAGLContext setCurrentContext:previous];
    if(error!=GL_NO_ERROR){[self recordError:@"Diagnostic readback failed"];return nil;}
    for(int y=0;y<_bufferHeight;++y)std::copy_n(bottom.data()+size_t(_bufferHeight-1-y)*_bufferWidth*4,size_t(_bufferWidth)*4,top.data()+size_t(y)*_bufferWidth*4);
    // The drawable is opaque. Readback alpha is post-blend framebuffer state,
    // not coverage: applying it again in UIImage would darken displayed RGB.
    for(size_t i=3;i<top.size();i+=4)top[i]=255;
    NSData *data=[NSData dataWithBytes:top.data() length:top.size()];CGDataProviderRef provider=CGDataProviderCreateWithCFData((__bridge CFDataRef)data);CGColorSpaceRef space=CGColorSpaceCreateDeviceRGB();
    CGImageRef image=CGImageCreate(_bufferWidth,_bufferHeight,8,32,size_t(_bufferWidth)*4,space,CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(kCGImageAlphaLast),provider,nullptr,false,kCGRenderingIntentDefault);
    UIImage *result=image?[UIImage imageWithCGImage:image]:nil;if(image)CGImageRelease(image);CGColorSpaceRelease(space);CGDataProviderRelease(provider);return result;
}
- (NSDictionary *)diagnostics {NSMutableDictionary* result=[@{@"backend":@"OpenGL ES 1.1",@"logicalWidth":@(_logicalWidth),@"logicalHeight":@(_logicalHeight),@"bufferWidth":@(_bufferWidth),@"bufferHeight":@(_bufferHeight),@"sceneReallocations":@(_sceneReallocations),@"drawableInternalFormat":@(_displayFormat),@"vendor":_vendor?:@"",@"renderer":_renderer?:@"",@"version":_version?:@"",@"textureCount":@(_textures.size()),@"textureUploads":@(_uploads),@"drawCalls":@(_drawCalls),@"frames":@(_frames),@"lastError":_lastError?:@""} mutableCopy];
#if HM_VALIDATION
    result[@"presentation_timings_ms"]=@{@"copy":@(_presentCopy),@"quad":@(_presentQuad),@"swap":@(_presentSwap),@"wait_and_restore":@(_presentWait)};
#endif
    return result;}
@end
