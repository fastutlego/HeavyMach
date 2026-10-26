#pragma once
#if defined(HM_VALIDATION) && HM_VALIDATION
#import "HMPlatform.h"
#if TARGET_OS_OSX
#define HMPanelBase NSWindowController
#else
#define HMPanelBase UIViewController
#endif
#include "ValidationScenario.hpp"
@interface HMValidationPanel : HMPanelBase
- (instancetype)initWithData:(const heavymach::GameData *)data;
@property(nonatomic,copy) void(^applyFixture)(const heavymach::ValidationScenario&);
@property(nonatomic,copy) void(^resumeSave)(void);
@property(nonatomic,copy) void(^addExperience)(void);
@property(nonatomic,copy) void(^closed)(void);
@property(nonatomic) BOOL wideCanvas;
@property(nonatomic,copy) void(^canvasChanged)(BOOL enabled);
@end
#endif
