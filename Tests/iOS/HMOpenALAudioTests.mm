#import "HMResources.h"
#import <XCTest/XCTest.h>
#include "../../vendor/openal-soft/include/AL/al.h"
#include "../../vendor/openal-soft/include/AL/alc.h"
#import <TargetConditionals.h>
#if !TARGET_OS_OSX
#import <AVFoundation/AVFoundation.h>
#endif
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
- (void)testInactivityStopsSourcesAndRejectsPlaybackWithoutReallocatingOrAutoResuming {
    HMOpenALAudio *audio=[HMOpenALAudio new];XCTAssertTrue([audio prepareOriginalSounds]);
    NSDictionary *initial=[audio diagnostics];
    NSDictionary *title=[audio soundInfo:@"Title_TitleIn"],*weapon=[audio soundInfo:@"Game_ChangeWeapon"];
    ALCcontext *external=[audio validationCreateExternalContext];XCTAssertNotEqual(external,nullptr);
    if(!external)return;
    ALCcontext *previous=[HMOpenALAudio validationCurrentContext];
    XCTAssertTrue([HMOpenALAudio validationMakeContextCurrent:external]);
    for(NSUInteger cycle=0;cycle<20;++cycle){
        [audio sound:@"Title_TitleIn" pan:0];
        [audio sound:@"Game_ChangeWeapon" pan:0];
        XCTAssertEqual([[audio soundInfo:@"Title_TitleIn"][@"state"] intValue],AL_PLAYING);
        NSUInteger starts=[[audio diagnostics][@"startCount"] unsignedIntegerValue];
        [audio setApplicationActive:NO];
        [audio setApplicationActive:NO]; // resign-active followed by background
        XCTAssertFalse([[audio diagnostics][@"applicationActive"] boolValue]);
        XCTAssertEqual([[audio diagnostics][@"stopAllCount"] unsignedIntegerValue],cycle+1);
        XCTAssertEqual([[audio soundInfo:@"Title_TitleIn"][@"state"] intValue],AL_STOPPED);
        XCTAssertEqual([[audio soundInfo:@"Game_ChangeWeapon"][@"state"] intValue],AL_STOPPED);
        [audio sound:@"Title_TitleIn" pan:50];
        [audio sound:@"Game_ChangeWeapon" pan:50];
        [audio sound:@"inactive-unloaded-sound" pan:0]; // must not read or allocate
        XCTAssertEqual([[audio diagnostics][@"startCount"] unsignedIntegerValue],starts);
        XCTAssertEqualObjects([audio soundInfo:@"Title_TitleIn"][@"position"],(@[@0,@0,@0]));
        [audio setApplicationActive:YES];
        [audio setApplicationActive:YES];
        XCTAssertTrue([[audio diagnostics][@"applicationActive"] boolValue]);
        XCTAssertEqual([[audio soundInfo:@"Title_TitleIn"][@"state"] intValue],AL_STOPPED);
        XCTAssertEqual([[audio soundInfo:@"Game_ChangeWeapon"][@"state"] intValue],AL_STOPPED);
        XCTAssertEqual([[audio diagnostics][@"startCount"] unsignedIntegerValue],starts);
        XCTAssertEqual([HMOpenALAudio validationCurrentContext],external);
    }
    [audio sound:@"Title_TitleIn" pan:0];
    [audio sound:@"Game_ChangeWeapon" pan:0];
    XCTAssertEqual([[audio soundInfo:@"Title_TitleIn"][@"state"] intValue],AL_PLAYING);
    XCTAssertEqual([[audio diagnostics][@"startCount"] unsignedIntegerValue],NSUInteger(42));
    XCTAssertEqualObjects([audio soundInfo:@"Title_TitleIn"][@"sourceID"],title[@"sourceID"]);
    XCTAssertEqualObjects([audio soundInfo:@"Title_TitleIn"][@"attachedBufferID"],title[@"bufferID"]);
    XCTAssertEqualObjects([audio soundInfo:@"Game_ChangeWeapon"][@"sourceID"],weapon[@"sourceID"]);
    XCTAssertEqualObjects([audio soundInfo:@"Game_ChangeWeapon"][@"attachedBufferID"],weapon[@"bufferID"]);
    XCTAssertEqualObjects([audio diagnostics][@"bufferCount"],initial[@"bufferCount"]);
    XCTAssertEqualObjects([audio diagnostics][@"sourceCount"],initial[@"sourceCount"]);
    XCTAssertEqualObjects([audio diagnostics][@"error"],@"");
    [HMOpenALAudio validationMakeContextCurrent:previous];[HMOpenALAudio validationDestroyContext:external];
}
- (void)testInitiallyInactiveAudioDoesNotLoadRequestedSound {
    HMOpenALAudio *audio=[HMOpenALAudio new];
    [audio setApplicationActive:NO];
    [audio sound:@"Game_ChangeWeapon" pan:0];
    XCTAssertEqual([[audio diagnostics][@"sourceCount"] intValue],0);
    XCTAssertEqual([[audio diagnostics][@"startCount"] intValue],0);
    [audio setApplicationActive:YES];
    XCTAssertEqual([[audio diagnostics][@"sourceCount"] intValue],0);
    [audio sound:@"Game_ChangeWeapon" pan:0];
    XCTAssertEqual([[audio diagnostics][@"sourceCount"] intValue],1);
    XCTAssertEqual([[audio diagnostics][@"startCount"] intValue],1);
    XCTAssertEqualObjects([audio diagnostics][@"error"],@"");
}
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
    NSURL *url=[HMGameResourceBundle() URLForResource:@"Wpn_Gun01" withExtension:@"wav" subdirectory:@"Audio"];
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
#if !TARGET_OS_OSX
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
#endif
@end
