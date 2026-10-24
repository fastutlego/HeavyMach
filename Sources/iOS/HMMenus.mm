#import "HMMenus.h"
#import "HMMedia.h"
#include "HMButtonState.hpp"
#include <map>
#include "../Core/GlobalInfo.hpp"
#include "../Core/Visuals.hpp"
#include "../Core/Motion.hpp"
#include <algorithm>
#include <array>
#include <ctime>
#include <vector>

namespace {
// QobButton 0x87f8 rejects <= minimum and > maximum, in binary32.
// CGRectContainsPoint instead includes minimum and excludes maximum.
bool originalButtonContains(CGRect bounds,CGPoint point) {
    const float x=float(point.x),y=float(point.y);
    return x>float(CGRectGetMinX(bounds))&&x<=float(CGRectGetMaxX(bounds))&&
           y>float(CGRectGetMinY(bounds))&&y<=float(CGRectGetMaxY(bounds));
}
struct MenuButton { int identifier; CGRect bounds; };
// Original Vector2 / GuiSelectMission::_slotPos are binary32 (0x19ba0).
struct MenuPosition {float x,y;};
enum class MenuScreen { slots, mode, mission };
struct SlotPreview {
    bool exists=false;
    heavymach::GobHvMach motion;
    std::array<heavymach::PartsState,3> parts;
    heavymach::WeaponState weapon;
};
}
@implementation HMMenus {
    HMMedia *_media;
    heavymach::SaveData *_save;
    const heavymach::GameData *_data;
    MenuScreen _screen;
    int _slot, _gameSlot, _mode, _mission, _stage, _top, _pressed;
    bool _deleteConfirmation, _slotSelectionMoved;
    std::array<MenuPosition,7> _positions, _targets;
    std::vector<MenuButton> _buttons;
    std::map<int,heavymach::HMButtonState> _buttonStates;
    std::map<int,size_t> _receiverOrder;
    size_t _nextReceiverOrder;
    std::array<SlotPreview,3> _previews;
    NSMutableArray<NSArray<NSString *> *> *_slotTexts;
    std::array<float,3> _slotGauges;
    double _time, _previewWorldTime;
    BOOL _previewWorldPaused;
}
- (instancetype)initWithMedia:(HMMedia *)media save:(heavymach::SaveData *)save data:(const heavymach::GameData *)data {
    if ((self=[super init])) { _media=media; _save=save; _data=data; _pressed=-1; _slotTexts=[NSMutableArray arrayWithArray:@[@[],@[],@[]]]; }
    return self;
}
- (void)setPreviewWorldTime:(double)time paused:(BOOL)paused {_previewWorldTime=time;_previewWorldPaused=paused;}
- (void)showSlots {
    _buttonStates.clear(); _buttons.clear();
    _receiverOrder.clear();_nextReceiverOrder=0;
    _receiverOrder[1000]=_nextReceiverOrder++;_receiverOrder[1001]=_nextReceiverOrder++;
    _screen=MenuScreen::slots; _slot=1; _pressed=-1; _deleteConfirmation=false; _slotSelectionMoved=false;
    bool found=false;
    for (int i=0;i<3;++i) if (_save->slots[i].occupied() && (!found || _save->slots[i].i32(4)>_save->slots[_slot].i32(4))) {
        found=true; _slot=i;
    }
    for (int i=0;i<3;++i) [self refreshPreview:i];
    _receiverOrder[1300]=_nextReceiverOrder++;_receiverOrder[1301]=_nextReceiverOrder++;
}
- (void)refreshPreview:(int)i {
    // 0x186d8 removes the old slot subtree, then constructs a new button.
    // Its UI receiver is appended after every receiver that already exists.
    _receiverOrder[1100+i]=_nextReceiverOrder++;
    _buttonStates.erase(1100+i);
    auto& preview=_previews[i]; preview=SlotPreview{};
    _slotTexts[i]=@[];_slotGauges[i]=0;
    if (!_save->slots[i].occupied()) return;
    preview.exists=true;
    const auto& slot=_save->slots[i];
    // GuiSelectSlot.refreshSlot 0x186d8 formats labels once when building the
    // slot's QobImageFont objects; draw only submits those existing glyphs.
    _slotGauges[i]=slot.f32(0x24);
    _slotTexts[i]=@[[NSString stringWithFormat:@"%02d",slot.level()+1],
        [self dateText:slot.i32(0)],[self dateText:slot.i32(4)],
        [NSString stringWithFormat:@"%02d",slot.bytes[0x2f]+1],
        [NSString stringWithFormat:@"%02d",slot.bytes[0x2e]+1],
        [NSString stringWithFormat:@"%02d",slot.bytes[0x2d]+1]];
    if(self.previewPlayerCreated)self.previewPlayerCreated();
    NSString *names[3]={ [NSString stringWithFormat:@"P_Body_%02d",slot.bytes[0x2f]+1],
        [NSString stringWithFormat:@"P_Top_%02d",slot.bytes[0x2e]+1],
        [NSString stringWithFormat:@"P_Wpn_Gun%02d",slot.bytes[0x2d]+1] };
    for (int j=0;j<3;++j) {
        auto found=_data->parts.find(names[j].UTF8String);
        if (found!=_data->parts.end()) {preview.parts[j].info=&found->second;if(found->second.tileCnt)[_media imageSize:[NSString stringWithUTF8String:found->second.tileName().c_str()] scope:@"shared"];}
        heavymach::attachParts(preview.parts[j],j?&preview.parts[j-1]:nullptr,j?1:0);
        // setPartsToMyMach 0x1be84 subtracts .01 after every part assignment.
        preview.motion.rotation-=.01f;
    }
    // refreshSlot -> addParts -> GobMachWeapon.init (0x11d70): one draw per
    // created weapon, even for a stationary/unselected model. The hidden World
    // keeps its previous clock; constructor draws are independent of pause.
    preview.motion.hasWeapon=preview.parts[2].info&&preview.parts[2].info->partsType==2;
    if(preview.motion.hasWeapon){preview.weapon.resetTime=float(_previewWorldTime);preview.weapon.delayTime=float(self.random31?self.random31():0)*0x1p-31f+.5f;}
    auto weapon=_data->weapons.find(names[2].UTF8String);
    if (weapon!=_data->weapons.end()) preview.weapon.weaponInfo=&weapon->second;
    preview.weapon.aimDirDest=0.1f;
    preview.motion.position={float(82+155*i),166};
    preview.motion.setMoveVel(i==_slot?1:0);
    preview.motion.player=true; preview.motion.useEngineBounce=true;
}
- (void)drawPreview:(int)i {
    NSString* previousScope=_media.resourceScope;_media.resourceScope=@"shared";
    auto& preview=_previews[i]; auto& motion=preview.motion;
    for (int j=0;j<3;++j) {
        auto sprite=j==2 ? heavymach::drawWeapon(preview.parts[j],preview.weapon,motion.position,true,3,motion.rotation,motion.engineBounce)
                         : heavymach::drawParts(preview.parts[j],motion.position,motion.rotation,motion.engineBounce,0,motion.speed,_time);
        if (!sprite.tile.empty()) [_media draw:[NSString stringWithUTF8String:sprite.tile.c_str()] x:sprite.position.x y:sprite.position.y columns:sprite.cols rows:sprite.rows tile:sprite.frame scaleX:sprite.scale.x scaleY:sprite.scale.y rotation:sprite.rotation alpha:sprite.alpha];
    }
    _media.resourceScope=previousScope;
}
- (void)tile:(NSString *)name x:(double)x y:(double)y columns:(int)columns rows:(int)rows index:(int)index sx:(double)sx {
    [_media draw:name x:x y:y columns:columns rows:rows tile:index scaleX:sx scaleY:1 rotation:0 alpha:1];
}
- (void)button:(NSString *)name identifier:(int)identifier x:(double)x y:(double)y rows:(int)rows tile:(int)tile enabled:(BOOL)enabled {
    if(!_receiverOrder.contains(identifier))_receiverOrder[identifier]=_nextReceiverOrder++;
    CGSize imageSize=[_media imageSize:name];
    auto& motion=_buttonStates[identifier];
    // Every GuiSelectSlot/Mode/Mission button explicitly disables displacement.
    motion.pushDepth=motion.releaseHeight=motion.releaseDelay=0;
    [self tile:name x:x y:y+motion.yOffset(_time,enabled) columns:1 rows:rows index:enabled && rows>1 && motion.pressed ? 1 : tile sx:1];
    if (enabled && imageSize.width>0) _buttons.push_back({identifier,CGRectMake(x-imageSize.width/2,y-imageSize.height/rows/2,imageSize.width,imageSize.height/rows)});
}
- (void)numberText:(NSString *)text x:(double)x y:(double)y pitch:(double)pitch {
    for (NSUInteger i=0;i<text.length;++i) {
        unichar ch=[text characterAtIndex:i];
        int tile=ch>='0' && ch<='9' ? ch-'0' : ch=='.'?10:ch=='/'?11:ch==':'?12:ch=='-'?13:-1;
        if (tile>=0 && tile<16) [_media draw:@"NumSet01.png" x:x+i*pitch y:y columns:16 rows:1 tile:tile scaleX:1 scaleY:1 rotation:0 alpha:1 blend:2];
    }
}
- (NSString *)dateText:(int32_t)value {
    time_t stamp=value; struct tm local{}; localtime_r(&stamp,&local);
    char text[32]; strftime(text,sizeof(text),"%Y/%m/%d %H:%M:%S",&local);
    return [NSString stringWithUTF8String:text];
}
- (void)drawSlots {
    [_media draw:@"SelSlot_BG.png" x:240 y:160];
    [self button:@"Common_Btn_OK.png" identifier:1000 x:422 y:21 rows:2 tile:0 enabled:YES];
    [self button:@"Common_Btn_DEL.png" identifier:1001 x:332 y:20 rows:2 tile:0 enabled:YES];
    for (int i=0;i<3;++i) {
        const double dx=i*155;
        if (!_previews[i].exists) [_media draw:@"SelSlot_Empty.png" x:82+dx y:156 columns:1 rows:1 tile:0 scaleX:0.9 scaleY:0.9 rotation:0 alpha:1];
        else {
            float gauge=_slotGauges[i];
            NSArray<NSString *> *labels=_slotTexts[i];
            [self tile:@"SelSlot_ExpGauge.png" x:68+dx+32*gauge y:155 columns:1 rows:1 index:0 sx:gauge];
            // 0x186d8 creates the gauge before the model in the same slot.
            [self drawPreview:i];
            [self numberText:labels[0] x:52+dx y:156 pitch:7];
            [self numberText:labels[1] x:36+dx y:106 pitch:6];
            [self numberText:labels[2] x:36+dx y:82 pitch:6];
            [self numberText:labels[3] x:50+dx y:62 pitch:7];
            [self numberText:labels[4] x:91+dx y:62 pitch:7];
            [self numberText:labels[5] x:132+dx y:62 pitch:7];
        }
        [self button:@"SelSlot_Btn.png" identifier:1100+i x:85+dx y:142 rows:1 tile:0 enabled:YES];
    }
    [_media draw:@"SelSlot_Sel.png" x:(_slotSelectionMoved?85:83)+155*_slot y:_slotSelectionMoved?142:144 columns:1 rows:1 tile:0 scaleX:1 scaleY:1 rotation:0 alpha:1 blend:2];
    if (_deleteConfirmation) {
        [_media draw:@"SelSlot_DelSlot.png" x:240 y:140];
        [self button:@"Common_Btn_YES.png" identifier:1300 x:236 y:108 rows:2 tile:0 enabled:YES];
        [self button:@"Common_Btn_NO.png" identifier:1301 x:307 y:108 rows:2 tile:0 enabled:YES];
    }
}
- (void)drawMode {
    [_media draw:@"SelMode_BG.png" x:240 y:160];
    [self button:@"Common_Btn_OK.png" identifier:1000 x:434 y:56 rows:2 tile:0 enabled:YES];
    [self button:@"SelMode_Btn_Scenario.png" identifier:1001 x:240 y:180 rows:2 tile:_mode==0 enabled:YES];
    [self button:@"SelMode_Btn_Survival.png" identifier:1002 x:240 y:104 rows:2 tile:_mode==1 enabled:YES];
}
- (void)setTop:(int)top {
    // Original order permits -1 for a new save (first row centered at y154).
    _top=std::min(std::max(top,0),_save->slots[_slot].i32(0xc)-1);
    for (int i=0;i<7;++i) {
        if (i<_top) _targets[i]={float(220-480*(_top-i)),250};
        else _targets[i]={220,float(i>=_top+3 ? -50 : 250-96*(i-_top))};
    }
}
- (void)showMissions {
    _buttonStates.clear(); _buttons.clear(); _pressed=-1;
    _receiverOrder.clear();_nextReceiverOrder=0;
    _screen=MenuScreen::mission;
    _mission=_save->slots[_slot].i32(0xc); _stage=_save->slots[_slot].i32(0x10);
    for (int i=0;i<7;++i) _positions[i]={220,float(i<3?250-96*i:-50)};
    [self setTop:_mission-1];
}
- (void)showMode {_receiverOrder.clear();_nextReceiverOrder=0;_buttonStates.clear();_buttons.clear();_slot=_gameSlot;_screen=MenuScreen::mode;_mode=0;_pressed=-1;}
- (void)startSelectedGame {if(self.startGame)self.startGame(_gameSlot,_mode,_mission,_stage);}
- (void)showMissions:(int)mission stage:(int)stage {
    [self showMissions]; _mission=mission; _stage=stage; _pressed=-1;
    [self setTop:_mission-1];
}
- (void)drawMissions {
    [_media draw:@"SelMission_BG.png" x:240 y:160];
    [self button:@"Common_Btn_OK.png" identifier:1000 x:438 y:26 rows:2 tile:0 enabled:YES];
    [self button:@"SelMission_Btn_Up.png" identifier:1001 x:438 y:210 rows:2 tile:0 enabled:YES];
    [self button:@"SelMission_Btn_Dn.png" identifier:1002 x:438 y:110 rows:2 tile:0 enabled:YES];
    const auto& save=_save->slots[_slot]; int lastMission=save.i32(0xc),lastStage=save.i32(0x10);
    for (int i=0;i<7;++i) {
        if (i>lastMission+1) continue;
        MenuPosition p=_positions[i];
        [self tile:@"SelMission_Slot.png" x:p.x y:p.y columns:1 rows:2 index:i==_mission sx:1];
        [self tile:@"SelMission_Text.png" x:p.x+18 y:p.y+26 columns:1 rows:8 index:i sx:1];
        [self button:[NSString stringWithFormat:@"SelMission_TileMission%02d.png",i+1] identifier:1200+i x:p.x-114 y:p.y+4 rows:3 tile:i>lastMission?2:i==_mission enabled:i<=lastMission];
        if (i<=lastMission) {
            [self button:@"SelMission_Btn_Left.png" identifier:1100+10*i x:p.x+78 y:p.y+4 rows:2 tile:0 enabled:YES];
            [self button:@"SelMission_Btn_Right.png" identifier:1101+10*i x:p.x+140 y:p.y+4 rows:2 tile:0 enabled:YES];
        }
        for (int j=0;j<heavymach::GlobalInfo::stageCount(i);++j) {
            int tile=(i<lastMission || (i==lastMission && j<=lastStage))?0:2;
            if (i==_mission && j==_stage) tile=1;
            [self tile:[NSString stringWithFormat:@"SelMission_Stage%02d.png",j+1] x:p.x-22+35*j y:p.y-28 columns:1 rows:3 index:tile sx:1];
        }
    }
}
- (void)draw {
    _buttons.clear();
    switch (_screen) {
        case MenuScreen::slots: [self drawSlots]; break;
        case MenuScreen::mode: [self drawMode]; break;
        case MenuScreen::mission: [self drawMissions]; break;
    }
}
- (void)tick:(double)time {
    _time=time;
    if (_screen==MenuScreen::slots&&!_previewWorldPaused) for (int i=0;i<3;++i) if (_previews[i].exists) {
        auto& preview=_previews[i]; preview.motion.setMoveVel(i==_slot ? 1 : 0);
        heavymach::MotionWorld world; world.globalTime=time; world.time=_previewWorldTime;
        world.random=[self] { return self.random31?self.random31():0; };
        for (const auto& event:preview.motion.processModel(world))
            if (event.kind==heavymach::MotionEventKind::ModelAim) heavymach::setWeaponAim(preview.weapon,event.value);
    }
    if (_screen==MenuScreen::mission) for (int i=0;i<7;++i) {
        _positions[i].x+=(_targets[i].x-_positions[i].x)/5;
        _positions[i].y+=(_targets[i].y-_positions[i].y)/5;
    }
}
- (void)activate:(int)identifier {
    NSString *sound=@"UI_Click";
    if (_screen==MenuScreen::slots) {
        if (identifier==1300 && _deleteConfirmation) {
            _save->slots[_slot].bytes[8]=0; [self refreshPreview:_slot]; _deleteConfirmation=false;
            sound=@"UI_Btn_OK";
        } else if (identifier==1301 && _deleteConfirmation) { _deleteConfirmation=false; sound=@"UI_Btn_Cancel"; }
        else if (!_deleteConfirmation && identifier==1000) {
            heavymach::GlobalInfo(_save->slots[_slot]).selectSlot(int32_t(std::time(nullptr)));
            if(self.slotSelected)self.slotSelected(_slot);
            if (self.saveChanged) self.saveChanged();
            _gameSlot=_slot; if(self.changeScreen)self.changeScreen(2); sound=@"UI_Btn_OK";
        } else if (!_deleteConfirmation && identifier==1001) {
            _deleteConfirmation=_save->slots[_slot].occupied(); sound=@"UI_Btn_OK";
        } else return;
    } else if (_screen==MenuScreen::mode) {
        if (identifier==1001) _mode=0;
        else if (identifier==1002) _mode=1;
        else if (identifier==1000) {
            sound=@"UI_Btn_OK";
            if (_mode==0) {if(self.changeScreen)self.changeScreen(4);}
            else {_mission=4;_stage=4;if(self.changeScreen)self.changeScreen(6);}
        } else return;
    } else {
        if (identifier==1000) { if (self.changeScreen) self.changeScreen(6); sound=@"UI_Btn_OK"; }
        else if (identifier==1001) [self setTop:_top-1];
        else if (identifier==1002) [self setTop:_top+1];
        else if (identifier>=1200 && identifier<1207) { _mission=identifier-1200; _stage=0; [self setTop:_top]; }
        else if (identifier>=1100 && identifier<1170) {
            int mission=(identifier-1100)/10, direction=(identifier-1100)%10;
            if (_mission!=mission) { _mission=mission; _stage=0; }
            _stage=direction==0 ? std::max(0,_stage-1) : _stage+1;
            if (_mission==_save->slots[_slot].i32(0xc)) _stage=std::min(_stage,_save->slots[_slot].i32(0x10));
            _stage=std::min(_stage,heavymach::GlobalInfo::stageCount(_mission)-1);
            [self setTop:_top];
        } else return;
    }
    [_media sound:sound];
}
- (void)touch:(CGPoint)logical state:(int)state {
    // QobManager 0xfc18 stops immediately after the first receiver consumes.
    auto receivers=_buttons;
    std::stable_sort(receivers.begin(),receivers.end(),[&](const auto& a,const auto& b){
        return _receiverOrder[a.identifier]<_receiverOrder[b.identifier];
    });
    for(auto it=receivers.rbegin();it!=receivers.rend();++it){
        bool inside=originalButtonContains(it->bounds,logical);
        _buttonStates[it->identifier].touch(inside,state,_time);
        if(!inside)continue;
        int hit=it->identifier;
        if(state==0&&_screen==MenuScreen::slots&&!_deleteConfirmation&&hit>=1100&&hit<1103){
            _slot=hit-1100;_slotSelectionMoved=true;[_media sound:@"UI_Click"];
        }else if(state==2)[self activate:hit];
        break;
    }
}
@end
