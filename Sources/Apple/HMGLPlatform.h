#pragma once
#import "HMPlatform.h"
#if TARGET_OS_OSX
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
typedef NSOpenGLContext HMGLContext;
typedef NSView HMGLSurface;
@interface NSOpenGLContext (HMActivation)
+ (BOOL)setCurrentContext:(NSOpenGLContext*)context;
@end
static inline HMGLContext *HMCreateGLContext(void) {
 NSOpenGLPixelFormatAttribute attributes[]={NSOpenGLPFAOpenGLProfile,NSOpenGLProfileVersionLegacy,NSOpenGLPFAAccelerated,NSOpenGLPFADoubleBuffer,NSOpenGLPFAColorSize,24,NSOpenGLPFAAlphaSize,8,0};
 NSOpenGLPixelFormat *format=[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
 HMGLContext *context=[[NSOpenGLContext alloc] initWithFormat:format shareContext:nil];
 // The shared game timer and presentation limiter already pace frames at 60 Hz.
 // Avoid adding a vertical-refresh wait to the existing frame pacing.
 GLint sync=0;[context setValues:&sync forParameter:NSOpenGLContextParameterSwapInterval];return context;
}
static inline NSString *HMGLBackendName(void){return @"macOS OpenGL compatibility";}
#define glOrthof glOrtho
#define GL_COLOR_ATTACHMENT0_OES GL_COLOR_ATTACHMENT0_EXT
#define GL_FRAMEBUFFER_BINDING_OES GL_FRAMEBUFFER_BINDING_EXT
#define GL_FRAMEBUFFER_COMPLETE_OES GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_OES GL_FRAMEBUFFER_EXT
#define GL_MAX_RENDERBUFFER_SIZE_OES GL_MAX_RENDERBUFFER_SIZE_EXT
#define GL_RENDERBUFFER_BINDING_OES GL_RENDERBUFFER_BINDING_EXT
#define GL_RENDERBUFFER_HEIGHT_OES GL_RENDERBUFFER_HEIGHT_EXT
#define GL_RENDERBUFFER_INTERNAL_FORMAT_OES GL_RENDERBUFFER_INTERNAL_FORMAT_EXT
#define GL_RENDERBUFFER_OES GL_RENDERBUFFER_EXT
#define GL_RENDERBUFFER_WIDTH_OES GL_RENDERBUFFER_WIDTH_EXT
#define GL_RGBA8_OES GL_RGBA8
#define glBindFramebufferOES glBindFramebufferEXT
#define glBindRenderbufferOES glBindRenderbufferEXT
#define glCheckFramebufferStatusOES glCheckFramebufferStatusEXT
#define glDeleteFramebuffersOES glDeleteFramebuffersEXT
#define glDeleteRenderbuffersOES glDeleteRenderbuffersEXT
#define glFramebufferRenderbufferOES glFramebufferRenderbufferEXT
#define glGenFramebuffersOES glGenFramebuffersEXT
#define glGenRenderbuffersOES glGenRenderbuffersEXT
#define glGetRenderbufferParameterivOES glGetRenderbufferParameterivEXT
#define glRenderbufferStorageOES glRenderbufferStorageEXT
#else
#import <QuartzCore/CAEAGLLayer.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
typedef EAGLContext HMGLContext;
typedef CAEAGLLayer HMGLSurface;
static inline HMGLContext *HMCreateGLContext(void){return [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];}
static inline NSString *HMGLBackendName(void){return @"OpenGL ES 1.1";}
#endif
