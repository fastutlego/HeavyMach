#import "HMGameView.h"
#import "HMValidationControls.h"
#import "HMGLRenderer.h"
#import <QuartzCore/CAEAGLLayer.h>
#import "HMMedia.h"
#import "HMMenus.h"
#import "HMGameHUD.h"
#import "HMOpenALAudio.h"
#if defined(HM_VALIDATION) && HM_VALIDATION
#import "HMValidationPanel.h"
#endif
#import <CoreMotion/CoreMotion.h>
#import <mach/mach_time.h>
#import <TargetConditionals.h>
#include "GameSession.hpp"
#include "TiltState.hpp"
#include "CameraShake.hpp"
#include "TitleState.hpp"
#include "LoadingTransition.hpp"
#include "Viewport.hpp"
#include "CanvasGeometry.hpp"
#include "GlobalInfo.hpp"
#include <ctime>

@implementation HMGameView {
    HMMedia *_media;
    UIGraphicsImageRenderer *_logicalRenderer; // CPU reference, offscreen validation only.
    HMGLRenderer *_glRenderer;
    UILabel *_renderError;
    HMMenus *_menus;
    HMGameHUD *_hud;
    CMMotionManager *_motion;
    NSTimer *_motionTimer;
    heavymach::TiltState _tilt;
    heavymach::CameraShake _titleShake, _worldShake;
    CGPoint _keyboardAim;
    BOOL _hasKeyboardAim;
    heavymach::PlayerPassives _passives;
    std::unique_ptr<heavymach::GameSession> _session;
    int _slotIndex, _ammoPreview;
    heavymach::SaveSlot *_originalSelectedSlot, *_originalWeaponSlot;
    int _originalSelectedWeapon, _originalWeaponIndex;
    BOOL _gameMenu, _leftPressed, _rightPressed, _upPressed, _downPressed, _keyboardFire;
    float _hpGauge, _expGauge, _reloadGauge;
    heavymach::GameData _data;
    heavymach::SaveData _save;
    std::shared_ptr<const heavymach::SaveSlot> _pendingSaveSnapshot;
    NSURL *_saveURL;
    BOOL _showMenu;
    NSString *_screenResourceScope;
    NSUInteger _screenResourceGeneration;
    NSString *_loadError;
    NSTimer *_frameTimer;
    double _nextTimerDue, _nextMotionDue;
    heavymach::TitleState _title;
    heavymach::LoadingTransition _loading;
    heavymach::Viewport _viewport;
    CGFloat _canvasWidth, _hudScale;
    CGPoint _hudOffset;
    BOOL _wideCanvasEnabled;
    CGRect _menuSafeRect;
    NSHashTable<UITouch*> *_worldTouches, *_hudTouches;
    double _time, _retainedWorldTime;
    uint32_t _randomState;
    heavymach::Vector2 _retainedWorldCamera;
    BOOL _active, _trace, _resumeRequired;
    double _lastInteraction;
    NSHashTable<UITouch*> *_heldTouches;
    int _traceSecond;
#if defined(HM_VALIDATION) && HM_VALIDATION
    UIButton *_validationButton;
    HMValidationControls *_validationControls;
    std::optional<heavymach::ValidationScenario> _validationFixture;
#endif
}
+ (Class)layerClass { return CAEAGLLayer.class; }
- (instancetype)initWithFrame:(CGRect)frame {
    if ((self=[super initWithFrame:frame])) {
        self.backgroundColor=UIColor.blackColor;
        self.multipleTouchEnabled=YES;
        self.contentScaleFactor=UIScreen.mainScreen.nativeScale;
        ((CAEAGLLayer*)self.layer).opaque=YES;
        ((CAEAGLLayer*)self.layer).drawableProperties=@{kEAGLDrawablePropertyRetainedBacking:@NO,kEAGLDrawablePropertyColorFormat:kEAGLColorFormatRGB565};
        _glRenderer=[HMGLRenderer new];
        _renderError=[[UILabel alloc] initWithFrame:self.bounds];
        _renderError.textColor=UIColor.whiteColor;_renderError.numberOfLines=0;
        _renderError.userInteractionEnabled=NO;_renderError.hidden=YES;
        [self addSubview:_renderError];
        _heldTouches=[NSHashTable weakObjectsHashTable];_worldTouches=[NSHashTable weakObjectsHashTable];_hudTouches=[NSHashTable weakObjectsHashTable];_lastInteraction=[self interactionTime];
        _canvasWidth=480;_hudScale=1;
        _wideCanvasEnabled=YES;
        _media=[HMMedia new];_media.textureRenderer=_glRenderer; _active=YES;_ammoPreview=-1;
        UIGraphicsImageRendererFormat *format=[UIGraphicsImageRendererFormat defaultFormat];
        format.scale=1;format.opaque=YES;format.preferredRange=UIGraphicsImageRendererFormatRangeStandard;
        _logicalRenderer=[[UIGraphicsImageRenderer alloc] initWithSize:CGSizeMake(480,320) format:format];
        _randomState=uint32_t(mach_absolute_time());
        _trace=[NSProcessInfo.processInfo.environment[@"HM_TRACE"] isEqualToString:@"1"];
        _saveURL=[[NSFileManager.defaultManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask].firstObject URLByAppendingPathComponent:@"HvM.dat"];
        try {
            NSString *raw=[NSBundle.mainBundle.resourcePath stringByAppendingPathComponent:@"RawData"];
            _data=heavymach::GameData::loadDirectory(raw.fileSystemRepresentation);
            if ([NSFileManager.defaultManager fileExistsAtPath:_saveURL.path]) {
                NSError *error=nil;
                NSData *bytes=[NSData dataWithContentsOfURL:_saveURL options:0 error:&error];
                if (!bytes) throw std::runtime_error(error.localizedDescription.UTF8String);
                _save=heavymach::SaveData::decode({static_cast<const uint8_t *>(bytes.bytes),bytes.length});
            }
        } catch (const std::exception& error) {
            _loadError=[NSString stringWithUTF8String:error.what()];
            NSLog(@"HeavyMach load failed; existing save preserved: %@",_loadError);
        }
        _menus=[[HMMenus alloc] initWithMedia:_media save:&_save data:&_data];
        __weak HMGameView *weakSelf=self;
        _menus.random31=^{return [weakSelf nextOriginalRandom];};
        _menus.slotSelected=^(int slot){HMGameView* view=weakSelf;if(view)view->_originalSelectedSlot=&view->_save.slots.at(slot);};
        _menus.previewPlayerCreated=^{
            HMGameView* view=weakSelf;if(!view)return;
            NSNumber* charge=[view->_hud changeWeaponForSlot:view->_originalSelectedSlot selectedWeapon:view->_originalSelectedWeapon selectedItemSlot:view->_originalWeaponSlot selectedItemIndex:view->_originalWeaponIndex time:view->_time];
            if(charge)view->_ammoPreview=charge.intValue;
        };
        _menus.saveChanged=^{[weakSelf persistSave];};
        _menus.changeScreen=^(int screen){[weakSelf requestScreen:screen];};
        _menus.startGame=^(int slot,int mode,int mission,int stage) {
            [weakSelf startSlot:slot mode:mode mission:mission stage:stage];
        };
        for(NSString* name in @[@"Loading_Top.png",@"Loading_Bottom.png",@"Loading_Left.png",@"Loading_Right.png",@"GameUI_Btm.png",@"GameUI_Top.png",@"GameMenu.png",@"GameMenuSub.png",@"GameUI_Exp.png",@"NumSet01.png",@"GameUI_Gauge.png",@"GameUI_BlueLight.png",@"GameUI_RedLight.png",@"GameUI_Gear.png",@"Btn_Booster.png",@"Btn_Weapon.png",@"WeaponTile.png",@"NumSet02b.png",@"SelWeapon.png",@"ItemTile.png",@"Text_Mission.png",@"NumSet_Mission.png"])@autoreleasepool{[_media imageSize:name scope:@"shared"];}
        _hud=[[HMGameHUD alloc] initWithMedia:_media];
        _hud.button=^(int identifier,int state) {[weakSelf gameButton:identifier state:state];};
        _hud.upgradeClosed=^{[weakSelf finishUpgrade];};
        [self prepareScreenResources:1];
        _motion=[CMMotionManager new];
        if (_motion.accelerometerAvailable) [_motion startAccelerometerUpdates];
        // The host polls the latest SDL sample at the original 50 Hz delegate interval.
        [self scheduleOriginalMotionTimer:CACurrentMediaTime()+heavymach::TiltState::interval];
        // Original guest NSTimer @0x20b8 runs through touchHLE's deadline rules.
        [self scheduleOriginalTimer:CACurrentMediaTime()+0.01666666753590107];
        self.accessibilityIdentifier=@"HeavyMach.Restored.GameView";
        self.accessibilityLabel=@"Heavy Mach";
#if defined(HM_VALIDATION) && HM_VALIDATION
        _validationButton=[UIButton buttonWithType:UIButtonTypeSystem];
        [_validationButton setTitle:@"검증" forState:UIControlStateNormal];
        _validationButton.backgroundColor=[UIColor.systemBackgroundColor colorWithAlphaComponent:.9];
        _validationButton.layer.cornerRadius=8;_validationButton.accessibilityIdentifier=@"HMValidation.Open";
        [_validationButton addTarget:self action:@selector(openValidation) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_validationButton];
        _validationControls=[HMValidationControls new];
        [_validationControls addTarget:self action:@selector(validationDirectionChanged:) forControlEvents:UIControlEventValueChanged];
        [self addSubview:_validationControls];
#endif
    }
    return self;
}
#if defined(HM_VALIDATION) && HM_VALIDATION
- (void)validationDirectionChanged:(HMValidationControls *)sender {
    _leftPressed=NO;_rightPressed=NO;_upPressed=NO;_downPressed=NO;
}
- (void)stopValidationDirection {
    if(_validationControls.selectedSegmentIndex!=3)_validationControls.selectedSegmentIndex=1;
    _leftPressed=NO;_rightPressed=NO;_upPressed=NO;_downPressed=NO;
}
- (void)updateValidationKeyboardDirection {
    _validationControls.selectedSegmentIndex=1+int(_rightPressed)-int(_leftPressed);
}
- (void)openValidation {
    UIViewController *presenter=self.window.rootViewController;if(!presenter||presenter.presentedViewController)return;
    [self stopValidationDirection];
    HMValidationPanel *panel=[[HMValidationPanel alloc] initWithData:&_data];
    __weak HMGameView *weakSelf=self;
    panel.wideCanvas=_wideCanvasEnabled;
    panel.canvasChanged=^(BOOL enabled){[weakSelf setWideCanvasEnabled:enabled];};
    auto pausedSession=_session.get();bool wasPaused=_session&&_session->world().paused;
    if(_session)_session->setPaused(true);
    panel.closed=^{HMGameView *view=weakSelf;if(view&&view->_session.get()==pausedSession&&view->_session)view->_session->setPaused(wasPaused||view->_resumeRequired);};
    panel.applyFixture=^(const heavymach::ValidationScenario& scenario){
        HMGameView *view=weakSelf;if(!view)return;
        view->_validationFixture=scenario;view->_loadError=nil;view->_loading=heavymach::LoadingTransition{};
        [view startSlot:0 mode:0 mission:scenario.mission stage:scenario.stage];[view persistSave];
    };
    panel.resumeSave=^{[weakSelf resumeValidationSave];};
    panel.addExperience=^{HMGameView *view=weakSelf;if(view&&view->_session){
        view->_session->validationAddExperience(1);[view consumeGameEvents];
        NSLog(@"HM_VALIDATION actual addExperience(1): exp=%d level=%d",view->_save.slots[view->_slotIndex].experience(),view->_save.slots[view->_slotIndex].level());
    }};
    [presenter presentViewController:panel animated:YES completion:nil];
}
- (void)resumeValidationSave {
    // Never applies fixture defaults here. Resume the validation bundle's own save.
    NSUserDefaults *defaults=NSUserDefaults.standardUserDefaults;
    int slot=int([defaults integerForKey:@"validation.resume.slot"]);
    if(slot<0||slot>=3||!_save.slots[slot].occupied()){
        UIAlertController *alert=[UIAlertController alertControllerWithTitle:@"저장 없음" message:@"초기 조건 재설정 후 시작으로 검증 저장을 먼저 만드세요." preferredStyle:UIAlertControllerStyleAlert];
        [alert addAction:[UIAlertAction actionWithTitle:@"확인" style:UIAlertActionStyleDefault handler:nil]];[self.window.rootViewController presentViewController:alert animated:YES completion:nil];return;
    }
    _validationFixture.reset();_loading=heavymach::LoadingTransition{};
    int mission=[defaults objectForKey:@"validation.resume.mission"]?int([defaults integerForKey:@"validation.resume.mission"]):_save.slots[slot].i32(0xc);
    int stage=[defaults objectForKey:@"validation.resume.stage"]?int([defaults integerForKey:@"validation.resume.stage"]):_save.slots[slot].i32(0x10);
    int mode=int([defaults integerForKey:@"validation.resume.mode"]);
    NSLog(@"HM_VALIDATION resume slot=%d mission=%d stage=%d exp=%d level=%d",slot,mission,stage,_save.slots[slot].experience(),_save.slots[slot].level());
    [self startSlot:slot mode:mode mission:mission stage:stage];
}
#endif
- (void)persistSave {
    if (_loadError) return;
    try {
#if defined(HM_VALIDATION) && HM_VALIDATION
        if(_session){auto stage=_session->stage();NSUserDefaults *defaults=NSUserDefaults.standardUserDefaults;
            [defaults setInteger:_slotIndex forKey:@"validation.resume.slot"];[defaults setInteger:stage.mission forKey:@"validation.resume.mission"];[defaults setInteger:stage.stage forKey:@"validation.resume.stage"];[defaults setInteger:_session->world().gameMode forKey:@"validation.resume.mode"];}
#endif
        auto saved=_save;
        if(_pendingSaveSnapshot)saved.slots.at(_slotIndex)=*_pendingSaveSnapshot;
        const auto bytes=saved.encode();
        NSError *error=nil;
        if (![[NSData dataWithBytes:bytes.data() length:bytes.size()] writeToURL:_saveURL options:NSDataWritingAtomic error:&error])
            NSLog(@"HeavyMach save failed: %@",error.localizedDescription);
    } catch (const std::exception& error) {NSLog(@"HeavyMach save rejected: %s",error.what());}
}
- (void)startSlot:(int)slot mode:(int)mode mission:(int)mission stage:(int)stage {
    try {
        if(_session){_retainedWorldTime=_session->world().gameTime;_retainedWorldCamera=_session->world().camera;}
        _slotIndex=slot;_originalSelectedSlot=&_save.slots.at(slot);_hasKeyboardAim=NO; _ammoPreview=-1; _gameMenu=NO; _hpGauge=1;_expGauge=0;_reloadGauge=0;
        // Original GameMain keeps GuiGame across screen and slot changes.
        // Its HP needle, gear and closing animations continue while hidden.
        HMMedia *media=_media;
        __weak HMGameView *randomOwner=self;
        _session=std::make_unique<heavymach::GameSession>(_data,_save.slots.at(slot),
            [media](std::string_view name) {
                NSString *key=[[NSString alloc] initWithBytes:name.data() length:name.size() encoding:NSUTF8StringEncoding];
                CGSize size=[media imageSize:key scope:@"shared"];
                return heavymach::Vector2{float(size.width),float(size.height)};
            }, [randomOwner]{return [randomOwner nextOriginalRandom];}, &_passives,
            [media](std::string_view name){NSString* key=[[NSString alloc] initWithBytes:name.data() length:name.size() encoding:NSUTF8StringEncoding];CGSize size=[media imageSize:key scope:@"world"];return heavymach::Vector2{float(size.width),float(size.height)};});
        // openMap must know the target extent before preparing a new world.
        _showMenu=NO;[self updateCanvasGeometry];
        _session->restoreWorldCamera(_retainedWorldCamera);
#if defined(HM_VALIDATION) && HM_VALIDATION
        if(_validationFixture){auto fixture=*_validationFixture;_validationFixture.reset();_session->startValidation(fixture);}
        else _session->start(mode,mission,stage,_retainedWorldTime);
#else
        _session->start(mode,mission,stage,_retainedWorldTime);
#endif
        _showMenu=NO;
        [_hud setGameStart:_time]; [self consumeGameEvents];
        // GameMain.setScreen_Game0x3a1c unpauses after setMission/setSurvival,
        // even when openMap's addExp(0) opens an upgrade from leftover EXP.
        _session->setPaused(false);[self updateCanvasGeometry];
        [self noteInteraction];if(_resumeRequired){[self pauseForInterruption];_session->prepareInitialPausedFrame();}
        [self becomeFirstResponder];
    } catch (const std::exception& error) {_loadError=[NSString stringWithUTF8String:error.what()];}
}
- (void)requestScreen:(int)screen {
    // Original GameMain accepts repeated requests even while shutters move.
    _loading.changeScreen(screen);
}
- (void)prepareScreenResources:(int)screen {
    _screenResourceScope=[NSString stringWithFormat:@"screen%lu",(unsigned long)++_screenResourceGeneration];
    NSMutableArray<NSString*> *names=[NSMutableArray new];
    if(screen==1)[names addObjectsFromArray:@[@"Main_BG",@"Main_Episode",@"Main_Title",@"Main_Copyright",@"Main_TapToStart"]];
    if(screen==2)[names addObjectsFromArray:@[@"SelMode_BG",@"Common_Btn_OK",@"SelMode_Btn_Scenario",@"SelMode_Btn_Survival"]];
    if(screen==3){
        [names addObjectsFromArray:@[@"SelSlot_BG",@"Common_Btn_OK",@"Common_Btn_DEL",@"SelSlot_Sel",@"SelSlot_DelSlot",@"Common_Btn_YES",@"Common_Btn_NO",@"NumSet01",@"SelSlot_Btn"]];
        BOOL occupied=NO,empty=NO;for(const auto& slot:_save.slots){occupied|=slot.occupied();empty|=!slot.occupied();}
        if(occupied)[names addObject:@"SelSlot_ExpGauge"];if(empty)[names addObject:@"SelSlot_Empty"];
    }
    if(screen==4){
        [names addObjectsFromArray:@[@"SelMission_BG",@"Common_Btn_OK",@"SelMission_Btn_Up",@"SelMission_Btn_Dn",@"SelMission_Slot",@"SelMission_Text",@"SelMission_Btn_Left",@"SelMission_Btn_Right"]];
        for(int i=1;i<=7;++i)[names addObject:[NSString stringWithFormat:@"SelMission_TileMission%02d",i]];
        for(int i=1;i<=6;++i)[names addObject:[NSString stringWithFormat:@"SelMission_Stage%02d",i]];
    }
    for(NSString *name in names)@autoreleasepool{[_media imageSize:[name stringByAppendingPathExtension:@"png"] scope:_screenResourceScope];}
    if(screen==1)[_media imageSize:@"Extensions/TitleWide.png" scope:_screenResourceScope];
    if(screen==2)[_media imageSize:@"Extensions/ModeWide.png" scope:_screenResourceScope];
    if(screen==3)[_media imageSize:@"Extensions/MenuMetal.png" scope:_screenResourceScope];
    if(screen==4)[_media imageSize:@"Extensions/MissionWide.png" scope:_screenResourceScope];
}
- (void)refreshScreen:(int)screen {
#if defined(HM_VALIDATION) && HM_VALIDATION
    [self stopValidationDirection];
#endif
    // Called synchronously at loading state3, before this frame's child ticks.
    heavymach::StagePosition stage{};BOOL fromGame=bool(_session);
    if(_session){_originalWeaponSlot=&_save.slots.at(_slotIndex);_originalSelectedWeapon=_session->selectedWeapon();_originalWeaponIndex=_originalSelectedWeapon;stage=_session->stage();_retainedWorldTime=_session->world().gameTime;_retainedWorldCamera=_session->world().camera;[_menus setPreviewWorldTime:_retainedWorldTime paused:_session->world().paused];}
    _session.reset();if(screen!=6)_resumeRequired=NO;[_media releaseScope:@"world"];
    NSString* oldScope=_screenResourceScope;[self prepareScreenResources:screen];
    switch(screen){
        case 1:_showMenu=NO;_titleShake={};_title=heavymach::TitleState{};_title.startTime=float(_time);break;
        case 2:_showMenu=YES;[_menus showMode];break;
        case 3:_showMenu=YES;[_menus showSlots];break;
        case 4:_showMenu=YES;
            if(fromGame)[_menus showMissions:stage.mission stage:stage.stage];else [_menus showMissions];
            break;
        case 6:[_menus startSelectedGame];break;
        default:NSLog(@"Unrestored screen ID %d",screen);break;
    }
    if(oldScope)[_media releaseScope:oldScope];
}
- (void)drawLoading {
    if(_canvasWidth>480&&!_session){
        CGFloat center=(_canvasWidth-480)/2;
        for(int side=-1;side<=1;++side){
            CGRect clip=side<0?CGRectMake(0,0,center,320):side>0?CGRectMake(center+480,0,center,320):CGRectMake(center,0,480,320);
            [_media setClipRect:clip];[_media setDrawingOffset:CGPointMake(center+side*480,0)];
            for(const auto& sprite:_loading.sprites())[_media draw:[NSString stringWithUTF8String:sprite.tile.c_str()] x:sprite.position.x y:sprite.position.y columns:sprite.cols rows:sprite.rows tile:sprite.frame scaleX:sprite.scale.x scaleY:sprite.scale.y rotation:sprite.rotation alpha:sprite.alpha blend:sprite.blend];
        }
        [_media resetClip];[_media setDrawingOffset:CGPointZero];
    }else for(const auto& sprite:_loading.sprites())
        [_media draw:[NSString stringWithUTF8String:sprite.tile.c_str()] x:sprite.position.x y:sprite.position.y columns:sprite.cols rows:sprite.rows tile:sprite.frame scaleX:sprite.scale.x scaleY:sprite.scale.y rotation:sprite.rotation alpha:sprite.alpha blend:sprite.blend];
}
- (void)finishUpgrade {
    if(!_session)return;
    _session->finishUpgradeSlide();
    if(_resumeRequired)[self pauseForInterruption];
}
- (double)interactionTime {return CACurrentMediaTime();}
- (void)noteInteraction {_lastInteraction=[self interactionTime];}
- (void)cancelGameInput {
#if defined(HM_VALIDATION) && HM_VALIDATION
    [self stopValidationDirection];
#endif
    _leftPressed=_rightPressed=_upPressed=_downPressed=_keyboardFire=NO;_hasKeyboardAim=NO;
    [_heldTouches removeAllObjects];[_worldTouches removeAllObjects];[_hudTouches removeAllObjects];[_hud cancelInput];
    if(_session)_session->releaseFire();
    else if(_showMenu)[_menus touch:CGPointMake(-1000,-1000) state:3];
    _tilt={};
}
- (void)pauseForInterruption {
    [self cancelGameInput];
    if(!_session)return;
    _resumeRequired=YES;
    const auto* player=_session->player();
    // Keep result/upgrade panels; never cover them with an ordinary pause menu.
    const auto& world=_session->world();
    if((world.gameState==0 && (!player||player->upgradeStep==0)) ||
       (world.gameState!=0 && world.presentation.step>=3)){
        _gameMenu=YES;[_hud setMenuVisible:YES];
    }
    _session->setPaused(true);
}
- (void)checkInactivity {
    if(!_active||!_session||_session->world().paused||_session->world().gameState!=0||_loading.visible())return;
    // Foreground inactivity is elapsed real time, not a catch-up game tick.
    if(_heldTouches.count||_keyboardFire||_leftPressed||_rightPressed||_upPressed||_downPressed||std::fabs(_tilt.y)>.03f){[self noteInteraction];return;}
    if([self interactionTime]-_lastInteraction>=120.0)[self pauseForInterruption];
}
- (void)gameButton:(int)identifier state:(int)state {
    if (!_active || !_session) return;
    [self noteInteraction];
    const auto& world=_session->world();
    if (world.presentation.step==2 && (identifier==1000 || identifier==1001)) {
        if (state==2) {_resumeRequired=NO;_session->resultButton(identifier);}return;
    }
    if (identifier==1000 && state==0) {
        if(!world.paused){_session->booster();[_media sound:@"UI_Click"];}
        return;
    }
    if (state!=2) return;
    // GuiGame.handleButton 0x17c4c keeps buttons active and gates actions here.
    if(world.paused && (identifier==100 || identifier==1100))return;
    if(!world.paused && ((identifier>=101&&identifier<=103) ||
                         (identifier>=1200&&identifier<=1202) || identifier==1300))return;
    switch (identifier) {
        case 100:
#if defined(HM_VALIDATION) && HM_VALIDATION
            [self stopValidationDirection];
#endif
            [self cancelGameInput];_gameMenu=YES;[_hud setMenuVisible:YES];_session->setPaused(true);[_media sound:@"UI_Click"];break;
        case 101: _resumeRequired=NO;_tilt={};[self noteInteraction];_gameMenu=NO;[_hud setMenuVisible:NO];_session->setPaused(world.gameState!=0);[_media sound:@"UI_Click"];break;
        case 102: _resumeRequired=NO;_session->setPaused(false);_gameMenu=NO;[_hud setMenuVisible:NO];[_media sound:@"UI_Click"];[self persistSave];[self requestScreen:3];break;
        case 103:
            if(world.paused){
                // 0x17c4c: original CFString bytes at file offset0x24964.
                [_media sound:@"UI_Click"];
                [UIApplication.sharedApplication openURL:[NSURL URLWithString:@"http://www.indiean.com"] options:@{} completionHandler:^(BOOL success){
                    if(!success)NSLog(@"Original website URL could not be opened");
                }];
            }break;
        case 1100: _session->cycleWeapon();[_media sound:@"Game_ChangeWeapon"];break; // 0x17c4c: release, then sfxID4.
        case 1200:case 1201:case 1202: _session->selectUpgrade(identifier-1200);break;
        case 1300: _resumeRequired=NO;_session->confirmUpgrade();break;
    }
    [self consumeGameEvents];
}
- (void)consumeGameEvents {
    if (!_session) return;
    using namespace heavymach;
    static NSString *const sounds[]={@"UI_Click",@"UI_Btn_OK",@"UI_Btn_Cancel",@"Game_GetItem",@"Game_ChangeWeapon",@"Game_WeaponEnd",@"Game_LevelUp",@"Game_LowHP",@"SFX_HitBody01",@"SFX_HitBody02",@"SFX_HitBody03",@"SFX_HitBody04",@"SFX_GroundHit",@"SFX_ExplGround",@"SFX_ExplMach"};
    _originalWeaponSlot=&_save.slots.at(_slotIndex);_originalSelectedWeapon=_session->selectedWeapon();_originalWeaponIndex=_originalSelectedWeapon;
    const auto events=_session->takeEvents();
    for (const auto& e:events) {
        if (e.kind==SessionEventKind::AudioIndex && e.index>=0 && e.index<15) [_media sound:sounds[e.index] pan:e.pan];
        else if (e.kind==SessionEventKind::AudioName) [_media sound:[NSString stringWithUTF8String:e.text.c_str()] pan:e.pan];
        else if (e.kind==SessionEventKind::PersistSave) {
            _pendingSaveSnapshot=e.savedSlot;[self persistSave];_pendingSaveSnapshot.reset();
        }
        else if (e.kind==SessionEventKind::CombatUI) {
            switch (CombatEventKind(e.code)) {
                case CombatEventKind::ReloadGauge:_reloadGauge=e.value;[_hud setReloadGauge:e.value];break;
                case CombatEventKind::ReloadComplete:_ammoPreview=-1;[_hud setReloadComplete:_time];break;
                case CombatEventKind::ChargePreview:_ammoPreview=int(e.value);break;
                default:break;
            }
        } else if (e.kind==SessionEventKind::PlayerUI) {
            switch (PlayerEventKind(e.code)) {
                case PlayerEventKind::Quake:_worldShake.setQuake(e.value,e.amount,e.delay,_time);break;
                case PlayerEventKind::HPGauge:_hpGauge=e.value;[_hud setHPGauge:e.value];break;
                case PlayerEventKind::UpgradeOpen:[_hud openUpgrade:_save.slots.at(_slotIndex)];break;
                case PlayerEventKind::UpgradeClose:[_hud closeUpgrade];break;
                case PlayerEventKind::ExperienceGauge:_expGauge=e.value;break;
                case PlayerEventKind::WeaponInventory:_ammoPreview=-1;[_hud showWeaponChange:_time];break;
                default:break;
            }
        } else if (e.kind==SessionEventKind::WorldUI) {
            switch (WorldEventKind(e.code)) {
                case WorldEventKind::Quake:_worldShake.setQuake(10,4,.3f,_time);break;
                case WorldEventKind::GameStart:[_hud setGameStart:_time];break;
                case WorldEventKind::ClearTileResources:[_media releaseScope:@"world"];break;
                case WorldEventKind::ChangeScreen:
                    if (_session) [self requestScreen:e.index];break;
                default:break;
            }
        } else if (e.kind==SessionEventKind::Unsupported) NSLog(@"Unrestored behavior: %s",e.text.c_str());
    }
}
- (BOOL)canBecomeFirstResponder {return YES;}
- (void)didMoveToWindow {[super didMoveToWindow];if(self.window)[self becomeFirstResponder];}
- (void)pressesBegan:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event {
    if(!_active)return;[self noteInteraction];
    BOOL handled=NO;
    for (UIPress *press in presses) {
#if defined(HM_VALIDATION) && HM_VALIDATION
        if ((press.key.keyCode==UIKeyboardHIDUsageKeyboardLeftShift || press.key.keyCode==UIKeyboardHIDUsageKeyboardRightShift || press.key.keyCode==UIKeyboardHIDUsageKeyboardF) && _session) {
            _keyboardFire=YES;handled=YES;
            if (auto p=_session->player()) _session->touch(_hasKeyboardAim?heavymach::Vector2{float(_keyboardAim.x),float(_keyboardAim.y)}:heavymach::Vector2{float(_canvasWidth-10),p->position.y-_session->world().camera.y+185},0);
        }
#endif
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardLeftArrow) {_leftPressed=YES;handled=YES;}
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardRightArrow) {_rightPressed=YES;handled=YES;}
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardUpArrow) {_upPressed=YES;handled=YES;}
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardDownArrow) {_downPressed=YES;handled=YES;}
#if defined(HM_VALIDATION) && HM_VALIDATION
        if(press.key.keyCode>=UIKeyboardHIDUsageKeyboardRightArrow && press.key.keyCode<=UIKeyboardHIDUsageKeyboardUpArrow)[self updateValidationKeyboardDirection];
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardSpacebar && _session) {_session->booster();handled=YES;}
#endif
    }
    if(!handled)[super pressesBegan:presses withEvent:event];
}
- (void)pressesEnded:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event {
    for (UIPress *press in presses) {
#if defined(HM_VALIDATION) && HM_VALIDATION
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardLeftShift || press.key.keyCode==UIKeyboardHIDUsageKeyboardRightShift || press.key.keyCode==UIKeyboardHIDUsageKeyboardF) {
            _keyboardFire=NO;if(_session)_session->releaseFire();
        }
#endif
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardLeftArrow)_leftPressed=NO;
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardRightArrow)_rightPressed=NO;
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardUpArrow)_upPressed=NO;
        if (press.key.keyCode==UIKeyboardHIDUsageKeyboardDownArrow)_downPressed=NO;
