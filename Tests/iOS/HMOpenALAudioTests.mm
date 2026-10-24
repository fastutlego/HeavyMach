#import <XCTest/XCTest.h>
#include "../../vendor/openal-soft/include/AL/al.h"
#include "../../vendor/openal-soft/include/AL/alc.h"
#import <AVFoundation/AVFoundation.h>
#import "../../Sources/iOS/HMOpenALAudio.h"
#include <cstring>
#include <limits>

@interface HMOpenALAudio (RIFFTest)
- (BOOL)loadSoundData:(NSData*)data name:(NSString*)name;
+ (ALCcontext*)validationCurrentContext;
+ (BOOL)validationMakeContextCurrent:(ALCcontext*)context;
+ (void)validationDestroyContext:(ALCcontext*)context;
- (ALCcontext*)validationCreateExternalContext;
- (BOOL)validationPauseSound:(NSString*)name sampleOffset:(NSInteger)offset;
@end
@interface HMOpenALAudioTests : XCTestCase
@end
@implementation HMOpenALAudioTests
- (void)testRestartResetsSampleOffsetWithoutReplacingSourceOrBuffer {
    HMOpenALAudio *audio=[HMOpenALAudio new];XCTAssertTrue([audio prepareOriginalSounds]);
    [audio sound:@"Title_TitleIn" pan:1000];
    NSDictionary *initial=[audio soundInfo:@"Title_TitleIn"];
    ALCcontext *previous=[HMOpenALAudio validationCurrentContext];
    ALint offset=[initial[@"bytes"] intValue]/4; // halfway into mono16 buffer
    XCTAssertGreaterThan(offset,100);
    XCTAssertTrue([audio validationPauseSound:@"Title_TitleIn" sampleOffset:offset]);
    XCTAssertEqual([HMOpenALAudio validationCurrentContext],previous);
    NSDictionary *advanced=[audio soundInfo:@"Title_TitleIn"];
    XCTAssertEqual([advanced[@"sampleOffset"] intValue],offset);
    [audio sound:@"Title_TitleIn" pan:-1000];
    NSDictionary *restarted=[audio soundInfo:@"Title_TitleIn"];
    XCTAssertEqualObjects(restarted[@"sourceID"],initial[@"sourceID"]);
    XCTAssertEqualObjects(restarted[@"attachedBufferID"],initial[@"bufferID"]);
    XCTAssertEqual([restarted[@"state"] intValue],AL_PLAYING);
    XCTAssertLessThan([restarted[@"sampleOffset"] intValue],offset/2);
    XCTAssertEqualObjects(restarted[@"position"],(@[@-1000,@0,@0]));
    XCTAssertEqual([restarted[@"startCount"] intValue],2);
    XCTAssertEqualObjects([audio diagnostics][@"error"],@"");
}
- (void)testOperationsPreserveExternalContextAndReuseVoices {
    HMOpenALAudio *deviceOwner=[HMOpenALAudio new];
    ALCcontext *previous=[HMOpenALAudio validationCurrentContext];
    ALCcontext *external=[deviceOwner validationCreateExternalContext];XCTAssertNotEqual(external,nullptr);
    if(!external)return;
    [HMOpenALAudio validationMakeContextCurrent:external];
    @autoreleasepool {
        HMOpenALAudio *audio=[HMOpenALAudio new];XCTAssertEqual([HMOpenALAudio validationCurrentContext],external);
        XCTAssertTrue([audio prepareOriginalSounds]);XCTAssertEqual([HMOpenALAudio validationCurrentContext],external);
        [audio sound:@"Wpn_Gun01" pan:2];XCTAssertEqual([HMOpenALAudio validationCurrentContext],external);
        [audio soundInfo:@"Wpn_Gun01"];XCTAssertEqual([HMOpenALAudio validationCurrentContext],external);
        [audio sound:@"Wpn_Gun01" pan:4];XCTAssertEqual([[audio diagnostics][@"startCount"] intValue],2);
        XCTAssertEqualObjects([audio diagnostics][@"error"],@"");
    }
    XCTAssertEqual([HMOpenALAudio validationCurrentContext],external);
    [HMOpenALAudio validationMakeContextCurrent:previous];[HMOpenALAudio validationDestroyContext:external];
}
- (void)testMalformedRIFFDoesNotAllocateALResources {
    NSURL *url=[NSBundle.mainBundle URLForResource:@"Wpn_Gun01" withExtension:@"wav" subdirectory:@"Audio"];
    NSData *valid=[NSData dataWithContentsOfURL:url];XCTAssertGreaterThan(valid.length,NSUInteger(44));if(valid.length<44)return;
    HMOpenALAudio *audio=[HMOpenALAudio new];
    XCTAssertFalse([audio loadSoundData:[valid subdataWithRange:NSMakeRange(0,11)] name:@"short"]);
    XCTAssertFalse([audio loadSoundData:[valid subdataWithRange:NSMakeRange(0,valid.length-1)] name:@"truncated"]);
    NSMutableData *overflow=[valid mutableCopy];memset((uint8_t*)overflow.mutableBytes+16,255,4);
    XCTAssertFalse([audio loadSoundData:overflow name:@"overflow"]);
    NSMutableData *wrongFormat=[valid mutableCopy];((uint8_t*)wrongFormat.mutableBytes)[20]=3;
    XCTAssertFalse([audio loadSoundData:wrongFormat name:@"float"]);
    XCTAssertEqual([[audio diagnostics][@"bufferCount"] intValue],0);XCTAssertEqual([[audio diagnostics][@"sourceCount"] intValue],0);
    XCTAssertTrue([audio loadSoundData:valid name:@"valid"]);XCTAssertEqual([[audio diagnostics][@"bufferCount"] intValue],1);
}
- (void)testNonfinitePositionDoesNotStartOrMutateSource {
    HMOpenALAudio *audio=[HMOpenALAudio new];XCTAssertTrue([audio prepareOriginalSounds]);
    [audio sound:@"Wpn_Gun01" pan:std::numeric_limits<float>::infinity()];
    XCTAssertEqual([[audio diagnostics][@"startCount"] intValue],0);
    XCTAssertEqualObjects([audio soundInfo:@"Wpn_Gun01"][@"position"],(@[@0,@0,@0]));
}
- (void)testSessionNotificationsDoNotIntroduceSourcePauseOrPlaybackGating {
    // Synthetic notifications exercise app policy only, not a real OS interruption.
    // The compatibility host adds no HeavyMach source pause/resume observer.
    NSString *category=AVAudioSession.sharedInstance.category;
    HMOpenALAudio *audio=[HMOpenALAudio new];XCTAssertTrue([audio prepareOriginalSounds]);
    ALCcontext *previous=[HMOpenALAudio validationCurrentContext];
    [audio sound:@"Title_TitleIn" pan:1000];
    NSDictionary *original=[audio soundInfo:@"Title_TitleIn"];
    XCTAssertEqual([original[@"state"] intValue],AL_PLAYING);
    [NSNotificationCenter.defaultCenter postNotificationName:AVAudioSessionInterruptionNotification object:AVAudioSession.sharedInstance userInfo:@{AVAudioSessionInterruptionTypeKey:@(AVAudioSessionInterruptionTypeBegan)}];
    XCTAssertEqual([[audio soundInfo:@"Title_TitleIn"][@"state"] intValue],AL_PLAYING);
    [audio sound:@"Title_TitleIn" pan:10];
    XCTAssertEqual([[audio diagnostics][@"startCount"] intValue],2);
    [NSNotificationCenter.defaultCenter postNotificationName:AVAudioSessionInterruptionNotification object:AVAudioSession.sharedInstance userInfo:@{AVAudioSessionInterruptionTypeKey:@(AVAudioSessionInterruptionTypeEnded),AVAudioSessionInterruptionOptionKey:@0}];
    [audio sound:@"Title_TitleIn" pan:-1000];
    NSDictionary *restarted=[audio soundInfo:@"Title_TitleIn"];
    XCTAssertEqualObjects(original[@"sourceID"],restarted[@"sourceID"]);
    XCTAssertEqualObjects(original[@"bufferID"],restarted[@"attachedBufferID"]);
    XCTAssertEqual([restarted[@"state"] intValue],AL_PLAYING);
    XCTAssertEqual([[audio diagnostics][@"startCount"] intValue],3);
    XCTAssertEqual([HMOpenALAudio validationCurrentContext],previous);
    XCTAssertEqualObjects(AVAudioSession.sharedInstance.category,category);
    XCTAssertEqualObjects([audio diagnostics][@"error"],@"");
}
@end
