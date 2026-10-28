#pragma once
#if defined(HM_VALIDATION) && HM_VALIDATION
#import "HMPlatform.h"
#if TARGET_OS_OSX
#define HMControlBase NSControl
#else
#define HMControlBase UIControl
#endif

// Kept separate from the game surface so a second screen can host controls.
@interface HMValidationControls : HMControlBase
@property(nonatomic) BOOL hudEditing;
@property(nonatomic) NSInteger selectedSegmentIndex;
@property(nonatomic,readonly) NSInteger numberOfSegments;
@end
#endif
