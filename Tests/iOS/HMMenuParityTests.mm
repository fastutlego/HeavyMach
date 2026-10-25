#import <XCTest/XCTest.h>
#import "../../Sources/iOS/HMMenus.h"
#import "../../Sources/iOS/HMGameHUD.h"
#import "../../Sources/iOS/HMMedia.h"

@interface HMMenuCommandMedia : HMMedia
@property(nonatomic,strong) NSMutableArray<NSDictionary *> *commands;
@property(nonatomic,strong) UIImage *fixtureImage;
@property(nonatomic,strong) UIImage *slotButtonImage;
@end
@implementation HMMenuCommandMedia
- (instancetype)init {if((self=[super init]))_commands=[NSMutableArray new];return self;}
- (UIImage *)image:(NSString *)name {return [name isEqualToString:@"SelSlot_Btn.png"]&&self.slotButtonImage?self.slotButtonImage:self.fixtureImage;}
- (void)draw:(NSString *)name x:(CGFloat)x y:(CGFloat)y {
    [self draw:name x:x y:y columns:1 rows:1 tile:0 scaleX:1 scaleY:1 rotation:0 alpha:1 blend:0];
}
- (void)draw:(NSString *)name x:(CGFloat)x y:(CGFloat)y columns:(NSInteger)c rows:(NSInteger)r tile:(NSInteger)t scaleX:(CGFloat)sx scaleY:(CGFloat)sy rotation:(CGFloat)angle alpha:(CGFloat)a {
    [self draw:name x:x y:y columns:c rows:r tile:t scaleX:sx scaleY:sy rotation:angle alpha:a blend:0];
}
- (void)draw:(NSString *)name x:(CGFloat)x y:(CGFloat)y columns:(NSInteger)c rows:(NSInteger)r tile:(NSInteger)t scaleX:(CGFloat)sx scaleY:(CGFloat)sy rotation:(CGFloat)angle alpha:(CGFloat)a blend:(NSInteger)b {
    [_commands addObject:@{@"name":name,@"x":@(x),@"y":@(y),@"alpha":@(a),@"rotation":@(angle),@"tile":@(t)}];
}
@end

@interface HMMenus (ParityDateProbe)
- (NSString *)dateText:(int32_t)value;
@end
@interface HMDateCountingMenus : HMMenus
@property(nonatomic) NSInteger dateFormatCount;
@end
@implementation HMDateCountingMenus
- (NSString *)dateText:(int32_t)value {self.dateFormatCount++;return [super dateText:value];}
@end

@interface HMMenuParityTests : XCTestCase
@end
@implementation HMMenuParityTests
- (void)testInterruptedResultDrawsResumeMenuAboveIllustrationAndAcceptsResume {
    HMMenuCommandMedia* media=[HMMenuCommandMedia new];
    media.fixtureImage=[[[UIGraphicsImageRenderer alloc]initWithSize:CGSizeMake(120,120)]imageWithActions:^(UIGraphicsImageRendererContext* c){}];
    HMGameHUD* hud=[[HMGameHUD alloc]initWithMedia:media];[hud setMenuVisible:YES];
    for(int i=0;i<120;++i)[hud tick:i/60.0];
    heavymach::StagePresentation presentation;presentation.step=4;presentation.alpha=1;presentation.loadingAsset="GameLoading_01.jpg";
    HMGameHUDSnapshot snapshot;snapshot.paused=true;snapshot.menuVisible=true;snapshot.presentation=&presentation;[hud draw:snapshot];
    NSInteger imageIndex=-1,menuIndex=-1;
    for(NSUInteger i=0;i<media.commands.count;++i){
        if([media.commands[i][@"name"]isEqual:@"GameLoading_01.jpg"])imageIndex=i;
        if([media.commands[i][@"name"]isEqual:@"GameMenuSub.png"])menuIndex=i;
    }
    XCTAssertGreaterThan(imageIndex,NSInteger(-1));XCTAssertGreaterThan(menuIndex,imageIndex);
    __block int clicked=-1;hud.button=^(int identifier,int state){if(state==2)clicked=identifier;};
    XCTAssertTrue([hud touch:CGPointMake(220,220) state:0]);XCTAssertTrue([hud touch:CGPointMake(220,220) state:2]);XCTAssertEqual(clicked,101);
}
- (void)testMissionScrollRetainsOriginalFloatCoordinates {
    HMMenuCommandMedia *media=[HMMenuCommandMedia new];
    heavymach::SaveData save;heavymach::GameData data;
    HMMenus *menus=[[HMMenus alloc] initWithMedia:media save:&save data:&data];
    [menus showMissions:0 stage:0];
    // 0x192ec starts row 0 at y250. 0x19ea4 targets y154 for a new save.
    // 0x19ba0 performs separate binary32 subtraction, division, and addition.
    float expected=250;
    for(int i=0;i<60;++i){expected=expected+(154.f-expected)/5.f;[menus tick:double(i)/60];}
    [menus draw];
    NSDictionary *row=nil;
    for(NSDictionary *command in media.commands)if([command[@"name"] isEqual:@"SelMission_Slot.png"]){row=command;break;}
    XCTAssertNotNil(row);XCTAssertEqual([row[@"y"] doubleValue],double(expected));
}
- (void)testMissionIntroUsesOriginalFloatStartTime {
    HMMenuCommandMedia *media=[HMMenuCommandMedia new];
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:media];
    // 0x169b0 truncates g_time to float; 0x161d0 converts elapsed to float.
    [hud setGameStart:8192.000123];
    HMGameHUDSnapshot snapshot;snapshot.time=8192.25;snapshot.mode=0;
    [hud draw:snapshot];
    NSDictionary *intro=nil;
    for(NSDictionary *command in media.commands)if([command[@"name"] isEqual:@"Text_Mission.png"]){intro=command;break;}
    XCTAssertNotNil(intro);XCTAssertEqual([intro[@"alpha"] doubleValue],.125);
}
- (void)testPausedHUDConsumesOnlyButtonHitsAndReleasesOutsideToWorld {
    HMMenuCommandMedia *media=[HMMenuCommandMedia new];
    UIGraphicsImageRenderer *renderer=[[UIGraphicsImageRenderer alloc] initWithSize:CGSizeMake(32,32)];
    media.fixtureImage=[renderer imageWithActions:^(UIGraphicsImageRendererContext *context){}];
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:media];
    HMGameHUDSnapshot snapshot;snapshot.paused=true;snapshot.time=1;
    [hud draw:snapshot];
    // GuiGame has no modal hit surface. Pause filtering belongs to its handler.
    XCTAssertFalse([hud touch:CGPointMake(200,150) state:0]);
    XCTAssertFalse([hud touch:CGPointMake(26,34) state:0]);
    XCTAssertTrue([hud touch:CGPointMake(42,34) state:0]);
    XCTAssertTrue([hud touch:CGPointMake(34,34) state:0]);
    XCTAssertFalse([hud touch:CGPointMake(200,150) state:1]);
    XCTAssertFalse([hud touch:CGPointMake(200,150) state:2]);
}

