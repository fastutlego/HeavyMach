#import "HMGameHUD.h"
#import "HMMedia.h"
#include "HMButtonState.hpp"
#include <map>
#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <vector>

namespace {
// QobButton 0x87f8 rejects <= minimum and > maximum, in binary32.
// CGRectContainsPoint instead includes minimum and excludes maximum.
bool originalButtonContains(CGRect bounds,CGPoint point) {
    const float x=float(point.x),y=float(point.y);
    return x>float(CGRectGetMinX(bounds))&&x<=float(CGRectGetMaxX(bounds))&&
           y>float(CGRectGetMinY(bounds))&&y<=float(CGRectGetMaxY(bounds));
}
struct HUDButton {int identifier,key;CGRect bounds;};
int glyph(unichar c) {return c>='0'&&c<='9'?c-'0':c=='.'?10:c=='/'?11:c==':'?12:c=='-'?13:-1;}
}
@implementation HMGameHUD {
    HMMedia *_media;
    heavymach::hud::Configuration _layout;
    int _layoutGroup;
    BOOL _editingLayout;
    int _keyboardFocus;
    BOOL _keyboardFocusVisible;
    std::vector<HUDButton> _buttons;
    std::map<int,heavymach::HMButtonState> _buttonStates;
    int _pressed,_menuStep,_upgradeStep;
    bool _modal,_upgradeVisible;
    double _time;
    // 0x169b0, 0x1767c, 0x178f0 store g_time after truncdfsf.
    float _startTime,_reloadTime,_weaponTime;
    float _hp,_hpDest,_hpRotation,_gear,_gearVelocity;
    std::array<float,3> _menuX;
    float _upgradeX;
    std::array<uint8_t,3> _upgradeGrades;
}
- (instancetype)initWithMedia:(HMMedia *)media {
    if ((self=[super init])) {_media=media;_layoutGroup=-1;_pressed=-1;_startTime=_reloadTime=_weaponTime=-1;_menuX.fill(-240);_hpDest=0;_upgradeX=-240;}
    return self;
}
- (void)setLayout:(const heavymach::hud::Configuration&)layout editing:(BOOL)editing {_layout=layout;_editingLayout=editing;}
- (void)useLayoutGroup:(int)group {
    _layoutGroup=group;
    if(!_layout.customized())return;
    auto t=group<0?_layout.legacy:_layout.group(group).transform;
    [_media setDrawingOffset:CGPointMake(t.offset.x,t.offset.y)];[_media setDrawingScale:t.scale];
    if(group<0){auto r=_layout.legacy.apply(heavymach::hud::Rect{0,0,480,320});[_media setClipRect:CGRectMake(r.x,r.y,r.width,r.height)];}
    else {auto r=_layout.group(group).visualBounds;[_media setClipRect:CGRectMake(r.x,r.y,r.width,r.height)];}
}
- (void)drawLayoutBackground {
    using namespace heavymach::hud;
    if(!_layout.customized()){[self image:@"GameUI_Btm.png" x:240 y:32];[self image:@"GameUI_Top.png" x:240 y:288];return;}
    const int ids[]={jump,weapon,gauges,aim,progress,menu};
    const CGRect sources[]={CGRectMake(0,0,72,64),CGRectMake(72,0,88,64),CGRectMake(160,0,174,64),CGRectMake(334,0,146,64),CGRectMake(0,0,102,27),CGRectMake(0,27,76,37)};
    for(int n=0;n<6;++n){int g=ids[n];[self useLayoutGroup:g];CGRect src=sources[n];BOOL top=n>=4;
        CGRect dest=CGRectMake(src.origin.x,top?320-CGRectGetMaxY(src):64-CGRectGetMaxY(src),src.size.width,src.size.height);
        [_media drawRegion:top?@"GameUI_Top.png":@"GameUI_Btm.png" source:src destination:dest];
        // Existing riveted vertical metal border closes the exposed rail ends.
        if(n<4){for(CGFloat x:{CGRectGetMinX(dest),CGRectGetMaxX(dest)-4})[_media drawRegion:@"GameUI_Top.png" source:CGRectMake(0,0,4,27) destination:CGRectMake(x,0,4,19)];}
        if(g==progress)[_media drawRegion:@"GameUI_Top.png" source:CGRectMake(0,0,4,27) destination:CGRectMake(98,293,4,27)];
    }
    [self useLayoutGroup:-1];
}
- (void)setGameStart:(double)time {_startTime=time;}
- (void)setReloadComplete:(double)time {_reloadTime=time;}
- (void)showWeaponChange:(double)time {_weaponTime=time;}
- (NSNumber *)changeWeaponForSlot:(heavymach::SaveSlot *)slot
                  selectedWeapon:(int)selectedWeapon
                selectedItemSlot:(heavymach::SaveSlot *)itemSlot
               selectedItemIndex:(int)itemIndex time:(double)time {
    // GuiGame.changeWeapon 0x178f0 first guards GlobalInfo.slotData. The item
    // pointer is independent of that pointer and persists through screen changes.
    if(!slot)return nil;
    [self showWeaponChange:time];
    if(!itemSlot||itemIndex<0||size_t(itemIndex)>=heavymach::SaveSlot::weaponCapacity)return nil;
    const size_t chargeOffset=0x4c+size_t(itemIndex)*0x1c;
    if(uint16_t(selectedWeapon)==0)itemSlot->setInt(chargeOffset,99);
    return @(itemSlot->i32(chargeOffset));
}
- (void)setHPGauge:(float)value {_hpDest=value;}
- (void)setReloadGauge:(float)value {
    // 0x1762c: every reload event restarts the gear, including repeated values.
    _gearVelocity=.15f;
}
- (void)openUpgrade:(const heavymach::SaveSlot&)slot {
    // 0x16a98 recreates the panel synchronously, before GuiGame.tick.
    // 0x16a98 removes the prior panel and constructs new QobButton instances.
    for(int identifier:{1200,1201,1202,1300})_buttonStates.erase(identifier);
    _upgradeVisible=true;_upgradeStep=1;_upgradeX=-240;
    _upgradeGrades={slot.bytes[0x2f],slot.bytes[0x2e],slot.bytes[0x2d]};
}
- (void)closeUpgrade {_upgradeVisible=false;_upgradeStep=2;}
- (void)setMenuVisible:(BOOL)visible {
    _menuStep=visible?1:2;if(visible)_menuX.fill(-240);
}
- (void)sprite:(NSString *)name x:(double)x y:(double)y columns:(int)columns rows:(int)rows tile:(int)tile sx:(double)sx sy:(double)sy rotation:(double)rotation alpha:(double)alpha blend:(int)blend {
    // Tile2D clamps oversized indices; this is observable at maximum upgrades.
    [_media draw:name x:x y:y columns:columns rows:rows tile:std::clamp(tile,0,columns*rows-1) scaleX:sx scaleY:sy rotation:rotation alpha:alpha blend:blend];
}
- (void)image:(NSString *)name x:(double)x y:(double)y {
    [self sprite:name x:x y:y columns:1 rows:1 tile:0 sx:1 sy:1 rotation:0 alpha:1 blend:0];
}
- (void)font:(NSString *)name text:(NSString *)text x:(double)x y:(double)y pitch:(double)pitch align:(double)align columns:(int)columns rows:(int)rows alpha:(double)alpha {
    double left=x-pitch*(text.length?text.length-1:0)*align;
    for (NSUInteger i=0;i<text.length;++i) {
        int tile=glyph([text characterAtIndex:i]);
        if(tile>=0)[self sprite:name x:left+i*pitch y:y columns:columns rows:rows tile:tile sx:1 sy:1 rotation:0 alpha:alpha blend:2];
    }
}
- (void)buttonImage:(NSString *)name identifier:(int)identifier x:(double)x y:(double)y columns:(int)columns rows:(int)rows tile:(int)tile enabled:(BOOL)enabled {
    // Result button1000 and booster1000 are distinct original QobButton objects.
    int key=identifier+([name isEqualToString:@"StageClearBtn.png"]?10000:0);
    auto& motion=_buttonStates[key];
    motion.pushDepth=motion.releaseHeight=motion.releaseDelay=0;motion.pushDelay=.1f;
    if(identifier==103){motion.pushDepth=1;motion.pushDelay=motion.releaseDelay=.01f;}
    if(identifier>=1200&&identifier<=1202)motion.releaseDelay=.1f;
    if(identifier==1300){motion.pushDepth=motion.releaseHeight=1;motion.releaseDelay=.1f;}
    int frame=motion.pressed&&enabled?tile+1:tile;
    [self sprite:name x:x y:y+motion.yOffset(_time,enabled) columns:columns rows:rows tile:frame sx:1 sy:1 rotation:0 alpha:1 blend:0];
    if(enabled){CGSize z=[_media imageSize:name];if(z.width>0){CGRect b=CGRectMake(x-z.width/columns/2,y-z.height/rows/2,z.width/columns,z.height/rows);
        if(_layoutGroup>=0&&_layout.customized()){auto t=_layout.group(_layoutGroup).transform;auto q=_layout.legacy.inverse(t.apply(heavymach::hud::Point{b.origin.x,b.origin.y}));b.origin=CGPointMake(q.x,q.y);}
        _buttons.push_back({identifier,key,b});}}
}
- (void)drawPassive:(const HMGameHUDSnapshot&)s {
    if(!s.passives)return;
    for(int i=0;i<4;++i)if(_editingLayout||s.passives->slots[i].active){
        [self sprite:@"ItemTile.png" x:10 y:200+i*16 columns:7 rows:4 tile:i+5 sx:.5 sy:.5 rotation:0 alpha:1 blend:0];
        [self font:@"NumSet02b.png" text:[NSString stringWithFormat:@"%02d",(s.passives->slots[i].active?s.passives->slots[i].number:999)] x:24 y:200+i*16 pitch:6 align:0 columns:16 rows:1 alpha:1];
    }
}
- (void)drawWeaponPreview:(const HMGameHUDSnapshot&)s {
    if(!s.slot||!s.data||(_weaponTime<0&&!_editingLayout))return;
    float elapsed=float(_time-_weaponTime),alpha=elapsed<=1?1:std::max(0.f,2.f-elapsed);
    if(_editingLayout)alpha=1;
    if(alpha<=0)return;
    int first=std::max(0,std::min(s.selectedWeapon-2,s.slot->weaponCount()-5));
    for(int i=0;i<5&&first+i<s.slot->weaponCount();++i){
        auto weapon=s.data->weapons.find(s.slot->weaponName(first+i));if(weapon==s.data->weapons.end())continue;
        double scale=first+i==s.selectedWeapon?1.4:1;
        [self sprite:@"WeaponTile.png" x:196+i*43 y:290 columns:7 rows:4 tile:weapon->second.uiTile sx:scale sy:scale rotation:0 alpha:alpha blend:0];
        if(first+i==s.selectedWeapon)[self sprite:@"SelWeapon.png" x:196+i*43 y:290 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:0 alpha:alpha blend:2];
    }
}
- (void)drawMissionIntro:(const HMGameHUDSnapshot&)s {
    if(s.mode!=0||_startTime<0)return;
    float elapsed=float(_time-_startTime);if(elapsed<0||elapsed>=4)return;
    float alpha=elapsed<1?elapsed*.5f:elapsed>=3?.5f-(elapsed-3.f)*.5f:.5f;
    [self sprite:@"Text_Mission.png" x:190 y:250 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:0 alpha:alpha blend:2];
    [self font:@"NumSet_Mission.png" text:[NSString stringWithFormat:@"%d-%d",s.mission+1,s.stage+1] x:290 y:250 pitch:16 align:0 columns:8 rows:2 alpha:alpha];
}
- (void)drawUpgrade:(const HMGameHUDSnapshot&)s {
    if(_upgradeStep==0)return;
    [self image:@"Upgrade_BG.png" x:_upgradeX y:180];
    if(s.upgradeSelection>=0&&s.upgradeSelection<3)
        [self sprite:@"Upgrade_Sel.png" x:_upgradeX+101*s.upgradeSelection-101 y:172 columns:1 rows:2 tile:1 sx:1 sy:1 rotation:0 alpha:1 blend:0];
    NSArray<NSString*> *names=@[@"Upgrade_Body.png",@"Upgrade_Top.png",@"Upgrade_Weapon.png"];
    for(int i=0;i<3;++i){
        [self sprite:names[i] x:_upgradeX+100*(i-1) y:196 columns:1 rows:7 tile:_upgradeGrades[i]+1 sx:1 sy:1 rotation:0 alpha:1 blend:0];
        if(_upgradeGrades[i]<6)[self buttonImage:@"Upgrade_Sel.png" identifier:1200+i x:_upgradeX+101*(i-1) y:172 columns:1 rows:2 tile:0 enabled:YES];
        for(int grade=0;grade<=_upgradeGrades[i]&&grade<7;++grade)
            [self sprite:@"Upgrade_Gauge.png" x:_upgradeX-127+101*i+grade*9 y:135 columns:7 rows:1 tile:grade sx:1 sy:1 rotation:0 alpha:1 blend:0];
    }
    if(s.upgradeSelection>=0&&s.upgradeSelection<3){
        int next=_upgradeGrades[s.upgradeSelection]+1;
        [self sprite:@"Upgrade_Gauge.png" x:_upgradeX-127+101*s.upgradeSelection+next*9 y:135 columns:7 rows:1 tile:next sx:1 sy:1 rotation:0 alpha:1 blend:0];
    }
    [self buttonImage:@"Common_Btn_OK.png" identifier:1300 x:_upgradeX+54 y:95 columns:1 rows:2 tile:0 enabled:YES];
}
- (void)drawPresentation:(const HMGameHUDSnapshot&)s {
    if(!s.presentation||s.presentation->step==7)return;
    const auto& p=*s.presentation;
    if(!p.loadingAsset.empty()){
        NSString *asset=[NSString stringWithUTF8String:p.loadingAsset.c_str()];
        // 0xb8f0 / 0xbf74 pass the original JPEG name to ResMgr_Tile.
        // HMMedia reads OriginalImages directly; no derived PNG suffix.
        if(s.canvasWidth>480||s.canvasHeight>320){
            // The illustration covers the world, independently of the safe-area HUD.
            // Preserve aspect ratio; only the illustration may crop vertically.
            CGSize size=[_media imageSize:asset];
            // LINEAR/REPEAT sprite textures have zero POT padding. Keep the
            // screen inside the outer texel centres so padding cannot darken its edges.
            double scale=size.width>1&&size.height>1?std::max(s.canvasWidth/(size.width-1),s.canvasHeight/(size.height-1)):1;
            [_media resetClip];[_media setDrawingOffset:CGPointZero];[_media setDrawingScale:1];
            [self sprite:asset x:s.canvasWidth/2 y:s.canvasHeight/2 columns:1 rows:1 tile:0 sx:scale sy:scale rotation:0 alpha:p.alpha blend:0];
            [_media setDrawingOffset:s.hudOffset];[_media setDrawingScale:s.hudScale];
            [_media setClipRect:CGRectMake(s.hudOffset.x,s.hudOffset.y,480*s.hudScale,320*s.hudScale)];
        }else [self sprite:asset x:240 y:160 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:0 alpha:p.alpha blend:0];
    }
    [self sprite:@"ResultTypo.png" x:180 y:280 columns:1 rows:5 tile:p.typoFrame sx:p.scale sy:p.scale rotation:p.rotation alpha:p.typoAlpha blend:2];
    // Parent loading image (240,160), button base (0,buttonY),
    // child centers (-120,-134)/(140,-134), from 0xb8f0.
    if(p.typoFrame==0||p.typoFrame==4){
        [self buttonImage:@"StageClearBtn.png" identifier:1000 x:120 y:26+p.buttonY columns:1 rows:4 tile:0 enabled:p.step==2];
        if(!p.missionEnd)[self buttonImage:@"StageClearBtn.png" identifier:1001 x:380 y:26+p.buttonY columns:1 rows:4 tile:2 enabled:p.step==2];
    }
}
- (void)draw:(const HMGameHUDSnapshot&)s {
    _time=s.time;_buttons.clear();
    bool result=s.presentation&&s.presentation->step!=7;
    _modal=s.paused||_menuStep||_upgradeStep||result;
    [self useLayoutGroup:heavymach::hud::passives];[self drawPassive:s];[self useLayoutGroup:-1];if(!_editingLayout)[self drawMissionIntro:s];[self useLayoutGroup:heavymach::hud::preview];[self drawWeaponPreview:s];[self useLayoutGroup:-1];
    [self drawLayoutBackground];
    [self useLayoutGroup:heavymach::hud::menu];
    [self buttonImage:@"GameMenu.png" identifier:100 x:38 y:279 columns:1 rows:2 tile:0 enabled:YES];
    [self useLayoutGroup:heavymach::hud::progress];
    [self sprite:@"GameUI_Exp.png" x:27+35*s.expGauge y:300 columns:1 rows:1 tile:0 sx:s.expGauge sy:1 rotation:0 alpha:1 blend:0];
    [self font:@"NumSet01.png" text:[NSString stringWithFormat:@"%02d",s.slot?s.slot->level()+1:1] x:40 y:313 pitch:7 align:0 columns:16 rows:1 alpha:1];
    [self useLayoutGroup:heavymach::hud::gauges];
    [self sprite:@"GameUI_Gauge.png" x:239 y:25 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:_hpRotation alpha:1 blend:0];
    float reloadElapsed=float(_time-_reloadTime);
    if(_reloadTime>=0&&reloadElapsed<.15f)[self sprite:@"GameUI_BlueLight.png" x:287 y:22 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:0 alpha:1.f+reloadElapsed/-.15f blend:2];
    [self sprite:@"GameUI_Gauge.png" x:287 y:22 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:s.reloadGauge*2*std::numbers::pi alpha:1 blend:0];
    [self sprite:@"GameUI_RedLight.png" x:265 y:35 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:0 alpha:std::min(1.f,std::cos(float(10*_time))+1.f) blend:2];
    [self sprite:@"GameUI_Gear.png" x:324 y:3 columns:1 rows:1 tile:0 sx:1 sy:1 rotation:_gear alpha:1 blend:0];
    [self useLayoutGroup:heavymach::hud::jump];
    [self buttonImage:@"Btn_Booster.png" identifier:1000 x:34 y:34 columns:2 rows:1 tile:0 enabled:YES];
    [self useLayoutGroup:heavymach::hud::weapon];
    [self buttonImage:@"Btn_Weapon.png" identifier:1100 x:103 y:31 columns:2 rows:1 tile:0 enabled:YES];
    [self sprite:@"WeaponTile.png" x:106 y:35 columns:7 rows:4 tile:s.weapon?s.weapon->uiTile:0 sx:1 sy:1 rotation:0 alpha:1 blend:0];
    [self font:@"NumSet02b.png" text:[NSString stringWithFormat:@"%02d",s.ammo] x:120 y:26 pitch:6 align:1 columns:16 rows:1 alpha:1];
    [self useLayoutGroup:-1];
    if(_editingLayout)return;
    [self drawUpgrade:s];[self drawPresentation:s];
    // An interrupted stage transition must leave its resume menu visible.
    if(_menuStep)for(int i=0;i<3;++i)[self buttonImage:@"GameMenuSub.png" identifier:101+i x:_menuX[i] y:220-50*i columns:1 rows:6 tile:i*2 enabled:YES];
}
- (void)tick:(double)time {
    _time=time;_gear+=_gearVelocity;_gearVelocity+=(0-_gearVelocity)/20;
    // 0x151a0 leaves the image at rotation zero. 0x161d0 updates it only
    // when HP actually changes, then stores the result as binary32.
    if(_hpDest!=_hp){
        _hp+=(_hpDest-_hp)*.125f;if(std::abs(_hp-_hpDest)<.01f)_hp=_hpDest;
        _hpRotation=float(double(_hp)*-5.183627803521752-0.6000000238418579);
    }
    if(_menuStep)for(int i=0;i<3;++i)_menuX[i]+=((_menuStep==1?220+20*i:720)-_menuX[i])/(7+2*i);
    if(_menuStep==2&&_menuX[2]>710)_menuStep=0;
    if(_upgradeStep){_upgradeX+=((_upgradeStep==1?240:660)-_upgradeX)/5;
        if(_upgradeStep==2&&_upgradeX>659){_upgradeStep=0;if(self.upgradeClosed)self.upgradeClosed();}}
}
- (void)cancelInput {_pressed=-1;_buttonStates.clear();}
- (int)receiverAt:(CGPoint)logical {
    for(auto i=_buttons.rbegin();i!=_buttons.rend();++i)if(originalButtonContains(i->bounds,logical))return i->key;
    return -1;
}
- (BOOL)touch:(CGPoint)logical state:(int)state {return [self touch:logical state:state receiver:-1];}
- (BOOL)touch:(CGPoint)logical state:(int)state receiver:(int)receiver {
    // 0x82b8 registers buttons at construction; 0xfc18 walks that array in
    // reverse and stops at the first hit. Drawing order is not receiver order.
    auto rank=[](const HUDButton& button){
        if(button.key>=10000)return 10+button.identifier-1000;
        if(button.identifier==100)return 0;
        if(button.identifier>=101&&button.identifier<=103)return button.identifier-100;
        if(button.identifier==1000)return 4;
        if(button.identifier==1100)return 5;
        if(button.identifier>=1200&&button.identifier<=1202)return 6+button.identifier-1200;
        return 9;
    };
    auto receivers=_buttons;
    std::stable_sort(receivers.begin(),receivers.end(),[&](const auto& a,const auto& b){return rank(a)<rank(b);});
    for(auto it=receivers.rbegin();it!=receivers.rend();++it){
        if(receiver>=0&&it->key!=receiver)continue;
        bool inside=originalButtonContains(it->bounds,logical);
        _buttonStates[it->key].touch(inside,state,_time);
        if(!inside)continue;
        if((state==0||state==2)&&self.button)self.button(it->identifier,state);
        return YES;
    }
    // GuiGame inherits QobBase.onTap (0x81b4), which does not consume empty space.
    return NO;
}
- (std::vector<HUDButton>)keyboardButtons {
    auto buttons=_buttons;
    buttons.erase(std::remove_if(buttons.begin(),buttons.end(),[&](const auto& b){
        return CGRectGetMidX(b.bounds)<0||CGRectGetMidX(b.bounds)>480||CGRectGetMidY(b.bounds)<0||CGRectGetMidY(b.bounds)>320;
    }),buttons.end());
    std::stable_sort(buttons.begin(),buttons.end(),[](const auto& a,const auto& b){
        if(std::fabs(CGRectGetMidY(a.bounds)-CGRectGetMidY(b.bounds))>8)return CGRectGetMidY(a.bounds)>CGRectGetMidY(b.bounds);
        return CGRectGetMidX(a.bounds)<CGRectGetMidX(b.bounds);
    });
    return buttons;
}
- (void)moveKeyboardFocus:(int)delta {
    auto buttons=[self keyboardButtons];if(buttons.empty())return;
    int index=-1;for(size_t i=0;i<buttons.size();++i)if(buttons[i].key==_keyboardFocus)index=int(i);
    if(index<0)index=delta<0?int(buttons.size())-1:0;else index=(index+delta+int(buttons.size()))%int(buttons.size());
    _keyboardFocus=buttons[index].key;_keyboardFocusVisible=YES;
}
- (CGRect)keyboardFocusRect {
    if(!_keyboardFocusVisible)return CGRectZero;
    auto buttons=[self keyboardButtons];for(const auto& b:buttons)if(b.key==_keyboardFocus)return b.bounds;
    if(buttons.empty())return CGRectZero;_keyboardFocus=buttons.front().key;return buttons.front().bounds;
}
- (void)activateKeyboardFocus {
    _keyboardFocusVisible=YES;CGRect rect=[self keyboardFocusRect];if(CGRectIsEmpty(rect))return;
    CGPoint point=CGPointMake(CGRectGetMidX(rect),CGRectGetMidY(rect));[self touch:point state:0];[self touch:point state:2];
}

@end
