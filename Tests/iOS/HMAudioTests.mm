#import "HMResources.h"
#import <XCTest/XCTest.h>
#include "../../vendor/openal-soft/include/AL/al.h"
#import "../../Sources/iOS/HMMedia.h"
#import "../../Sources/iOS/HMOpenALAudio.h"
#include <cfloat>

// Independent oracle from fd44, WeaponInfo.mch, GameMain2ecc and GuiTitleefbc.
static NSArray<NSString *> *expectedSourceOrder() {
    return @[@"UI_Click",@"UI_Btn_OK",@"UI_Btn_Cancel",@"Game_GetItem",@"Game_ChangeWeapon",@"Game_WeaponEnd",@"Game_LevelUp",@"Game_LowHP",@"SFX_HitBody01",@"SFX_HitBody02",@"SFX_HitBody03",@"SFX_HitBody04",@"SFX_GroundHit",@"SFX_ExplGround",@"SFX_ExplMach",
        @"Wpn_Gun01",@"Wpn_Gun02",@"Wpn_Gun03",@"Wpn_Shotgun",@"Wpn_Canon",@"Wpn_Missile",@"Wpn_Laser",@"Wpn_Default",@"Wpn_EnGun",@"Wpn_EnCanon",@"Wpn_EnMissile",
        @"Loading_Open",@"Loading_Close",@"Title_TitleIn",@"Title_Bang",@"Title_TapToStart"];
}
@interface HMAudioTests : XCTestCase
@end
@implementation HMAudioTests
- (void)testOriginalResourcesExistAndDecodeWithoutPlayback {
    HMMedia *media=[HMMedia new];
    for(NSString *name in expectedSourceOrder()) {
        XCTAssertNotNil([HMGameResourceBundle() URLForResource:name withExtension:@"wav" subdirectory:@"Audio"]);
        NSDictionary *voice=[media.audio soundInfo:name];
        XCTAssertGreaterThan([voice[@"bufferID"] unsignedIntValue],0u,@"%@ must decode into a real AL buffer",name);
        XCTAssertEqual([voice[@"channels"] intValue],1);XCTAssertEqual([voice[@"bits"] intValue],16);
        XCTAssertGreaterThan([voice[@"bytes"] intValue],0);
        XCTAssertTrue(([@[@22050,@44100,@48000] containsObject:voice[@"rate"]]));
        XCTAssertEqual([voice[@"state"] intValue],AL_INITIAL);
    }
}
- (void)testInitEagerlyCreatesAndPreparesOriginalSourcesInOrder {
    HMMedia *media=[HMMedia new];NSDictionary *diagnostics=[media.audio diagnostics];
    XCTAssertEqualObjects(diagnostics[@"renderer"],@"OpenAL Soft");
    #if TARGET_OS_OSX
    // CoreAudio exposes the selected device name on macOS (e.g. built-in speakers).
    XCTAssertGreaterThan([diagnostics[@"device"] length],NSUInteger(0));
#else
    XCTAssertEqualObjects(diagnostics[@"device"],@"CoreAudio Default");
#endif
    XCTAssertEqual([diagnostics[@"bufferCount"] unsignedIntegerValue],NSUInteger(31));
    XCTAssertEqual([diagnostics[@"sourceCount"] unsignedIntegerValue],NSUInteger(31));
    XCTAssertEqualObjects(diagnostics[@"creationOrder"],expectedSourceOrder());
    XCTAssertEqual([diagnostics[@"startCount"] unsignedIntegerValue],NSUInteger(0));
    NSUInteger index=0;
    for(NSString *name in expectedSourceOrder()) {
        NSDictionary *voice=[media.audio soundInfo:name];
        XCTAssertEqualObjects(voice[@"bufferID"],voice[@"attachedBufferID"]);
        XCTAssertEqual([voice[@"maxDistance"] floatValue],index++<26?800.f:FLT_MAX);
        XCTAssertEqual([voice[@"gain"] floatValue],1.f);
        XCTAssertEqual([voice[@"referenceDistance"] floatValue],1.f);XCTAssertEqual([voice[@"rolloff"] floatValue],1.f);
    }
    XCTAssertEqualObjects(diagnostics[@"error"],@"");
}
- (void)testRepeatedPreparationAndSoundRequestsReuseOneDecodedVoice {
    HMMedia *media=[HMMedia new];NSMutableDictionary *initial=[NSMutableDictionary new];
    for(NSString *name in expectedSourceOrder())initial[name]=[media.audio soundInfo:name];
    XCTAssertTrue([media prepareOriginalSounds]);
    for(NSString *name in expectedSourceOrder()) {
        NSDictionary *now=[media.audio soundInfo:name];
        XCTAssertEqualObjects(initial[name][@"sourceID"],now[@"sourceID"]);
        XCTAssertEqualObjects(initial[name][@"bufferID"],now[@"bufferID"]);
    }
    [media sound:@"Wpn_Gun01" pan:2];NSDictionary *first=[media.audio soundInfo:@"Wpn_Gun01"];
    XCTAssertEqualObjects(first[@"position"],(@[@2,@0,@0]));
    [media sound:@"Wpn_Gun01" pan:-4];NSDictionary *second=[media.audio soundInfo:@"Wpn_Gun01"];
    XCTAssertEqualObjects(first[@"sourceID"],second[@"sourceID"]);XCTAssertEqualObjects(first[@"bufferID"],second[@"attachedBufferID"]);
    XCTAssertEqualObjects(second[@"position"],(@[@-4,@0,@0]));XCTAssertEqual([second[@"gain"] floatValue],1.f);
    XCTAssertEqual([second[@"startCount"] unsignedIntegerValue],NSUInteger(2));
    XCTAssertEqual([[media.audio diagnostics][@"bufferCount"] unsignedIntegerValue],NSUInteger(31));
    XCTAssertEqualObjects([media.audio diagnostics][@"error"],@"");
}
@end