- (void)testSlotConstructionAndModelTicksConsumeSharedRandomInOriginalOrder {
    HMMenuCommandMedia *media=[HMMenuCommandMedia new];
    heavymach::SaveData save;heavymach::GameData data;
    for(int i=0;i<2;++i){save.slots[i].bytes[8]=1;save.slots[i].setInt(4,i+1);}
    data.parts["P_Body_01"]={.name="P_Body_01",.partsType=0,.tileCnt=1};
    data.parts["P_Top_01"]={.name="P_Top_01",.partsType=1,.tileCnt=1};
    data.parts["P_Wpn_Gun01"]={.name="P_Wpn_Gun01",.partsType=2,.tileCnt=1};
    HMDateCountingMenus *menus=[[HMDateCountingMenus alloc] initWithMedia:media save:&save data:&data];
    __block int randomCalls=0;
    NSMutableArray<NSString *> *events=[NSMutableArray new];
    menus.previewPlayerCreated=^{[events addObject:@"player"];};
    menus.random31=^int32_t {++randomCalls;[events addObject:@"weapon RNG"];return 1;};
    [menus showSlots];
    XCTAssertEqual(randomCalls,2);XCTAssertEqual(menus.dateFormatCount,4);
    XCTAssertEqualObjects(events,(@[@"player",@"weapon RNG",@"player",@"weapon RNG"]));
    // Draws neither reconstruct weapons nor format dates; they retain the
    // model rotation from three setPartsToMyMach0x1be84 calls.
    for(int frame=0;frame<20;++frame)[menus draw];
    XCTAssertEqual(randomCalls,2);XCTAssertEqual(menus.dateFormatCount,4);
    BOOL sawBody=NO;
    for(NSDictionary *command in media.commands)if([command[@"name"] isEqual:@"P_Body_01.png"]){
        sawBody=YES;XCTAssertEqual([command[@"rotation"] doubleValue],double(-.03f));
    }
    XCTAssertTrue(sawBody);
    [menus tick:1];XCTAssertEqual(randomCalls,3);
    [menus setPreviewWorldTime:1 paused:YES];
    [menus tick:2];XCTAssertEqual(randomCalls,3);
    [menus showMode];[menus tick:3];XCTAssertEqual(randomCalls,3);
}

