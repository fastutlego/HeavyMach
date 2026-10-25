#import <XCTest/XCTest.h>
#import <objc/runtime.h>
#import "../../Sources/iOS/HMGameView.h"
#define HM_VALIDATION 1
#include "../../Sources/Core/GameSession.hpp"
#include <stdexcept>
@interface HMGameView (SaveParity)
- (void)persistSave;
- (void)startSlot:(int)slot mode:(int)mode mission:(int)mission stage:(int)stage;
- (void)frame:(id)sender;
- (void)gameButton:(int)identifier state:(int)state;
@end
namespace {
template<class T>T& field(id o,const char* name){Ivar i=class_getInstanceVariable(HMGameView.class,name);if(!i)throw std::runtime_error(name);return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>((__bridge void*)o)+ivar_getOffset(i));}
}
@interface HMRealSaveView:HMGameView
@property(nonatomic)NSUInteger writes;
@end
@implementation HMRealSaveView
- (void)persistSave {++self.writes;[super persistSave];}
@end
@interface HMSaveParityTests:XCTestCase
@end
@implementation HMSaveParityTests
- (void)testFiringDoesNotWriteUntilOriginalMenuSavePoint {
 HMRealSaveView* view=[[HMRealSaveView alloc]initWithFrame:CGRectMake(0,0,480,320)];
 [[view valueForKey:@"frameTimer"]invalidate];[[view valueForKey:@"motionTimer"]invalidate];
 NSURL* directory=[NSURL fileURLWithPath:[NSTemporaryDirectory() stringByAppendingPathComponent:NSUUID.UUID.UUIDString] isDirectory:YES];
 XCTAssertTrue([NSFileManager.defaultManager createDirectoryAtURL:directory withIntermediateDirectories:YES attributes:nil error:nil]);
 NSURL* file=[directory URLByAppendingPathComponent:@"HvM.dat"];[view setValue:file forKey:@"saveURL"];
 auto& save=field<heavymach::SaveData>(view,"_save");save={};heavymach::GlobalInfo info(save.slots[0]);info.selectSlot(1);info.addWeaponItem("P_Wpn_Gun02",999);
 [view startSlot:0 mode:0 mission:0 stage:0];
 auto& session=field<std::unique_ptr<heavymach::GameSession>>(view,"_session");XCTAssertTrue(bool(session));if(!session)return;
 session->selectWeapon(4);const_cast<heavymach::GobHvM_Player*>(session->player())->hp=100000;session->touch({470,160},0);
 [view persistSave];XCTAssertEqual(view.writes,NSUInteger(1));NSData* before=[NSData dataWithContentsOfURL:file];XCTAssertEqual(before.length,NSUInteger(3624));
 NSUInteger maximumBullets=0;
 for(int i=0;i<180;++i){session->touch({470,160},1);[view frame:nil];maximumBullets=std::max(maximumBullets,session->bulletCount());}
 XCTAssertGreaterThan(maximumBullets,NSUInteger(0));XCTAssertEqual(view.writes,NSUInteger(1));
 XCTAssertEqualObjects([NSData dataWithContentsOfURL:file],before);
 [view gameButton:100 state:2];[view gameButton:102 state:2];XCTAssertEqual(view.writes,NSUInteger(2));
 NSData* after=[NSData dataWithContentsOfURL:file];auto expected=save.encode();XCTAssertEqualObjects(after,[NSData dataWithBytes:expected.data() length:expected.size()]);
 for(int i=0;i<90;++i)[view frame:nil];XCTAssertEqual(view.writes,NSUInteger(2));
 [view setApplicationActive:NO];XCTAssertEqual(view.writes,NSUInteger(2));
 NSLog(@"HM_SAVE_PARITY real_file=%@ firing_frames=180 writes_during_firing=0 explicit_menu_writes=1",file.path);
 XCTAssertTrue([NSFileManager.defaultManager removeItemAtURL:directory error:nil]);
}
@end
