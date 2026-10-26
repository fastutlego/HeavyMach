#import "HMGLPlatform.h"
#if TARGET_OS_OSX
@implementation NSOpenGLContext (HMActivation)
+ (BOOL)setCurrentContext:(NSOpenGLContext*)context {if(context)[context makeCurrentContext];else [self clearCurrentContext];return [self currentContext]==context;}
@end
#endif