- (void)testHiddenHUDWeaponNotificationPreservesOriginalGlobalPointers {
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:nil];
    heavymach::SaveSlot globalSlot,otherItemSlot;
    globalSlot.setWeapon(0,"P_Wpn_Gun01",12);
    otherItemSlot.setWeapon(0,"P_Wpn_Gun01",7);
    // Cold launch: slotData nil suppresses even an existing item pointer.
    XCTAssertNil([hud changeWeaponForSlot:nullptr selectedWeapon:0 selectedItemSlot:&otherItemSlot selectedItemIndex:0 time:2]);
    XCTAssertEqual(otherItemSlot.weaponCharge(0),7);
    XCTAssertEqual([[hud valueForKey:@"weaponTime"] floatValue],-1.f);
    XCTAssertNil([hud changeWeaponForSlot:&globalSlot selectedWeapon:0 selectedItemSlot:nullptr selectedItemIndex:0 time:3]);
    XCTAssertEqual([[hud valueForKey:@"weaponTime"] floatValue],3.f);
    XCTAssertEqualObjects([hud changeWeaponForSlot:&globalSlot selectedWeapon:1 selectedItemSlot:&otherItemSlot selectedItemIndex:0 time:4],@7);
    XCTAssertEqualObjects([hud changeWeaponForSlot:&globalSlot selectedWeapon:0 selectedItemSlot:&otherItemSlot selectedItemIndex:0 time:5],@99);
    XCTAssertEqual(otherItemSlot.weaponCharge(0),99);
    XCTAssertEqual(globalSlot.weaponCharge(0),12);
}

- (void)testRebuiltSlotButtonKeepsOriginalReceiverPriority {
    HMMenuCommandMedia *media=[HMMenuCommandMedia new];
    media.fixtureImage=[[[UIGraphicsImageRenderer alloc] initWithSize:CGSizeMake(32,32)] imageWithActions:^(UIGraphicsImageRendererContext *c){}];
    media.slotButtonImage=[[[UIGraphicsImageRenderer alloc] initWithSize:CGSizeMake(150,176)] imageWithActions:^(UIGraphicsImageRendererContext *c){}];
    heavymach::SaveData save;heavymach::GameData data;
    save.slots[0].bytes[8]=save.slots[1].bytes[8]=1;
    save.slots[0].setInt(4,1);save.slots[1].setInt(4,2);
    HMMenus *menus=[[HMMenus alloc] initWithMedia:media save:&save data:&data];
    [menus showSlots];[menus draw];
    [menus touch:CGPointMake(332,20) state:2];[menus draw];
    [menus touch:CGPointMake(236,108) state:2];
    XCTAssertFalse(save.slots[1].occupied());
    [menus draw];[menus touch:CGPointMake(85,142) state:0];
    [menus touch:CGPointMake(332,20) state:2];[menus draw];
    // The rebuilt slot 1 receiver now precedes YES in reverse registration
    // order. Its visible area consumes the overlap without deleting slot 0.
    [menus touch:CGPointMake(236,108) state:2];
    XCTAssertTrue(save.slots[0].occupied());
    XCTAssertTrue([[menus valueForKey:@"deleteConfirmation"] boolValue]);
}
- (void)testHPNeedleKeepsInitialRotationUntilHPChanges {
    HMMenuCommandMedia *media=[HMMenuCommandMedia new];
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:media];
    HMGameHUDSnapshot snapshot;snapshot.time=1;
    [hud tick:1];[hud draw:snapshot];
    NSDictionary *needle=nil;
    for(NSDictionary *command in media.commands)if([command[@"name"] isEqual:@"GameUI_Gauge.png"]&&[command[@"x"] intValue]==239){needle=command;break;}
    XCTAssertNotNil(needle);XCTAssertEqual([needle[@"rotation"] doubleValue],0);
    [media.commands removeAllObjects];[hud setHPGauge:1];[hud tick:2];[hud draw:snapshot];
    needle=nil;
    for(NSDictionary *command in media.commands)if([command[@"name"] isEqual:@"GameUI_Gauge.png"]&&[command[@"x"] intValue]==239){needle=command;break;}
    XCTAssertNotNil(needle);
    XCTAssertEqual([needle[@"rotation"] doubleValue],double(float(.125*-5.183627803521752-.6000000238418579)));
}

- (void)testReplacingUpgradePanelResetsOldButtonPress {
    HMMenuCommandMedia *media=[HMMenuCommandMedia new];
    media.fixtureImage=[[[UIGraphicsImageRenderer alloc] initWithSize:CGSizeMake(32,32)] imageWithActions:^(UIGraphicsImageRendererContext *c){}];
    HMGameHUD *hud=[[HMGameHUD alloc] initWithMedia:media];
    heavymach::SaveSlot slot;HMGameHUDSnapshot snapshot;snapshot.slot=&slot;snapshot.time=1;snapshot.paused=true;
    [hud openUpgrade:slot];[hud setValue:@240 forKey:@"upgradeX"];[hud draw:snapshot];
    XCTAssertTrue([hud touch:CGPointMake(294,95) state:0]);
    [hud openUpgrade:slot];[hud setValue:@240 forKey:@"upgradeX"];
    [media.commands removeAllObjects];[hud draw:snapshot];
    NSDictionary *button=nil;
    for(NSDictionary *command in media.commands)if([command[@"name"] isEqual:@"Common_Btn_OK.png"]){button=command;break;}
    XCTAssertNotNil(button);XCTAssertEqual([button[@"tile"] intValue],0);
}

@end
