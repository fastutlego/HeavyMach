#import "HMValidationPanel.h"
#if HM_VALIDATION && TARGET_OS_OSX
#include "GlobalInfo.hpp"
#include <algorithm>
@interface HMValidationPanel () <NSWindowDelegate>
@end
@implementation HMValidationPanel {
 const heavymach::GameData *_data;
 NSTextField *_mission,*_stage,*_level,*_grades[3],*_boss;
 NSButton *_all,*_near,*_canvas;
 NSPopUpButton *_weapon;
 BOOL _didClose;
}
- (NSTextField*)field:(NSString*)label value:(NSString*)value y:(CGFloat)y {
 NSTextField* text=[NSTextField labelWithString:label];text.frame=NSMakeRect(24,y,240,24);[self.window.contentView addSubview:text];
 NSTextField* field=[[NSTextField alloc]initWithFrame:NSMakeRect(280,y,180,24)];field.stringValue=value;[self.window.contentView addSubview:field];return field;
}
- (NSButton*)check:(NSString*)label y:(CGFloat)y {NSButton* b=[NSButton checkboxWithTitle:label target:nil action:nil];b.frame=NSMakeRect(24,y,440,24);[self.window.contentView addSubview:b];return b;}
- (void)button:(NSString*)title action:(SEL)action x:(CGFloat)x y:(CGFloat)y {NSButton* b=[NSButton buttonWithTitle:title target:self action:action];b.frame=NSMakeRect(x,y,220,32);[self.window.contentView addSubview:b];}
- (instancetype)initWithData:(const heavymach::GameData*)data {
 NSWindow* window=[[NSWindow alloc]initWithContentRect:NSMakeRect(0,0,500,560) styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable backing:NSBackingStoreBuffered defer:NO];
 if((self=[super initWithWindow:window])){
  _data=data;window.title=@"HeavyMach 검증 · 별도 저장";window.delegate=self;[window center];
  _mission=[self field:@"미션 (1–8)" value:@"1" y:500];_stage=[self field:@"스테이지 (1부터)" value:@"1" y:464];_level=[self field:@"레벨 (1–100)" value:@"1" y:428];
  for(int i=0;i<3;++i)_grades[i]=[self field:@[@"몸체 등급 (0–6)",@"상부 등급 (0–6)",@"무기 등급 (0–6)"][i] value:@"0" y:392-i*36];
  _boss=[self field:@"보스 번호 (0: 일반 시작)" value:@"0" y:284];
  _all=[self check:@"모든 원본 플레이어 무기 지급" y:248];_all.state=NSControlStateValueOn;
  _near=[self check:@"다음 레벨까지 EXP 1 남기기" y:216];
  _canvas=[self check:@"화면 전체 사용" y:184];_canvas.target=self;_canvas.action=@selector(canvasChangedAction:);
  _weapon=[[NSPopUpButton alloc]initWithFrame:NSMakeRect(24,144,440,28) pullsDown:NO];for(const auto& name:heavymach::validationWeapons(*data))[_weapon addItemWithTitle:[NSString stringWithUTF8String:name.c_str()]];[window.contentView addSubview:_weapon];
  [self button:@"초기 조건 재설정 후 시작" action:@selector(start:) x:24 y:96];[self button:@"저장 이어하기" action:@selector(resume:) x:254 y:96];
  [self button:@"현재 게임에 EXP 1 획득" action:@selector(experience:) x:24 y:52];[self button:@"현재 화면으로 돌아가기" action:@selector(close) x:254 y:52];
 }
 return self;
}
- (void)setWideCanvas:(BOOL)enabled {_wideCanvas=enabled;_canvas.state=enabled?NSControlStateValueOn:NSControlStateValueOff;}
- (void)canvasChangedAction:(id)sender {self.wideCanvas=_canvas.state==NSControlStateValueOn;if(self.canvasChanged)self.canvasChanged(self.wideCanvas);}
- (void)notifyClosed {if(_didClose)return;_didClose=YES;if(self.closed)self.closed();}
- (void)windowWillClose:(NSNotification*)notification {[self notifyClosed];}
- (void)close {[self notifyClosed];[super close];}
- (void)start:(id)sender {
 heavymach::ValidationScenario s;s.mission=int(_mission.integerValue)-1;s.stage=int(_stage.integerValue)-1;s.internalLevel=int(_level.integerValue)-1;
 for(int i=0;i<3;++i)s.grades[i]=uint8_t(std::clamp<NSInteger>(_grades[i].integerValue,0,6));
 s.allWeapons=_all.state==NSControlStateValueOn;s.nearLevelUp=_near.state==NSControlStateValueOn;if(_boss.integerValue>0)s.bossIndex=size_t(_boss.integerValue-1);
 if(_weapon.titleOfSelectedItem)s.weapons.emplace_back(_weapon.titleOfSelectedItem.UTF8String);
 try {
  heavymach::SaveSlot probe;heavymach::prepareValidationSlot(probe,*_data,s);
  if(!s.weapons.empty())for(int i=0;i<probe.weaponCount();++i)if(probe.weaponName(i)==s.weapons.front()){s.selectedWeapon=i;break;}
 }catch(const std::exception& error){NSAlert* a=[NSAlert new];a.messageText=@"시작 조건 확인";a.informativeText=[NSString stringWithUTF8String:error.what()];[a runModal];return;}
 auto action=self.applyFixture;[self close];if(action)action(s);
}
- (void)resume:(id)sender {auto action=self.resumeSave;[self close];if(action)action();}
- (void)experience:(id)sender {auto action=self.addExperience;[self close];if(action)action();}
@end
#endif
