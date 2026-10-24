#import "HMValidationPanel.h"
#if defined(HM_VALIDATION) && HM_VALIDATION
#include "GlobalInfo.hpp"
#include <algorithm>
@implementation HMValidationPanel {
    const heavymach::GameData *_data;
    std::vector<heavymach::ValidationStage> _stages;
    UIStepper *_mission,*_stage,*_level,*_boss;
    UILabel *_missionText,*_stageText,*_levelText,*_bossText;
    UISwitch *_near,*_all;
    UISegmentedControl *_grades[3];
    UIButton *_weapon;
    NSString *_weaponName;
    UIStackView *_stack;
}
- (instancetype)initWithData:(const heavymach::GameData *)data {
    if((self=[super init])){_data=data;_stages=heavymach::validationStages(*data);self.modalPresentationStyle=UIModalPresentationFullScreen;}
    return self;
}
- (UILabel *)label:(NSString *)text {
    UILabel *label=[UILabel new];label.text=text;label.numberOfLines=0;label.font=[UIFont systemFontOfSize:14];return label;
}
- (void)addRow:(NSString *)title control:(UIView *)control label:(UILabel *__strong *)value {
    UILabel *label=[self label:title];if(value)*value=label;
    UIStackView *row=[[UIStackView alloc] initWithArrangedSubviews:@[label,control]];row.axis=UILayoutConstraintAxisHorizontal;row.spacing=12;row.alignment=UIStackViewAlignmentCenter;
    [label setContentHuggingPriority:UILayoutPriorityDefaultLow forAxis:UILayoutConstraintAxisHorizontal];[_stack addArrangedSubview:row];
}
- (UIStepper *)stepper:(double)minimum maximum:(double)maximum value:(double)value {
    UIStepper *s=[UIStepper new];s.minimumValue=minimum;s.maximumValue=maximum;s.value=value;
    [s addTarget:self action:@selector(updateFields) forControlEvents:UIControlEventValueChanged];return s;
}
- (UIButton *)action:(NSString *)title selector:(SEL)selector {
    UIButton *b=[UIButton buttonWithType:UIButtonTypeSystem];[b setTitle:title forState:UIControlStateNormal];b.accessibilityIdentifier=[@"HMValidation." stringByAppendingString:NSStringFromSelector(selector)];
    [b addTarget:self action:selector forControlEvents:UIControlEventTouchUpInside];[_stack addArrangedSubview:b];return b;
}
- (void)viewDidLoad {
    [super viewDidLoad];self.view.backgroundColor=UIColor.systemBackgroundColor;
    UIScrollView *scroll=[UIScrollView new];scroll.translatesAutoresizingMaskIntoConstraints=NO;[self.view addSubview:scroll];
    _stack=[UIStackView new];_stack.axis=UILayoutConstraintAxisVertical;_stack.spacing=10;_stack.translatesAutoresizingMaskIntoConstraints=NO;[scroll addSubview:_stack];
    [NSLayoutConstraint activateConstraints:@[[scroll.leadingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.leadingAnchor constant:20],[scroll.trailingAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.trailingAnchor constant:-20],[scroll.topAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor],[scroll.bottomAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.bottomAnchor],[_stack.leadingAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.leadingAnchor],[_stack.trailingAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.trailingAnchor],[_stack.topAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.topAnchor constant:12],[_stack.bottomAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.bottomAnchor constant:-12],[_stack.widthAnchor constraintEqualToAnchor:scroll.frameLayoutGuide.widthAnchor]]];
    [_stack addArrangedSubview:[self label:@"HM Validation · 별도 저장\n초기 조건 재설정은 검증 슬롯1을 교체합니다. 이어하기는 저장된 경험치·장비·탄약을 유지합니다."]];
    NSUserDefaults *d=NSUserDefaults.standardUserDefaults;
    _mission=[self stepper:1 maximum:8 value:[d integerForKey:@"fixture.mission"]+1];[self addRow:@"미션" control:_mission label:&_missionText];
    _stage=[self stepper:1 maximum:6 value:[d integerForKey:@"fixture.stage"]+1];[self addRow:@"스테이지" control:_stage label:&_stageText];
    _level=[self stepper:1 maximum:100 value:[d integerForKey:@"fixture.level"]+1];[self addRow:@"레벨" control:_level label:&_levelText];
    NSArray *names=@[@"몸체 등급",@"상부 등급",@"무기 등급"];
    for(int i=0;i<3;++i){_grades[i]=[[UISegmentedControl alloc] initWithItems:@[@"0",@"1",@"2",@"3",@"4",@"5",@"6"]];_grades[i].selectedSegmentIndex=std::clamp<NSInteger>([d integerForKey:[NSString stringWithFormat:@"fixture.grade%d",i]],0,6);[self addRow:names[i] control:_grades[i] label:nullptr];}
    _near=[UISwitch new];_near.on=[d boolForKey:@"fixture.near"];[self addRow:@"다음 레벨까지 EXP 1 남기기" control:_near label:nullptr];
    _all=[UISwitch new];_all.on=[d boolForKey:@"fixture.all"];[self addRow:@"모든 원본 플레이어 무기 지급" control:_all label:nullptr];
    _weaponName=[d stringForKey:@"fixture.weapon"]?:@"P_Wpn_Gun01";_weapon=[self action:@"무기 선택" selector:@selector(chooseWeapon)];
    _boss=[self stepper:-1 maximum:100 value:[d objectForKey:@"fixture.boss"]?[d integerForKey:@"fixture.boss"]:-1];[self addRow:@"보스" control:_boss label:&_bossText];
    [self action:@"초기 조건 재설정 후 시작" selector:@selector(resetFixture)];
    [self action:@"저장 이어하기 (초기 조건 적용 안 함)" selector:@selector(resume)];
    [self action:@"현재 게임에 EXP 1 획득" selector:@selector(experience)];
    [self action:@"현재 화면으로 돌아가기" selector:@selector(close)];[self updateFields];
}
- (const heavymach::ValidationStage *)selectedStage {
    for(const auto& s:_stages)if(s.mission==int(_mission.value)-1&&s.stage==int(_stage.value)-1)return &s;return nullptr;
}
- (void)updateFields {
    int mission=int(_mission.value)-1;_stage.maximumValue=heavymach::GlobalInfo::stageCount(mission);
    _missionText.text=[NSString stringWithFormat:@"미션 %d%@",mission+1,mission==7?@" · 원본 데이터 직접 진입":@""];
    const auto *stage=[self selectedStage];_stageText.text=[NSString stringWithFormat:@"스테이지 %d · %s",int(_stage.value),stage?stage->mapName.c_str():"missing"];
    _levelText.text=[NSString stringWithFormat:@"레벨 %d (내부 %d)",int(_level.value),int(_level.value)-1];
    _boss.maximumValue=stage?double(stage->bosses.size())-1:-1;
    if(_boss.value>_boss.maximumValue)_boss.value=-1;
    _bossText.text=_boss.value<0?@"보스: 일반 시작":[NSString stringWithFormat:@"보스 %d · %s",int(_boss.value)+1,stage->bosses.at(size_t(_boss.value)).name.c_str()];
    [_weapon setTitle:[@"장착 무기: " stringByAppendingString:_weaponName] forState:UIControlStateNormal];
}
- (void)chooseWeapon {
    UIAlertController *alert=[UIAlertController alertControllerWithTitle:@"원본 무기 선택" message:nil preferredStyle:UIAlertControllerStyleAlert];
    for(const auto& name:heavymach::validationWeapons(*_data)){
        NSString *value=[NSString stringWithUTF8String:name.c_str()];
        [alert addAction:[UIAlertAction actionWithTitle:value style:UIAlertActionStyleDefault handler:^(UIAlertAction *){self->_weaponName=value;[self updateFields];}]];
    }
    [alert addAction:[UIAlertAction actionWithTitle:@"취소" style:UIAlertActionStyleCancel handler:nil]];[self presentViewController:alert animated:YES completion:nil];
}
- (void)finish:(void(^)(void))action {
    auto closed=self.closed;[self dismissViewControllerAnimated:YES completion:^{if(closed)closed();if(action)action();}];
}
- (void)resetFixture {
    heavymach::ValidationScenario s;s.mission=int(_mission.value)-1;s.stage=int(_stage.value)-1;s.internalLevel=int(_level.value)-1;
    for(int i=0;i<3;++i)s.grades[i]=uint8_t(_grades[i].selectedSegmentIndex);
    s.nearLevelUp=_near.on;s.allWeapons=_all.on;if(_boss.value>=0)s.bossIndex=size_t(_boss.value);
    std::vector<std::string> inventory={"P_Wpn_Gun01","P_Wpn_RaderBomb","P_Wpn_RaderMsl","P_Wpn_RaderLsr"};
    auto append=[&](const std::string& name){if(std::find(inventory.begin(),inventory.end(),name)==inventory.end())inventory.push_back(name);};
    if(s.allWeapons)for(const auto& name:heavymach::validationWeapons(*_data))append(name);
    s.weapons.emplace_back(_weaponName.UTF8String);append(s.weapons.front());
    s.selectedWeapon=int(std::find(inventory.begin(),inventory.end(),s.weapons.front())-inventory.begin());
    NSUserDefaults *d=NSUserDefaults.standardUserDefaults;[d setInteger:s.mission forKey:@"fixture.mission"];[d setInteger:s.stage forKey:@"fixture.stage"];[d setInteger:s.internalLevel forKey:@"fixture.level"];[d setBool:s.nearLevelUp forKey:@"fixture.near"];[d setBool:s.allWeapons forKey:@"fixture.all"];[d setObject:_weaponName forKey:@"fixture.weapon"];[d setInteger:s.bossIndex?NSInteger(*s.bossIndex):-1 forKey:@"fixture.boss"];
    for(int i=0;i<3;++i)[d setInteger:s.grades[i] forKey:[NSString stringWithFormat:@"fixture.grade%d",i]];
    NSLog(@"HM_VALIDATION fixture mission=%d stage=%d internalLevel=%d grades=%d/%d/%d near=%d allWeapons=%d selected=%s boss=%ld",s.mission,s.stage,s.internalLevel,s.grades[0],s.grades[1],s.grades[2],s.nearLevelUp,s.allWeapons,_weaponName.UTF8String,s.bossIndex?long(*s.bossIndex):-1L);
    auto callback=self.applyFixture;[self finish:^{if(callback)callback(s);}];
}
- (void)resume {auto callback=self.resumeSave;[self finish:callback];}
- (void)experience {auto callback=self.addExperience;[self finish:callback];}
- (void)close {[self finish:nil];}
@end
#endif
