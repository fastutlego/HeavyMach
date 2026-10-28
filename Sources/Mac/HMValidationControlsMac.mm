#import "HMValidationControls.h"
#if HM_VALIDATION && TARGET_OS_OSX
@implementation HMValidationControls { NSSegmentedControl *_segments; }
- (instancetype)init {if((self=[super initWithFrame:NSZeroRect])){_segments=[NSSegmentedControl segmentedControlWithLabels:@[@"←",@"정지",@"→",@"키보드"] trackingMode:NSSegmentSwitchTrackingSelectOne target:self action:@selector(changed:)];[self addSubview:_segments];self.selectedSegmentIndex=3;}return self;}
- (NSView*)hitTest:(NSPoint)point {return self.hudEditing?nil:[super hitTest:point];}
- (NSInteger)numberOfSegments{return 4;}
- (void)setSelectedSegmentIndex:(NSInteger)value {if(value<0||value>3)return;_selectedSegmentIndex=value;_segments.selectedSegment=value;}
- (void)changed:(id)sender {_selectedSegmentIndex=_segments.selectedSegment;[self sendAction:self.action to:self.target];}
- (void)layout {[super layout];_segments.frame=self.bounds;}
@end
#endif
