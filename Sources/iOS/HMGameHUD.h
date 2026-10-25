#import <UIKit/UIKit.h>
#include "../Core/Player.hpp"
#include "../Core/World.hpp"
@class HMMedia;
struct HMGameHUDSnapshot {
    const heavymach::SaveSlot *slot=nullptr;
    const heavymach::GameData *data=nullptr;
    const heavymach::WeaponInfo *weapon=nullptr;
    const heavymach::PlayerPassives *passives=nullptr;
    const heavymach::StagePresentation *presentation=nullptr;
    float hpGauge=1,expGauge=0,reloadGauge=0;
    int ammo=99,mission=0,stage=0,mode=0,selectedWeapon=0;
    bool paused=false,upgradeVisible=false,menuVisible=false;
    int upgradeSelection=0;
    double time=0;
};
@interface HMGameHUD : NSObject
- (instancetype)initWithMedia:(HMMedia *)media;
- (void)draw:(const HMGameHUDSnapshot&)snapshot;
- (void)tick:(double)time;
- (void)setGameStart:(double)time;
- (void)setReloadComplete:(double)time;
- (void)showWeaponChange:(double)time;
// Retained GlobalInfo pointers may be nil or reference a different slot than
// the slot currently being previewed. Returns the original charge-label update.
- (NSNumber *)changeWeaponForSlot:(heavymach::SaveSlot *)slot
                  selectedWeapon:(int)selectedWeapon
                selectedItemSlot:(heavymach::SaveSlot *)itemSlot
               selectedItemIndex:(int)itemIndex time:(double)time;
- (void)setHPGauge:(float)value;
- (void)setReloadGauge:(float)value;
- (void)openUpgrade:(const heavymach::SaveSlot&)slot;
- (void)closeUpgrade;
- (void)setMenuVisible:(BOOL)visible;
- (void)cancelInput;
// Logical y-up. state0 began,1 moved,2 ended,3 cancelled. YES consumes touch.
- (BOOL)touch:(CGPoint)logical state:(int)state;
@property(nonatomic,copy) void(^button)(int originalID,int state);
// Original GuiGame unpauses after the upgrade panel moves past x659.
@property(nonatomic,copy) void(^upgradeClosed)(void);
@end