#if defined(HM_VALIDATION) && HM_VALIDATION
        if(press.key.keyCode>=UIKeyboardHIDUsageKeyboardRightArrow && press.key.keyCode<=UIKeyboardHIDUsageKeyboardUpArrow)[self updateValidationKeyboardDirection];
#endif
    }
    [super pressesEnded:presses withEvent:event];
}
- (void)pressesCancelled:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event {
#if defined(HM_VALIDATION) && HM_VALIDATION
    [self stopValidationDirection];
#endif
    _leftPressed=NO;_rightPressed=NO;_upPressed=NO;_downPressed=NO;_keyboardFire=NO;if(_session)_session->releaseFire();[super pressesCancelled:presses withEvent:event];
}
- (void)scheduleOriginalMotionTimer:(double)due {
    _nextMotionDue=due;
    __weak HMGameView *owner=self;
    _motionTimer=[NSTimer timerWithTimeInterval:heavymach::TiltState::interval repeats:NO block:^(NSTimer *timer) {
        HMGameView *view=owner;if(!view||!view->_active)return;
        constexpr double interval=heavymach::TiltState::interval;
        double now=CACurrentMediaTime();
        double next=view->_nextMotionDue+interval*std::ceil(std::max((now-view->_nextMotionDue)/interval,1.0));
        [view sampleOriginalAcceleration];
        if(view->_active)[view scheduleOriginalMotionTimer:next];
    }];
    _motionTimer.fireDate=[NSDate dateWithTimeIntervalSinceNow:std::max(due-CACurrentMediaTime(),0.0)];
    [NSRunLoop.mainRunLoop addTimer:_motionTimer forMode:NSDefaultRunLoopMode];
}
- (void)scheduleOriginalTimer:(double)due {
    _nextTimerDue=due;
    __weak HMGameView *owner=self;
    _frameTimer=[NSTimer timerWithTimeInterval:0.01666666753590107 repeats:NO block:^(NSTimer *timer) {
        HMGameView *view=owner;if(!view||!view->_active)return;
        // ns_timer.rs handle_timer advances BEFORE the callback, with ceil
        // (not the native repeating timer's skipped-fire rescheduling).
        constexpr double interval=0.01666666753590107;
        double now=CACurrentMediaTime();
        double next=view->_nextTimerDue+interval*std::ceil(std::max((now-view->_nextTimerDue)/interval,1.0));
        [view frame:timer];
        if(view->_active)[view scheduleOriginalTimer:next];
    }];
    _frameTimer.fireDate=[NSDate dateWithTimeIntervalSinceNow:std::max(due-CACurrentMediaTime(),0.0)];
    [NSRunLoop.mainRunLoop addTimer:_frameTimer forMode:NSDefaultRunLoopMode];
}
- (int32_t)nextOriginalRandom {
    // touchHLE libc/stdlib.rs random(): shared xorshift32, not Darwin random().
    uint32_t state=std::max(_randomState,uint32_t(1));
    state^=state<<13;state^=state>>17;state^=state<<5;
    _randomState=state;return int32_t(state&0x7fffffff);
}
- (UIInterfaceOrientation)accelerationInterfaceOrientation {
    return self.window.windowScene.interfaceOrientation;
}
- (void)sampleOriginalAcceleration {
    if(!_active)return;
    CMAccelerometerData *sample=_motion.accelerometerData;
    CMAcceleration acceleration=sample?sample.acceleration:CMAcceleration{0,0,-1};
    // SDL CoreMotion stores m/s^2 in float, then touchHLE converts back to g.
    constexpr float gravity=9.80665f;
    float x=-float(-acceleration.x*gravity)/gravity;
    float y=-float(-acceleration.y*gravity)/gravity;
    float z=-float(-acceleration.z*gravity)/gravity;
    // CoreMotion axes stay attached to the portrait device body. The guest's
    // fixed landscape-left UI uses +Y toward screen right; our UI also rotates
    // to landscape-right, where those body X/Y axes point the opposite way.
    // Normalize real samples before filtering. Screen-relative manual inputs
    // below already use the guest axes and must not be flipped again.
    if([self accelerationInterfaceOrientation]==UIInterfaceOrientationLandscapeRight){x=-x;y=-y;}
#if defined(HM_VALIDATION) && HM_VALIDATION
    // Same path on physical iPhone and Simulator; feed the original filter.
    if(_validationControls.selectedSegmentIndex!=3 || _leftPressed||_rightPressed||_upPressed||_downPressed){
        x=float(int(_downPressed)-int(_upPressed))*.2f;
        y=(_leftPressed||_rightPressed)?float(int(_rightPressed)-int(_leftPressed))*.2f:
          (_validationControls.selectedSegmentIndex==3?0.f:float(_validationControls.selectedSegmentIndex-1)*.2f);
        z=-std::sqrt(1.f-x*x-y*y);
    } else if(!_motion.accelerometerAvailable){x=0;y=0;z=-1;}
#elif TARGET_OS_SIMULATOR
    if(_leftPressed||_rightPressed||_upPressed||_downPressed){
        x=float(int(_downPressed)-int(_upPressed))*.2f;
        y=float(int(_rightPressed)-int(_leftPressed))*.2f;
        z=-std::sqrt(1.f-x*x-y*y);
    } else if(!_motion.accelerometerAvailable){x=0;y=0;z=-1;}
#endif
    _tilt.sample(x,y,z);
}
- (void)dealloc {[_frameTimer invalidate];[_motionTimer invalidate];[_motion stopAccelerometerUpdates];}
- (void)setApplicationActive:(BOOL)active {
    // Resign-active happens before background. The duplicate background call
    // must not issue GL commands after the app has already become inactive.
    if(_active==active)return;
    if(!active){
        if(_loading.visible())_resumeRequired=YES;
        [self pauseForInterruption];
        [_frameTimer invalidate];_frameTimer=nil;[_motionTimer invalidate];_motionTimer=nil;
        [_motion stopAccelerometerUpdates];[_media.audio setApplicationActive:NO];
        [_glRenderer finish];_active=NO;
    }else{
        _active=YES;_tilt={};[self noteInteraction];[_media.audio setApplicationActive:YES];
        if(_motion.accelerometerAvailable)[_motion startAccelerometerUpdates];
        [self scheduleOriginalTimer:CACurrentMediaTime()+0.01666666753590107];
        [self scheduleOriginalMotionTimer:CACurrentMediaTime()+heavymach::TiltState::interval];
        // Pause/menu state is intentionally retained until an explicit action.
    }
}
- (void)setWideCanvasEnabled:(BOOL)enabled {
    if(_wideCanvasEnabled==enabled)return;
    [self cancelGameInput];_wideCanvasEnabled=enabled;[self updateCanvasGeometry];
}
- (void)updateCanvasGeometry {
    CGRect bounds=self.bounds;
    const int dw=int(std::lround(bounds.size.width*self.contentScaleFactor));
    const int dh=int(std::lround(bounds.size.height*self.contentScaleFactor));
    auto canvas=heavymach::CanvasGeometry::forDrawable(dw,dh,_wideCanvasEnabled);
    // A paused scene keeps its cached draw commands; apply mode changes before
    // the next simulation tick, never by collecting stateful visuals twice.
    if(_session&&_session->world().paused)canvas.width=float(_canvasWidth);
    _canvasWidth=canvas.width;
    _viewport=heavymach::Viewport::drawable(dw,dh,self.contentScaleFactor,_canvasWidth);
    _viewport.x+=bounds.origin.x;_viewport.y+=bounds.origin.y;
    _hudScale=1;_hudOffset=CGPointMake((_canvasWidth-480)/2,0);
    _menuSafeRect=CGRectMake(0,0,_canvasWidth,320);
    if(_canvasWidth>480 && _viewport.scale>0){
        CGRect safe=UIEdgeInsetsInsetRect(bounds,self.safeAreaInsets);
        const CGFloat left=std::max(CGFloat(0),(CGRectGetMinX(safe)-_viewport.x)/_viewport.scale);
        const CGFloat right=std::min(_canvasWidth,(CGRectGetMaxX(safe)-_viewport.x)/_viewport.scale);
        const CGFloat bottom=std::max(CGFloat(0),320-(CGRectGetMaxY(safe)-_viewport.y)/_viewport.verticalScale());
        const CGFloat top=std::min(CGFloat(320),320-(CGRectGetMinY(safe)-_viewport.y)/_viewport.verticalScale());
        _menuSafeRect=CGRectMake(left,bottom,right-left,top-bottom);
        _hudScale=std::max(CGFloat(.01),std::min({CGFloat(1),(right-left)/480,(top-bottom)/320}));
        _hudOffset=CGPointMake((left+right-480*_hudScale)/2,(bottom+top-320*_hudScale)/2);
    }
    [_menus setCanvasWidth:_canvasWidth safeRect:_menuSafeRect];
#if defined(HM_VALIDATION) && HM_VALIDATION
    _validationControls.hidden=!_session||_showMenu;
#endif
    if(_session&&_session->canvasWidth()!=float(_canvasWidth))_session->setCanvasWidth(float(_canvasWidth));
}
- (BOOL)worldInputBlocked {
    if(!_session)return YES;
    return _loading.visible()||_gameMenu||_session->world().paused||_session->world().gameState!=0;
}
- (void)layoutSubviews {
    [super layoutSubviews];
    [self updateCanvasGeometry];
#if defined(HM_VALIDATION) && HM_VALIDATION
    CGRect safe=UIEdgeInsetsInsetRect(self.bounds,self.safeAreaInsets);
    _validationButton.frame=CGRectMake(CGRectGetMaxX(safe)-72,CGRectGetMinY(safe)+8,64,36);
    if(_viewport.x-CGRectGetMinX(safe)>=68)
        _validationControls.frame=CGRectMake(_viewport.x-68,CGRectGetMidY(safe)-94,60,188);
    else _validationControls.frame=CGRectMake(CGRectGetMidX(safe)-104,CGRectGetMinY(safe)+8,208,44);
#endif
    // Original EAGLView::layoutSubviews (0x27dc) only resizes its surface.
    // Drawing here would advance stateful menu weapon visuals outside the timer.
}
// Original IPA renderScene @0x21e0 increments this exact double once, then
// ticks and draws once. A wall-time accumulator can alternate zero/two ticks
// on adjacent display callbacks and visibly stutter even at a steady 60 Hz.
- (void)advanceOriginalFrame {
    _time+=0.01666666753590107; // Original constant 0x3f91111120000000.
    if(_session)_worldShake.tick(_time);
    else if(!_showMenu)_titleShake.tick(_time);
    if (_loadError) return;
    _loading.tick([self](heavymach::LoadingEvent event){
        switch(event.kind){
            case heavymach::LoadingEventKind::CloseSound:[self->_media sound:@"Loading_Close"];break;
            case heavymach::LoadingEventKind::OpenSound:[self->_media sound:@"Loading_Open"];break;
            case heavymach::LoadingEventKind::RefreshScreen:[self refreshScreen:event.screen];break;
        }
    });
    if (_loadError) return;
    if (_showMenu) {[_menus tick:_time];_passives.tick();[_hud tick:_time];return;}
    if (_session) {
        // Pause menus and interrupted upgrades must also freeze passive timers.
        if(_gameMenu||(_resumeRequired&&_session->world().gameState==0)){[_hud tick:_time];return;}
        try {
            float input=_tilt.y;
            // Samples have already been mapped to the original landscape axes.
            _session->setCameraOffset(_worldShake.offset);
            _session->tick(_time,input);[self consumeGameEvents];[_hud tick:_time];
            if (_trace && _session && int(_time)!=_traceSecond) {
                _traceSecond=int(_time);const auto *p=_session->player();auto stage=_session->stage();
                NSLog(@"HM_TRACE t=%d mission=%d stage=%d x=%.2f y=%.2f hp=%d exp=%d level=%d enemies=%zu bullets=%zu gameState=%d paused=%d fireKey=%d",_traceSecond,stage.mission,stage.stage,p?p->position.x:0,p?p->position.y:0,p?p->hp:0,_save.slots[_slotIndex].experience(),_save.slots[_slotIndex].level(),_session->enemyCount(),_session->bulletCount(),_session->world().gameState,_session->world().paused,_keyboardFire);
            }
        } catch (const std::exception& error) {_loadError=[NSString stringWithUTF8String:error.what()];}
        return;
    }
    _passives.tick();[_hud tick:_time];
    for (auto event:_title.tick(_time,[self]{return uint32_t([self nextOriginalRandom]);})) {
        switch (event) {
            case heavymach::TitleEvent::titleIn: [_media sound:@"Title_TitleIn"]; break;
            case heavymach::TitleEvent::bangTitle:_titleShake.setQuake(10,4,.3f,_time);[_media sound:@"Title_Bang"];break;
            case heavymach::TitleEvent::bangEpisode:_titleShake.setQuake(6,4,.3f,_time);[_media sound:@"Title_Bang"];break;
            case heavymach::TitleEvent::tapSound: [_media sound:@"Title_TapToStart"]; break;
        }
    }
}
- (void)frame:(id)sender {
    (void)sender;
    if (!_active) return;
    [self updateCanvasGeometry];[self checkInactivity];
    [self advanceOriginalFrame];
    [self renderOpenGLFrame];
}
// Production submits directly to CAEAGLLayer. Readback is never used to present.
- (void)renderOpenGLFrame {
    if (!_active || !self.window) return;
    _renderError.frame=CGRectInset(self.bounds,24,24);
    _renderError.text=_loadError ?: (_glRenderer?nil:@"OpenGL ES renderer unavailable");
    _renderError.hidden=(_renderError.text==nil);
    if (!_glRenderer || _loadError) return;
    if(![_glRenderer setLogicalWidth:_canvasWidth height:320]){_renderError.text=@"Game canvas allocation failed";_renderError.hidden=NO;return;}
    if (![_glRenderer beginFrame]) { _renderError.text=@"OpenGL ES context unavailable";_renderError.hidden=NO;return; }
    _media.glRenderer=_glRenderer;
    @try {
        [self drawLogicalFrame];
        if(![_glRenderer presentToLayer:(CAEAGLLayer*)self.layer viewport:CGRectMake(_viewport.x,_viewport.y,_canvasWidth*_viewport.scale,320*_viewport.verticalScale())]) {
            _renderError.text=@"OpenGL ES presentation failed";_renderError.hidden=NO;
        }
    } @finally { _media.glRenderer=nil;[_glRenderer endFrame]; }
}
// The CPU path is kept as a comparison oracle for existing offscreen tests.
- (void)drawRect:(CGRect)rect {
    if (self.window) return; // The original timer exclusively owns onscreen drawing.
    CGContextRef ctx=UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(ctx,UIColor.blackColor.CGColor); CGContextFillRect(ctx,self.bounds);
    // Expanded diagnostics use the same GPU scene. CPU raster remains a 480 oracle.
    if(_canvasWidth>480){
        if([_glRenderer setLogicalWidth:_canvasWidth height:320]&&[_glRenderer beginFrame]){
            _media.glRenderer=_glRenderer;[self drawLogicalFrame];_media.glRenderer=nil;[_glRenderer endFrame];
            [[_glRenderer snapshot]drawInRect:CGRectMake(_viewport.x,_viewport.y,_canvasWidth*_viewport.scale,320*_viewport.verticalScale())];
        }
        return;
    }
    // Compose once at the original framebuffer resolution. Scaling individual
    // translucent sprites before blending changes their overlapping pixels.
    UIImage *frame=[_logicalRenderer imageWithActions:^(UIGraphicsImageRendererContext *rendererContext) {
        [self drawLogicalFrame];
    }];
    CGContextSaveGState(ctx);CGContextSetInterpolationQuality(ctx,kCGInterpolationNone);
    [frame drawInRect:CGRectMake(_viewport.x,_viewport.y,480*_viewport.scale,320*_viewport.verticalScale())];
    CGContextRestoreGState(ctx);
}
- (void)drawLogicalFrame {
    CGContextRef ctx=_media.glRenderer?nullptr:UIGraphicsGetCurrentContext();
    if(ctx){CGContextSetFillColorWithColor(ctx,UIColor.blackColor.CGColor);CGContextFillRect(ctx,CGRectMake(0,0,480,320));}
    [_media setCameraOffset:CGPointZero];[_media setDrawingOffset:CGPointZero];[_media setDrawingScale:1];[_media resetClip];_media.resourceScope=@"shared";
    if(ctx){CGContextSaveGState(ctx);CGContextClipToRect(ctx,CGRectMake(0,0,480,320));}
    if (_loadError) {
        if(ctx)[_loadError drawInRect:CGRectMake(24,100,432,180) withAttributes:@{NSForegroundColorAttributeName:UIColor.whiteColor,NSFontAttributeName:[UIFont systemFontOfSize:14]}];
        if(ctx)CGContextRestoreGState(ctx); return;
    }
    if (_showMenu) {_media.resourceScope=_screenResourceScope;[_menus draw];_media.resourceScope=@"shared";[self drawLoading];if(ctx)CGContextRestoreGState(ctx);return;}
    if (_session) {
        [_media setCameraOffset:CGPointMake(_worldShake.offset.x,_worldShake.offset.y)];
        if(!(_canvasWidth>480&&_loading.visible()))for (const auto& item:_session->renderItems()) {
            if (const auto *sprite=std::get_if<heavymach::DrawSprite>(&item)){
                _media.resourceScope=sprite->worldTexture?@"world":@"shared";
                [_media draw:[NSString stringWithUTF8String:sprite->tile.c_str()] x:sprite->position.x y:sprite->position.y columns:sprite->cols rows:sprite->rows tile:sprite->frame scaleX:sprite->scale.x scaleY:sprite->scale.y rotation:sprite->rotation alpha:sprite->alpha blend:sprite->blend];}
            else {_media.resourceScope=@"shared";[_media drawTrail:std::get<heavymach::TrailMesh>(item)];}
        }
        [_media setCameraOffset:CGPointZero];_media.resourceScope=@"shared";
        [_media setDrawingOffset:_hudOffset];[_media setDrawingScale:_hudScale];
        [_media setClipRect:CGRectMake(_hudOffset.x,_hudOffset.y,480*_hudScale,320*_hudScale)];
        HMGameHUDSnapshot snapshot;
        snapshot.canvasWidth=_canvasWidth;snapshot.hudOffset=_hudOffset;snapshot.hudScale=_hudScale;
        snapshot.slot=&_save.slots[_slotIndex];snapshot.data=&_data;snapshot.passives=&_passives;
        snapshot.hpGauge=_hpGauge;snapshot.expGauge=_expGauge;snapshot.reloadGauge=_reloadGauge;
        snapshot.selectedWeapon=_session->selectedWeapon();
        if(snapshot.selectedWeapon>=0 && snapshot.selectedWeapon<snapshot.slot->weaponCount()) {
            auto name=snapshot.slot->weaponName(snapshot.selectedWeapon);
            auto found=_data.weapons.find(name);if(found!=_data.weapons.end())snapshot.weapon=&found->second;
            snapshot.ammo=_ammoPreview>=0?_ammoPreview:snapshot.slot->weaponCharge(snapshot.selectedWeapon);
        }
        const auto& world=_session->world();
        snapshot.presentation=&world.presentation;snapshot.paused=world.paused;snapshot.mode=world.gameMode;
        auto stage=_session->stage();snapshot.mission=stage.mission;snapshot.stage=stage.stage;
        snapshot.menuVisible=_gameMenu;snapshot.time=_time;
        if(auto player=_session->player()) {
            snapshot.upgradeVisible=player->upgradeStep==1;
            snapshot.upgradeSelection=player->upgradeSelection;
        }
        [_hud draw:snapshot];[self drawLoading];[_media resetClip];[_media setDrawingOffset:CGPointZero];[_media setDrawingScale:1];if(ctx)CGContextRestoreGState(ctx);return;
    }
    _media.resourceScope=_screenResourceScope;
    if(_canvasWidth>480) {
        CGSize size=[_media imageSize:@"Extensions/TitleWide.png"];
        // Draw only the two OUTER quarters of the generated candidate. Its center
        // never enters the frame; the original owns that moving rectangle.
        [_media setCameraOffset:CGPointMake(_titleShake.offset.x,_titleShake.offset.y)];
        CGFloat extensionScale=320/size.height;
        CGFloat distance=240+size.width/8*extensionScale;
        for(int side:{-1,1}) {
            CGFloat dx=side*distance*_title.bgScale;
            [_media draw:@"Extensions/TitleWide.png" x:_canvasWidth/2+_title.camX+std::cos(_title.bgRotate)*dx y:160+_title.camY+std::sin(_title.bgRotate)*dx columns:4 rows:1 tile:side<0?0:3 scaleX:extensionScale*_title.bgScale scaleY:extensionScale*_title.bgScale rotation:_title.bgRotate alpha:1];
        }
        [_media setDrawingOffset:CGPointMake((_canvasWidth-480)/2,0)];
        [_media drawEdgeContinuation:@"Main_BG.png" x:240+_title.camX y:160+_title.camY scale:_title.bgScale rotation:_title.bgRotate width:20];
    }
    [_media setCameraOffset:CGPointMake(_titleShake.offset.x,_titleShake.offset.y)];
    if(_canvasWidth>480)[_media drawJoinedBackground:@"Main_BG.png" x:240+_title.camX y:160+_title.camY scale:_title.bgScale rotation:_title.bgRotate];
    else [_media draw:@"Main_BG.png" x:240+_title.camX y:160+_title.camY columns:1 rows:1 tile:0 scaleX:_title.bgScale scaleY:_title.bgScale rotation:_title.bgRotate alpha:1];
    if(_canvasWidth>480)[_media setDrawingOffset:CGPointMake(CGRectGetMidX(_menuSafeRect)-240,0)];
    if (_title.showTitle) [_media draw:@"Main_Title.png" x:240 y:80 columns:1 rows:1 tile:0 scaleX:_title.titleScale scaleY:_title.titleScale rotation:0 alpha:1];
    if (_title.showEpisode) [_media draw:@"Main_Episode.png" x:240 y:290 columns:1 rows:1 tile:0 scaleX:_title.episodeScale scaleY:_title.episodeScale rotation:0 alpha:1];
    [_media draw:@"Main_Copyright.png" x:240 y:_canvasWidth>480?std::max(CGFloat(12),CGRectGetMinY(_menuSafeRect)+8):12];
    if (_title.showStart) [_media draw:@"Main_TapToStart.png" x:240 y:140];
    [_media setCameraOffset:CGPointZero];
    [_media setDrawingOffset:CGPointZero];
    _media.resourceScope=@"shared";[self drawLoading];
    if(ctx)CGContextRestoreGState(ctx);
}
- (void)routeTouches:(NSSet<UITouch *> *)touches state:(int)state {
    if (!_active || _loadError) return;
    for (UITouch *touch in touches) {
        CGPoint point=[touch locationInView:self];
        auto logical=_viewport.logical(point.x,point.y);
        if(state>=2)[_heldTouches removeObject:touch];
        if(logical){[self noteInteraction];if(state==0)[_heldTouches addObject:touch];}
        if(_session&&_canvasWidth>480){
            const CGPoint p=logical?CGPointMake(logical->first,logical->second):CGPointMake(-1000,-1000);
            const CGPoint hp=CGPointMake((p.x-_hudOffset.x)/_hudScale,(p.y-_hudOffset.y)/_hudScale);
            const BOOL insideHUD=hp.x>=0&&hp.x<=480&&hp.y>=0&&hp.y<=320;
            if(state==0){
                if(insideHUD&&[_hud touch:hp state:0])[_hudTouches addObject:touch];
                else if(logical&&![self worldInputBlocked]){[_worldTouches addObject:touch];_session->touch({float(p.x),float(p.y)},0);_keyboardAim=p;_hasKeyboardAim=YES;}
            }else if([_hudTouches containsObject:touch]){
                [_hud touch:insideHUD?hp:CGPointMake(-1000,-1000) state:state];
                if(state>=2)[_hudTouches removeObject:touch];
            }else if([_worldTouches containsObject:touch]){
                if(state>=2){
                    [_worldTouches removeObject:touch];
                    if(!_worldTouches.count){
                        if(state==2&&logical)_session->touch({float(p.x),float(p.y)},2);
                        else _session->releaseFire();
                    }
                }
                else if(logical&&![self worldInputBlocked]){_session->touch({float(p.x),float(p.y)},state);_keyboardAim=p;_hasKeyboardAim=YES;}
            }
            [self consumeGameEvents];continue;
        }
        if (_session) {
            const CGPoint p=logical?CGPointMake(logical->first,logical->second):CGPointMake(-1000,-1000);
            // QobManager @0xfc18 dispatches each event in reverse receiver order.
            // There is no per-touch ownership; GobWorld @0xd92c receives only
            // events that no visible HUD button consumed.
            if(![_hud touch:p state:state]) {
                if(logical){_keyboardAim=p;_hasKeyboardAim=YES;}
                if(state>=2&&(state==3||!logical))_session->releaseFire();
                else _session->touch({float(p.x),float(p.y)},state>=2?2:state);
            }
            [self consumeGameEvents];continue;
        }
        if (!logical) {if (_showMenu && state>=2) [_menus touch:CGPointMake(-1000,-1000) state:3];continue;}
        if (_showMenu) {[_menus touch:CGPointMake(logical->first,logical->second) state:state];continue;}
        if (state==2 && _title.released(_time)) [self requestScreen:3];
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {[self routeTouches:touches state:0];}
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {[self routeTouches:touches state:1];}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {[self routeTouches:touches state:2];}
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {[self routeTouches:touches state:3];}
@end
