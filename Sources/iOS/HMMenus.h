#import <UIKit/UIKit.h>
#include "../Core/SaveData.hpp"
#include "../Core/GameData.hpp"
@class HMMedia;
@interface HMMenus : NSObject
- (instancetype)initWithMedia:(HMMedia *)media save:(heavymach::SaveData *)save data:(const heavymach::GameData *)data;
// Original hidden World survives screen changes; previews read its clock/pause.
- (void)setPreviewWorldTime:(double)time paused:(BOOL)paused;
- (void)showSlots;
- (void)showMode;
- (void)showMissions;
// Called only at LoadingTransition RefreshScreen for screen6.
- (void)startSelectedGame;
- (void)showMissions:(int)mission stage:(int)stage;
- (void)draw;
- (void)tick:(double)time;
// Logical 480x320, y-up. state: 0 began, 1 moved, 2 ended, 3 cancelled.
- (void)touch:(CGPoint)logical state:(int)state;
@property(nonatomic,copy) int32_t(^random31)(void);
@property(nonatomic,copy) void(^changeScreen)(int screen);
@property(nonatomic,copy) void(^startGame)(int slot,int mode,int mission,int stage);
@property(nonatomic,copy) void(^saveChanged)(void);
// GobHvM_Player.init 0x1bb84 notifies the retained game HUD before adding parts.
@property(nonatomic,copy) void(^previewPlayerCreated)(void);
@property(nonatomic,copy) void(^slotSelected)(int slot);
@end
