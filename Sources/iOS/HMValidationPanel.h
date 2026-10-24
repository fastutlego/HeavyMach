#pragma once
#if defined(HM_VALIDATION) && HM_VALIDATION
#import <UIKit/UIKit.h>
#include "ValidationScenario.hpp"
@interface HMValidationPanel : UIViewController
- (instancetype)initWithData:(const heavymach::GameData *)data;
@property(nonatomic,copy) void(^applyFixture)(const heavymach::ValidationScenario&);
@property(nonatomic,copy) void(^resumeSave)(void);
@property(nonatomic,copy) void(^addExperience)(void);
@property(nonatomic,copy) void(^closed)(void);
@end
#endif
