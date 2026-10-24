#import <Foundation/Foundation.h>

// One decoded buffer and one persistent OpenAL source per original sound.
// Call from the app's main thread; each operation restores the prior context.
@interface HMOpenALAudio : NSObject
- (BOOL)prepareOriginalSounds;
- (void)sound:(NSString *)name pan:(float)sourceX;
- (NSDictionary *)diagnostics;
- (NSDictionary *)soundInfo:(NSString *)name;
@end
