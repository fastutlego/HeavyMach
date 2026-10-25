#import "HMValidationControls.h"
#if defined(HM_VALIDATION) && HM_VALIDATION
@implementation HMValidationControls {
    NSArray<UIButton*> *_buttons;
}
- (instancetype)init {
    if ((self=[super initWithFrame:CGRectZero])) {
        self.accessibilityIdentifier=@"HMValidation.Direction";
        self.isAccessibilityElement=NO;
        NSArray* titles=@[@"←",@"정지",@"→",@"기울기"];
        NSArray* labels=@[@"왼쪽 이동",@"이동 정지",@"오른쪽 이동",@"기울기로 이동"];
        NSMutableArray* buttons=[NSMutableArray new];
        for(NSInteger i=0;i<4;++i){
            UIButton* button=[UIButton buttonWithType:UIButtonTypeSystem];button.tag=i;
            [button setTitle:titles[i] forState:UIControlStateNormal];button.accessibilityLabel=labels[i];
            button.accessibilityIdentifier=[NSString stringWithFormat:@"HMValidation.Direction.%ld",(long)i];
            button.layer.cornerRadius=8;
            [button addTarget:self action:@selector(chooseDirection:) forControlEvents:UIControlEventTouchUpInside];
            [self addSubview:button];[buttons addObject:button];
        }
        _buttons=buttons;self.selectedSegmentIndex=3;
    }
    return self;
}
- (NSInteger)numberOfSegments {return _buttons.count;}
- (void)setSelectedSegmentIndex:(NSInteger)index {
    if(index<0||index>=4)return;
    _selectedSegmentIndex=index;
    for(UIButton* button in _buttons){
        BOOL chosen=button.tag==index;
        button.backgroundColor=chosen?UIColor.systemBlueColor:[UIColor.systemBackgroundColor colorWithAlphaComponent:.95];
        [button setTitleColor:chosen?UIColor.whiteColor:UIColor.labelColor forState:UIControlStateNormal];
        button.accessibilityTraits=UIAccessibilityTraitButton|(chosen?UIAccessibilityTraitSelected:0);
    }
}
- (void)chooseDirection:(UIButton*)sender {self.selectedSegmentIndex=sender.tag;[self sendActionsForControlEvents:UIControlEventValueChanged];}
- (void)layoutSubviews {
    [super layoutSubviews];BOOL vertical=self.bounds.size.height>self.bounds.size.width;
    CGFloat extent=((vertical?self.bounds.size.height:self.bounds.size.width)-12)/4;
    for(UIButton* button in _buttons)button.frame=vertical?CGRectMake(0,button.tag*(extent+4),self.bounds.size.width,extent):CGRectMake(button.tag*(extent+4),0,extent,self.bounds.size.height);
}
@end
#endif
