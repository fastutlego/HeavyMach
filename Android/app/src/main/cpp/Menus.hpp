#pragma once
#include "Media.hpp"
#include "HMButtonState.hpp"
#include "GameSession.hpp"
#include "HUDConfiguration.hpp"
#include "GlobalInfo.hpp"
#include "Visuals.hpp"
#include "Motion.hpp"
#include <map>
#include <array>
#include <vector>
#include <numbers>
#include <ctime>
namespace hm {

// QobButton 0x87f8 rejects <= minimum and > maximum, in binary32.
// CGRectContainsPoint instead includes minimum and excludes maximum.

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

class Menus {
public:

    Media *_media{};
    int _keyboardFocus{};
    bool _keyboardFocusVisible{};
    heavymach::SaveData *_save{};
    const heavymach::GameData *_data{};
    MenuScreen _screen{};
    int _slot{}, _gameSlot{}, _mode{}, _mission{}, _stage{}, _top{}, _pressed{};
    bool _deleteConfirmation{}, _slotSelectionMoved{};
    std::array<MenuPosition,7> _positions{}, _targets{};
    std::vector<MenuButton> _buttons{};
    std::map<int,heavymach::HMButtonState> _buttonStates{};
    std::map<int,size_t> _receiverOrder{};
    size_t _nextReceiverOrder{};
    std::array<SlotPreview,3> _previews{};
    std::array<std::vector<std::string>,3> _slotTexts{};
    std::array<float,3> _slotGauges{};
    double _time{}, _previewWorldTime{};
    bool _previewWorldPaused{};
    CGFloat _canvasWidth{};
    CGRect _safeRect{};
    CGPoint _layoutOffset{};
    bool _cancelledPressByLayout{};
Menus(Media* media,heavymach::SaveData* save,const heavymach::GameData* data):_media(media),_save(save),_data(data){_canvasWidth=480;_safeRect=CGRectMake(0,0,480,320);_pressed=-1;}
std::function<int32_t()> random31; std::function<void(int)> changeScreen,slotSelected;std::function<void(int,int,int,int)> startGame;std::function<void()> saveChanged,previewPlayerCreated;
void setCanvasWidth(CGFloat width, CGRect safeRect) {
    if(_canvasWidth!=width||!CGRectEqualToRect(_safeRect,safeRect)) {
        // A resize cannot release an old press onto a newly positioned button.
        _pressed=-1; for(auto& entry:_buttonStates){if(entry.second.pressed)_cancelledPressByLayout=true;entry.second.touch(false,3,_time);}
        _buttons.clear();
    }
    _canvasWidth=width;_safeRect=safeRect;
}
void layoutOffset(CGPoint offset) {_layoutOffset=offset;_media->setDrawingOffset(offset);}
// Reuse exact original pixels through GL clipping; no new crop/bitmap per frame.
void piece(std::string  name, CGRect source, CGPoint offset) {
    this->layoutOffset(CGPointZero);
    _media->setClipRect(CGRectOffset(source,offset.x,offset.y));
    _media->draw(name,240+offset.x,160+offset.y);
    _media->resetClip();
}
void wideBackdrop() {
    this->layoutOffset(CGPointZero);
    CGSize size=_media->imageSize("Extensions/MenuMetal.png");
    CGFloat scale=std::max(_canvasWidth/size.width,320/size.height);
    _media->draw("Extensions/MenuMetal.png",_canvasWidth/2,160,1,1,0,scale,scale,0,1);
}
void connectedBackdrop(std::string  extension, std::string  original) {
    // Each screen has an outpaint of its own original. Discard the generated
    // centre; only the two outer quarters join the immutable 480px background.
    this->layoutOffset(CGPointZero);
    CGSize size=_media->imageSize(extension);
    CGFloat center=CGRectGetMidX(_safeRect),scale=320/size.height;
    CGFloat tileWidth=size.width*scale/4;
    for(int side:{-1,1}) {
        CGFloat extent=std::max(tileWidth,side<0?center-240:_canvasWidth-center-240);
        _media->draw(extension,center+side*(240+extent/2),160,4,1,side<0?0:3,extent/(size.width/4),scale,0,1);
    }
    // Blend only outside the original bounds, retaining its interior pixels.
    _media->drawEdgeContinuation(original,center,160,1,0,24);
    this->layoutOffset(CGPointMake(center-240,0));
}
CGFloat slotOffset(int i) {
    if(_canvasWidth<=480)return 0;
    CGFloat first=CGRectGetMinX(_safeRect)+82,last=CGRectGetMaxX(_safeRect)-85;
    return first+(last-first)*i/2-(85+155*i);
}
void setPreviewWorldTime(double time, bool paused) {_previewWorldTime=time;_previewWorldPaused=paused;}
void showSlots() {
    _buttonStates.clear(); _buttons.clear();
    _receiverOrder.clear();_nextReceiverOrder=0;
    _receiverOrder[1000]=_nextReceiverOrder++;_receiverOrder[1001]=_nextReceiverOrder++;
    _screen=MenuScreen::slots; _slot=1; _pressed=-1; _deleteConfirmation=false; _slotSelectionMoved=false;
    bool found=false;
    for (int i=0;i<3;++i) if (_save->slots[i].occupied() && (!found || _save->slots[i].i32(4)>_save->slots[_slot].i32(4))) {
        found=true; _slot=i;
    }
    for (int i=0;i<3;++i) this->refreshPreview(i);
    _receiverOrder[1300]=_nextReceiverOrder++;_receiverOrder[1301]=_nextReceiverOrder++;
}
void refreshPreview(int i) {
    // 0x186d8 removes the old slot subtree, then constructs a new button.
    // Its UI receiver is appended after every receiver that already exists.
    _receiverOrder[1100+i]=_nextReceiverOrder++;
    _buttonStates.erase(1100+i);
    auto& preview=_previews[i]; preview=SlotPreview{};
    _slotTexts[i]={};_slotGauges[i]=0;
    if (!_save->slots[i].occupied()) return;
    preview.exists=true;
    const auto& slot=_save->slots[i];
    // GuiSelectSlot.refreshSlot 0x186d8 formats labels once when building the
    // slot's QobImageFont objects; draw only submits those existing glyphs.
    _slotGauges[i]=slot.f32(0x24);
    _slotTexts[i]={fmt("%02d",slot.level()+1),
        this->dateText(slot.i32(0)),this->dateText(slot.i32(4)),
        fmt("%02d",slot.bytes[0x2f]+1),
        fmt("%02d",slot.bytes[0x2e]+1),
        fmt("%02d",slot.bytes[0x2d]+1)};
    if(this->previewPlayerCreated)this->previewPlayerCreated();
    std::string names[3]={ fmt("P_Body_%02d",slot.bytes[0x2f]+1),
        fmt("P_Top_%02d",slot.bytes[0x2e]+1),
        fmt("P_Wpn_Gun%02d",slot.bytes[0x2d]+1) };
    for (int j=0;j<3;++j) {
        auto found=_data->parts.find(names[j].c_str());
        if (found!=_data->parts.end()) {preview.parts[j].info=&found->second;if(found->second.tileCnt)_media->imageSize(std::string(found->second.tileName().c_str()),"shared");}
        heavymach::attachParts(preview.parts[j],j?&preview.parts[j-1]:nullptr,j?1:0);
        // setPartsToMyMach 0x1be84 subtracts .01 after every part assignment.
        preview.motion.rotation-=.01f;
    }
    // refreshSlot -> addParts -> GobMachWeapon.init (0x11d70): one draw per
    // created weapon, even for a stationary/unselected model. The hidden World
    // keeps its previous clock; constructor draws are independent of pause.
    preview.motion.hasWeapon=preview.parts[2].info&&preview.parts[2].info->partsType==2;
    if(preview.motion.hasWeapon){preview.weapon.resetTime=float(_previewWorldTime);preview.weapon.delayTime=float(this->random31?this->random31():0)*0x1p-31f+.5f;}
    auto weapon=_data->weapons.find(names[2].c_str());
    if (weapon!=_data->weapons.end()) preview.weapon.weaponInfo=&weapon->second;
    preview.weapon.aimDirDest=0.1f;
    preview.motion.position={float(82+155*i),166};
    preview.motion.setMoveVel(i==_slot?1:0);
    preview.motion.player=true; preview.motion.useEngineBounce=true;
}
void drawPreview(int i) {
    std::string  previousScope=_media->resourceScope;_media->resourceScope="shared";
    auto& preview=_previews[i]; auto& motion=preview.motion;
    for (int j=0;j<3;++j) {
        auto sprite=j==2 ? heavymach::drawWeapon(preview.parts[j],preview.weapon,motion.position,true,3,motion.rotation,motion.engineBounce)
                         : heavymach::drawParts(preview.parts[j],motion.position,motion.rotation,motion.engineBounce,0,motion.speed,_time);
        if (!sprite.tile.empty()) _media->draw(std::string(sprite.tile.c_str()),sprite.position.x,sprite.position.y,sprite.cols,sprite.rows,sprite.frame,sprite.scale.x,sprite.scale.y,sprite.rotation,sprite.alpha);
    }
    _media->resourceScope=previousScope;
}
void tile(std::string  name, double x, double y, int columns, int rows, int index, double sx) {
    _media->draw(name,x,y,columns,rows,index,sx,1,0,1);
}
void button(std::string  name, int identifier, double x, double y, int rows, int tile, bool enabled) {
    if(!_receiverOrder.contains(identifier))_receiverOrder[identifier]=_nextReceiverOrder++;
    CGSize imageSize=_media->imageSize(name);
    auto& motion=_buttonStates[identifier];
    // Every GuiSelectSlot/Mode/Mission button explicitly disables displacement.
    motion.pushDepth=motion.releaseHeight=motion.releaseDelay=0;
    this->tile(name,x,y+motion.yOffset(_time,enabled),1,rows,enabled && rows>1 && motion.pressed ? 1 : tile,1);
    if (enabled && imageSize.width>0) _buttons.push_back({identifier,CGRectMake(x+_layoutOffset.x-imageSize.width/2,y+_layoutOffset.y-imageSize.height/rows/2,imageSize.width,imageSize.height/rows)});
}
void numberText(std::string  text, double x, double y, double pitch) {
    for (size_t i=0;i<text.size();++i) {
        char ch=text.at(i);
        int tile=ch>='0' && ch<='9' ? ch-'0' : ch=='.'?10:ch=='/'?11:ch==':'?12:ch=='-'?13:-1;
        if (tile>=0 && tile<16) _media->draw("NumSet01.png",x+i*pitch,y,16,1,tile,1,1,0,1,2);
    }
}
std::string  dateText(int32_t value) {
    time_t stamp=value; struct tm local{}; localtime_r(&stamp,&local);
    char text[32]; strftime(text,sizeof(text),"%Y/%m/%d %H:%M:%S",&local);
    return std::string(text);
}
void drawSlots() {
    const bool wide=_canvasWidth>480;
    if(wide) {
        this->wideBackdrop();
        this->piece("SelSlot_BG.png",CGRectMake(0,242,480,78),CGPointMake(CGRectGetMidX(_safeRect)-240,12));
    } else _media->draw("SelSlot_BG.png",240,160);
    CGFloat bottom=wide?std::max(CGFloat(21),CGRectGetMinY(_safeRect)+22):21;
    this->button("Common_Btn_OK.png",1000,wide?CGRectGetMaxX(_safeRect)-58:422,bottom,2,0,true);
    this->button("Common_Btn_DEL.png",1001,wide?CGRectGetMinX(_safeRect)+58:332,wide?bottom:20,2,0,true);
    for (int i=0;i<3;++i) {
        const double dx=i*155;
        CGPoint offset=CGPointMake(this->slotOffset(i),wide?CGRectGetMinY(_safeRect)+4:0);
        if(wide)this->piece("SelSlot_BG.png",CGRectMake(7+dx,43,155,199),offset);
        this->layoutOffset(offset);
        if (!_previews[i].exists) _media->draw("SelSlot_Empty.png",82+dx,156,1,1,0,0.9,0.9,0,1);
        else {
            float gauge=_slotGauges[i];
            std::vector<std::string> labels=_slotTexts[i];
            this->tile("SelSlot_ExpGauge.png",68+dx+32*gauge,155,1,1,0,gauge);
            // 0x186d8 creates the gauge before the model in the same slot.
            this->drawPreview(i);
            this->numberText(labels[0],52+dx,156,7);
            this->numberText(labels[1],36+dx,106,6);
            this->numberText(labels[2],36+dx,82,6);
            this->numberText(labels[3],50+dx,62,7);
            this->numberText(labels[4],91+dx,62,7);
            this->numberText(labels[5],132+dx,62,7);
        }
        this->button("SelSlot_Btn.png",1100+i,85+dx,142,1,0,true);
    }
    this->layoutOffset(CGPointMake(this->slotOffset(_slot),wide?CGRectGetMinY(_safeRect)+4:0));
    _media->draw("SelSlot_Sel.png",(_slotSelectionMoved?85:83)+155*_slot,_slotSelectionMoved?142:144,1,1,0,1,1,0,1,2);
    this->layoutOffset(CGPointMake(wide?CGRectGetMidX(_safeRect)-240:0,0));
    if (_deleteConfirmation) {
        _media->draw("SelSlot_DelSlot.png",240,140);
        this->button("Common_Btn_YES.png",1300,236,108,2,0,true);
        this->button("Common_Btn_NO.png",1301,307,108,2,0,true);
    }
}
void drawMode() {
    if(_canvasWidth>480)_media->drawJoinedBackground("SelMode_BG.png",240,160,1,0);
    else _media->draw("SelMode_BG.png",240,160);
    this->button("Common_Btn_OK.png",1000,434,56,2,0,true);
    this->button("SelMode_Btn_Scenario.png",1001,240,180,2,_mode==0,true);
    this->button("SelMode_Btn_Survival.png",1002,240,104,2,_mode==1,true);
}
void setTop(int top) {
    // Original order permits -1 for a new save (first row centered at y154).
    _top=std::min(std::max(top,0),_save->slots[_slot].i32(0xc)-1);
    for (int i=0;i<7;++i) {
        if (i<_top) _targets[i]={float(220-480*(_top-i)),250};
        else _targets[i]={220,float(i>=_top+3 ? -50 : 250-96*(i-_top))};
    }
}
void showMissions() {
    _buttonStates.clear(); _buttons.clear(); _pressed=-1;
    _receiverOrder.clear();_nextReceiverOrder=0;
    _screen=MenuScreen::mission;
    _mission=_save->slots[_slot].i32(0xc); _stage=_save->slots[_slot].i32(0x10);
    for (int i=0;i<7;++i) _positions[i]={220,float(i<3?250-96*i:-50)};
    this->setTop(_mission-1);
}
bool isMissionSelection() {return _screen==MenuScreen::mission;}
void showMode() {_receiverOrder.clear();_nextReceiverOrder=0;_buttonStates.clear();_buttons.clear();_slot=_gameSlot;_screen=MenuScreen::mode;_mode=0;_pressed=-1;}
void startSelectedGame() {if(this->startGame)this->startGame(_gameSlot,_mode,_mission,_stage);}
void showMissions(int mission, int stage) {
    this->showMissions(); _mission=mission; _stage=stage; _pressed=-1;
    this->setTop(_mission-1);
}
void drawMissions() {
    if(_canvasWidth>480)_media->drawJoinedBackground("SelMission_BG.png",240,160,1,0);
    else _media->draw("SelMission_BG.png",240,160);
    this->button("Common_Btn_OK.png",1000,438,_canvasWidth>480?std::max(CGFloat(26),CGRectGetMinY(_safeRect)+22):26,2,0,true);
    this->button("SelMission_Btn_Up.png",1001,438,210,2,0,true);
    this->button("SelMission_Btn_Dn.png",1002,438,110,2,0,true);
    const auto& save=_save->slots[_slot]; int lastMission=save.i32(0xc),lastStage=save.i32(0x10);
    for (int i=0;i<7;++i) {
        if (i>lastMission+1) continue;
        MenuPosition p=_positions[i];
        this->tile("SelMission_Slot.png",p.x,p.y,1,2,i==_mission,1);
        this->tile("SelMission_Text.png",p.x+18,p.y+26,1,8,i,1);
        this->button(fmt("SelMission_TileMission%02d.png",i+1),1200+i,p.x-114,p.y+4,3,i>lastMission?2:i==_mission,i<=lastMission);
        if (i<=lastMission) {
            this->button("SelMission_Btn_Left.png",1100+10*i,p.x+78,p.y+4,2,0,true);
            this->button("SelMission_Btn_Right.png",1101+10*i,p.x+140,p.y+4,2,0,true);
        }
        for (int j=0;j<heavymach::GlobalInfo::stageCount(i);++j) {
            int tile=(i<lastMission || (i==lastMission && j<=lastStage))?0:2;
            if (i==_mission && j==_stage) tile=1;
            this->tile(fmt("SelMission_Stage%02d.png",j+1),p.x-22+35*j,p.y-28,1,3,tile,1);
        }
    }
}
void draw() {
    _buttons.clear();this->layoutOffset(CGPointZero);
    if(_canvasWidth>480&&_screen==MenuScreen::mode)this->connectedBackdrop("Extensions/ModeWide.png","SelMode_BG.png");
    if(_canvasWidth>480&&_screen==MenuScreen::mission)this->connectedBackdrop("Extensions/MissionWide.png","SelMission_BG.png");
    switch (_screen) {
        case MenuScreen::slots: this->drawSlots(); break;
        case MenuScreen::mode: this->drawMode(); break;
        case MenuScreen::mission: this->drawMissions(); break;
    }
    this->layoutOffset(CGPointZero);
}
void tick(double time) {
    _time=time;
    if (_screen==MenuScreen::slots&&!_previewWorldPaused) for (int i=0;i<3;++i) if (_previews[i].exists) {
        auto& preview=_previews[i]; preview.motion.setMoveVel(i==_slot ? 1 : 0);
        heavymach::MotionWorld world; world.globalTime=time; world.time=_previewWorldTime;
        world.random=[this] { return this->random31?this->random31():0; };
        for (const auto& event:preview.motion.processModel(world))
            if (event.kind==heavymach::MotionEventKind::ModelAim) heavymach::setWeaponAim(preview.weapon,event.value);
    }
    if (_screen==MenuScreen::mission) for (int i=0;i<7;++i) {
        _positions[i].x+=(_targets[i].x-_positions[i].x)/5;
        _positions[i].y+=(_targets[i].y-_positions[i].y)/5;
    }
}
void activate(int identifier) {
    std::string sound="UI_Click";
    if (_screen==MenuScreen::slots) {
        if (identifier==1300 && _deleteConfirmation) {
            _save->slots[_slot].bytes[8]=0; this->refreshPreview(_slot); _deleteConfirmation=false;
            sound="UI_Btn_OK";
        } else if (identifier==1301 && _deleteConfirmation) { _deleteConfirmation=false; sound="UI_Btn_Cancel"; }
        else if (!_deleteConfirmation && identifier==1000) {
            heavymach::GlobalInfo(_save->slots[_slot]).selectSlot(int32_t(std::time(nullptr)));
            if(this->slotSelected)this->slotSelected(_slot);
            if (this->saveChanged) this->saveChanged();
            _gameSlot=_slot; if(this->changeScreen)this->changeScreen(2); sound="UI_Btn_OK";
        } else if (!_deleteConfirmation && identifier==1001) {
            _deleteConfirmation=_save->slots[_slot].occupied(); sound="UI_Btn_OK";
        } else return;
    } else if (_screen==MenuScreen::mode) {
        if (identifier==1001) _mode=0;
        else if (identifier==1002) _mode=1;
        else if (identifier==1000) {
            sound="UI_Btn_OK";
            if (_mode==0) {if(this->changeScreen)this->changeScreen(4);}
            else {_mission=4;_stage=4;if(this->changeScreen)this->changeScreen(6);}
        } else return;
    } else {
        if (identifier==1000) { if (this->changeScreen) this->changeScreen(6); sound="UI_Btn_OK"; }
        else if (identifier==1001) this->setTop(_top-1);
        else if (identifier==1002) this->setTop(_top+1);
        else if (identifier>=1200 && identifier<1207) { _mission=identifier-1200; _stage=0; this->setTop(_top); }
        else if (identifier>=1100 && identifier<1170) {
            int mission=(identifier-1100)/10, direction=(identifier-1100)%10;
            if (_mission!=mission) { _mission=mission; _stage=0; }
            _stage=direction==0 ? std::max(0,_stage-1) : _stage+1;
            if (_mission==_save->slots[_slot].i32(0xc)) _stage=std::min(_stage,_save->slots[_slot].i32(0x10));
            _stage=std::min(_stage,heavymach::GlobalInfo::stageCount(_mission)-1);
            this->setTop(_top);
        } else return;
    }
    _media->sound(sound);
}
void touch(CGPoint logical, int state) {
    if(state==0)_cancelledPressByLayout=false;
    if(_cancelledPressByLayout){if(state>=2)_cancelledPressByLayout=false;return;}
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
            _slot=hit-1100;_slotSelectionMoved=true;_media->sound("UI_Click");
        }else if(state==2)this->activate(hit);
        break;
    }
}
std::vector<MenuButton> keyboardButtons() {
    auto buttons=_buttons;
    buttons.erase(std::remove_if(buttons.begin(),buttons.end(),[&](const auto& b){
        return CGRectGetMidX(b.bounds)<0||CGRectGetMidX(b.bounds)>_canvasWidth||CGRectGetMidY(b.bounds)<0||CGRectGetMidY(b.bounds)>320;
    }),buttons.end());
    std::stable_sort(buttons.begin(),buttons.end(),[](const auto& a,const auto& b){
        if(std::fabs(CGRectGetMidY(a.bounds)-CGRectGetMidY(b.bounds))>8)return CGRectGetMidY(a.bounds)>CGRectGetMidY(b.bounds);
        return CGRectGetMidX(a.bounds)<CGRectGetMidX(b.bounds);
    });
    return buttons;
}
void moveKeyboardFocus(int delta) {
    auto buttons=this->keyboardButtons();if(buttons.empty())return;
    int index=-1;for(size_t i=0;i<buttons.size();++i)if(buttons[i].identifier==_keyboardFocus)index=int(i);
    if(index<0)index=delta<0?int(buttons.size())-1:0;else index=(index+delta+int(buttons.size()))%int(buttons.size());
    _keyboardFocus=buttons[index].identifier;_keyboardFocusVisible=true;
}
CGRect keyboardFocusRect() {
    if(!_keyboardFocusVisible)return CGRectZero;
    auto buttons=this->keyboardButtons();for(const auto& b:buttons)if(b.identifier==_keyboardFocus)return b.bounds;
    if(buttons.empty())return CGRectZero;_keyboardFocus=buttons.front().identifier;return buttons.front().bounds;
}
void activateKeyboardFocus() {
    _keyboardFocusVisible=true;CGRect rect=this->keyboardFocusRect();if(CGRectIsEmpty(rect))return;
    CGPoint point=CGPointMake(CGRectGetMidX(rect),CGRectGetMidY(rect));this->touch(point,0);this->touch(point,2);
}
};
}
