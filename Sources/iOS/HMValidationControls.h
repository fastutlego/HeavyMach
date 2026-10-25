#pragma once
#if defined(HM_VALIDATION) && HM_VALIDATION
#import <UIKit/UIKit.h>

// Kept separate from the game surface so a second screen can host controls.
@interface HMValidationControls : UIControl
@property(nonatomic) NSInteger selectedSegmentIndex;
@property(nonatomic,readonly) NSInteger numberOfSegments;
@end
#endif
