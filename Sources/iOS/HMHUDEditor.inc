// Included by HMGameView: editor UI uses the game's renderer and coordinate model.
- (heavymach::hud::Configuration)currentHUDLayout {
    auto c=_hudEditing?_hudDraft:_hudLayout;c.legacy={{_hudOffset.x,_hudOffset.y},_hudScale};
    c.safe={_menuSafeRect.origin.x,_menuSafeRect.origin.y,_menuSafeRect.size.width,_menuSafeRect.size.height};
    if(!_hudEditing&&!heavymach::hud::valid(c,HM_VALIDATION))c.positions={};return c;
}
- (NSURL*)hudLayoutURL {
#if TARGET_OS_OSX
    NSString* profile=@"mac";
#else
    NSString* profile=UIDevice.currentDevice.userInterfaceIdiom==UIUserInterfaceIdiomPad?@"ipad":@"iphone";
#endif
    return [[_saveURL URLByDeletingLastPathComponent]URLByAppendingPathComponent:[NSString stringWithFormat:@"HUDLayoutV1-%@.json",profile]];
}
- (void)loadHUDLayout {
    NSData* data=[NSData dataWithContentsOfURL:[self hudLayoutURL]];if(!data)return;
    id object=[NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
    if(![object isKindOfClass:NSDictionary.class]||![object[@"version"] isEqual:@1]||![object[@"positions"] isKindOfClass:NSArray.class])return;
    NSArray* values=object[@"positions"];if(values.count!=heavymach::hud::groupCount)return;
    auto candidate=_hudLayout;candidate.positions={};
    for(int i=0;i<values.count;++i){id v=values[i];if(v==NSNull.null)continue;
        if(![v isKindOfClass:NSArray.class]||[v count]!=2||![v[0] isKindOfClass:NSNumber.class]||![v[1] isKindOfClass:NSNumber.class])return;
        double x=[v[0] doubleValue],y=[v[1] doubleValue];if(!std::isfinite(x)||!std::isfinite(y)||x<0||x>1||y<0||y>1)return;
        candidate.positions[i]=heavymach::hud::Placement{x,y};
    }
    _hudLayout=candidate;
}
- (HMEditorButton*)editorButton:(NSString*)title action:(SEL)action tag:(NSInteger)tag {
#if TARGET_OS_OSX
    HMEditorButton* b=[NSButton buttonWithTitle:title target:self action:action];b.bezelStyle=NSBezelStyleRounded;
#else
    HMEditorButton* b=[UIButton buttonWithType:UIButtonTypeSystem];[b setTitle:title forState:UIControlStateNormal];[b addTarget:self action:action forControlEvents:UIControlEventTouchUpInside];
    b.backgroundColor=[UIColor colorWithWhite:.13 alpha:.96];[b setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];b.layer.cornerRadius=5;b.titleLabel.font=[UIFont systemFontOfSize:13 weight:UIFontWeightSemibold];
#endif
    b.tag=tag;return b;
}
- (void)prepareHUDEditor {
    _hudSelected=heavymach::hud::jump;_hudShowHits=YES;
    _hudEntry=[self editorButton:@"HUD 배치" action:@selector(openHUDEditor:) tag:0];_hudEntry.hidden=YES;[self addSubview:_hudEntry];
    _hudToolbar=[[HMNativeGameView alloc]initWithFrame:CGRectZero];_hudToolbar.hidden=YES;[self addSubview:_hudToolbar];
#if TARGET_OS_OSX
    _hudEditorLabel=[NSTextField labelWithString:@""];_hudEditorLabel.textColor=NSColor.whiteColor;_hudEditorLabel.drawsBackground=YES;_hudEditorLabel.backgroundColor=NSColor.blackColor;
#else
    _hudEditorLabel=[UILabel new];_hudEditorLabel.textColor=UIColor.whiteColor;_hudEditorLabel.backgroundColor=[UIColor colorWithWhite:.05 alpha:.9];_hudEditorLabel.font=[UIFont systemFontOfSize:13];_hudEditorLabel.adjustsFontSizeToFitWidth=YES;
#endif
    [_hudToolbar addSubview:_hudEditorLabel];NSMutableArray* buttons=[NSMutableArray new];
    NSArray* titles=@[@"기본 배치",@"취소",@"적용",@"화면 확인",@"예시 전투",@"도구 위치",@"터치 영역"];
    for(int i=0;i<titles.count;++i){auto b=[self editorButton:titles[i] action:@selector(hudEditorAction:) tag:i];[_hudToolbar addSubview:b];[buttons addObject:b];}_hudEditorButtons=buttons;
}
- (void)updateHUDEntry {
    BOOL hidden=_hudEditing||_loading.visible()||_keyboardSettingsOpen||!(((_hudLayoutFallback&&!_gameMenu)&&_session&&_session->world().gameState==0)||(_showMenu&&[_menus isMissionSelection]));
    if(_hudEntry.hidden!=hidden)_hudEntry.hidden=hidden;
}
- (void)updateHUDEditorGeometry {
    BOOL changed=_hudDraft.safe.x!=_menuSafeRect.origin.x||_hudDraft.safe.y!=_menuSafeRect.origin.y||_hudDraft.safe.width!=_menuSafeRect.size.width||_hudDraft.safe.height!=_menuSafeRect.size.height||_hudDraft.legacy.scale!=_hudScale;
    // Editing must not change the live session's render cache on resize.
    if(_hudEditing&&(_hudDraft.safe.width!=_menuSafeRect.size.width||_hudDraft.safe.height!=_menuSafeRect.size.height)){
        _hudDragToken=nil;_hudExample=YES;_hudExampleSession.reset();
    }
    _hudLayout.legacy={{_hudOffset.x,_hudOffset.y},_hudScale};_hudLayout.safe={_menuSafeRect.origin.x,_menuSafeRect.origin.y,_menuSafeRect.size.width,_menuSafeRect.size.height};
#if HM_VALIDATION
    CGRect nsafe=[self safeContentRect];CGRect f=_viewport.x-CGRectGetMinX(nsafe)>=68?CGRectMake(_viewport.x-68,CGRectGetMidY(nsafe)-94,60,188):CGRectMake(CGRectGetMidX(nsafe)-104,CGRectGetMinY(nsafe)+8,208,44);
    auto origin=_hudLayout.legacy.inverse(heavymach::hud::Point{(f.origin.x-_viewport.x)/_viewport.scale,_canvasHeight-(CGRectGetMaxY(f)-_viewport.y)/_viewport.verticalScale()});
    _hudLayout.cheatBounds={origin.x,origin.y,f.size.width/_viewport.scale/_hudScale,f.size.height/_viewport.verticalScale()/_hudScale};_hudDraft.cheatBounds=_hudLayout.cheatBounds;
#endif
    _hudLayoutFallback=!heavymach::hud::valid(_hudLayout,HM_VALIDATION);
    _hudDraft.legacy=_hudLayout.legacy;_hudDraft.safe=_hudLayout.safe;
    if(_hudEditing&&changed)[self layoutHUDControls];
}
- (CGRect)hudNativeRect:(heavymach::hud::Rect)r {
    return CGRectMake(_viewport.x+r.x*_viewport.scale,_viewport.y+(_canvasHeight-r.y-r.height)*_viewport.verticalScale(),r.width*_viewport.scale,r.height*_viewport.verticalScale());
}
- (void)layoutHUDControls {
    CGRect safe=[self safeContentRect];CGFloat w=std::min(CGFloat(840),safe.size.width-16),h=76;
    _hudEntry.frame=CGRectMake(CGRectGetMaxX(safe)-112,CGRectGetMidY(safe)-20,104,40);
    _hudToolbar.frame=CGRectMake(CGRectGetMidX(safe)-w/2,_hudToolsBottom?CGRectGetMaxY(safe)-h-8:CGRectGetMinY(safe)+8,w,h);
    _hudEditorLabel.frame=CGRectMake(0,0,w,28);
    for(int i=0;i<_hudEditorButtons.count;++i)_hudEditorButtons[i].frame=CGRectMake(i*w/7,32,w/7-3,40);
#if defined(HM_VALIDATION) && HM_VALIDATION
    auto c=[self currentHUDLayout];
    if(!c.positions[heavymach::hud::cheat]){
        if(_viewport.x-CGRectGetMinX(safe)>=68)_validationControls.frame=CGRectMake(_viewport.x-68,CGRectGetMidY(safe)-94,60,188);
        else _validationControls.frame=CGRectMake(CGRectGetMidX(safe)-104,CGRectGetMinY(safe)+8,208,44);
    }
    else _validationControls.frame=[self hudNativeRect:c.group(heavymach::hud::cheat).visualBounds];
    if(_hudEditing&&CGRectIntersectsRect(_hudToolbar.frame,_validationControls.frame)){
        CGRect bar=_hudToolbar.frame;
        CGFloat below=CGRectGetMaxY(_validationControls.frame)+4;
        CGFloat above=CGRectGetMinY(_validationControls.frame)-h-4;
        if(!_hudToolsBottom&&below+h<=CGRectGetMaxY(safe))bar.origin.y=below;
        else if(above>=CGRectGetMinY(safe))bar.origin.y=above;
        _hudToolbar.frame=bar;
    }
    _validationButton.hidden=_hudEditing;
    _validationControls.hudEditing=_hudEditing;
    _validationControls.hidden=_hudEditing?NO:(!_session||_showMenu);
#endif
    [self updateHUDEntry];if(_hudEditing)[self refreshHUDEditorLabel:nil];
}
- (void)refreshHUDEditorLabel:(NSString*)error {
    NSArray* names=@[@"메뉴",@"레벨·경험치",@"점프",@"무기·탄약",@"계기판",@"자동조준 조작부",@"패시브",@"무기 선택 표시",@"치트 방향키"];
    NSString* label=error?:[NSString stringWithFormat:@"HUD 편집 · %@ · %@ — 끌어서 이동 / Tab 선택 / 방향키 이동",_hudExample?@"예시 전투":@"전투 정지",names[_hudSelected]];
#if TARGET_OS_OSX
    _hudEditorLabel.stringValue=label;
#else
    _hudEditorLabel.text=label;
#endif
}
- (void)openHUDEditor:(id)sender {
    if(_loading.visible()||_hudEditing||!((_session&&(_gameMenu||_hudLayoutFallback)&&_session->world().gameState==0)||(_showMenu&&[_menus isMissionSelection])))return;
    if(_session&&!_gameMenu){_gameMenu=YES;[_hud setMenuVisible:YES];}
    [self cancelGameInput];_hudFromMenu=_showMenu;_hudExample=_hudFromMenu;_hudDraft=_hudLayout;_hudEditing=YES;_hudHideGuides=NO;_hudDragToken=nil;
    _hudToolbar.hidden=NO;
#if HM_VALIDATION
    [self stopValidationDirection];
#endif
    if(_session)_session->setPaused(true);
    [self layoutHUDControls];
#if TARGET_OS_OSX
    [self.window makeFirstResponder:self];
#else
    [self becomeFirstResponder];
#endif
}
- (void)closeHUDEditor {
    _hudEditing=NO;_hudToolbar.hidden=YES;_hudDragToken=nil;_hudExampleSession.reset();_hudExampleData={};[_media releaseScope:@"hud-example"];
    [self cancelGameInput];[self noteInteraction];
#if TARGET_OS_OSX
    [self layoutGameSurface];
#else
    [self setNeedsLayout];[self layoutIfNeeded];
#endif
    // Leave the mission selector / pause menu exactly where editing began.
}
- (void)hudEditorAction:(HMEditorButton*)sender {
    if(!_hudEditing)return;
    switch(sender.tag){
      case 0:_hudDraft.positions={};_hudDragToken=nil;break;
      case 1:[self closeHUDEditor];return;
      case 2:{auto c=[self currentHUDLayout];
        if(!heavymach::hud::valid(c,HM_VALIDATION)){[self refreshHUDEditorLabel:@"버튼이 겹치거나 화면에 맞지 않습니다. 위치를 옮겨 주세요."];return;}
        if(!_hudDraft.samePositions(_hudLayout)){
            NSMutableArray* values=[NSMutableArray new];for(auto p:_hudDraft.positions)[values addObject:p?(id)@[@(p->x),@(p->y)]:(id)NSNull.null];
            NSError* error=nil;NSData* bytes=[NSJSONSerialization dataWithJSONObject:@{@"version":@1,@"positions":values} options:0 error:&error];
            if(!bytes||![bytes writeToURL:[self hudLayoutURL] options:NSDataWritingAtomic error:&error]){[self refreshHUDEditorLabel:@"배치를 저장하지 못했습니다. 적용을 다시 눌러 주세요."];return;}
            ++_hudLayoutWrites;_hudLayout=_hudDraft;
        }
        [self closeHUDEditor];return;
      }
      case 3:_hudHideGuides=YES;_hudToolbar.hidden=YES;return;
      case 4:if(!_hudFromMenu)_hudExample=!_hudExample;break;
      case 5:_hudToolsBottom=!_hudToolsBottom;break;
      case 6:_hudShowHits=!_hudShowHits;break;
    }
    [self layoutHUDControls];
}
- (BOOL)hudEditorKey:(unsigned)code pressed:(BOOL)pressed {
    if(!pressed)return YES;
    if(_hudHideGuides){_hudHideGuides=NO;_hudToolbar.hidden=NO;return YES;}
    if(code==0x29){[self closeHUDEditor];return YES;}
    if(code==0x2b){_hudSelected=(_hudSelected+1)%(HM_VALIDATION?9:8);[self refreshHUDEditorLabel:nil];return YES;}
    auto c=[self currentHUDLayout];auto r=c.group(_hudSelected).visualBounds;double dx=code==0x4f?1:code==0x50?-1:0,dy=code==0x52?1:code==0x51?-1:0;
    if(dx||dy){_hudDraft.positions[_hudSelected]=heavymach::hud::placementForOrigin(c.localBounds(_hudSelected),c.legacy,c.safe,{r.x+dx,r.y+dy});[self layoutHUDControls];}
    return YES;
}
- (void)hudEditorPoint:(CGPoint)p state:(int)state token:(id)token {
    if(_hudHideGuides){if(state==2){_hudHideGuides=NO;_hudToolbar.hidden=NO;}return;}
    auto c=[self currentHUDLayout];
    if(state==0&&!_hudDragToken){
        int hit=-1;
        for(int i=(HM_VALIDATION?8:7);i>=0;--i){auto r=c.group(i).visualBounds;

            if(r.contains({p.x,p.y},heavymach::hud::HitEdges::standard)){hit=i;_hudGrab=CGPointMake(p.x-r.x,p.y-r.y);break;}
        }
        if(hit<0){hit=_hudSelected;auto r=c.group(hit).visualBounds;_hudGrab=CGPointMake(r.width/2,r.height/2);}
        _hudSelected=hit;_hudDragToken=token;[self refreshHUDEditorLabel:nil];
    }
    if(token!=_hudDragToken)return;
    if(state<3&&p.x>=0&&p.y>=0){
        auto r=c.group(_hudSelected).visualBounds;double x=p.x-_hudGrab.x,y=p.y-_hudGrab.y;
        if(std::abs(x-r.x)>.01||std::abs(y-r.y)>.01){
            auto original=c.legacy.apply(c.localBounds(_hudSelected));
            if(std::abs(x-original.x)<.01&&std::abs(y-original.y)<.01)_hudDraft.positions[_hudSelected].reset();
            else _hudDraft.positions[_hudSelected]=heavymach::hud::placementForOrigin(c.localBounds(_hudSelected),c.legacy,c.safe,{x,y});
        }
    }
    if(state>=2)_hudDragToken=nil;
    [self layoutHUDControls];
}
- (void)prepareHUDExample {
    if(_hudExampleSession)return;
    _hudExampleData=_data;_hudExampleSlot={};heavymach::GlobalInfo(_hudExampleSlot).selectSlot(1);_hudExampleRandom=12345;
    auto& map=_hudExampleData.maps.at("Stage01");map.enemies={{"Tank001",{map.left+240,100},false},{"Heli001",{map.left+330,190},false}};
    HMMedia* media=_media;__weak HMGameView* owner=self;
    _hudExampleSession=std::make_unique<heavymach::GameSession>(_hudExampleData,_hudExampleSlot,[media](std::string_view name){NSString* key=[[NSString alloc]initWithBytes:name.data() length:name.size() encoding:NSUTF8StringEncoding];CGSize z=[media imageSize:key scope:@"hud-example"];return heavymach::Vector2{float(z.width),float(z.height)};},[owner]{HMGameView* v=owner;if(!v)return int32_t(0);uint32_t x=v->_hudExampleRandom;x^=x<<13;x^=x>>17;x^=x<<5;v->_hudExampleRandom=x;return int32_t(x&0x7fffffff);});
    _hudExampleSession->setCanvasWidth(_canvasWidth);_hudExampleSession->setCanvasHeight(_canvasHeight);_hudExampleSession->start(0,0,0);
    for(int i=1;i<=45;++i){_hudExampleSession->setPaused(false);_hudExampleSession->tick(i/60.,0);_hudExampleSession->takeEvents();}
    _hudExampleSession->prepareHUDPreview();
}
- (void)drawHUDEditor {
    if(_hudExample){[self prepareHUDExample];}
    auto* scene=_hudExample?_hudExampleSession.get():_session.get();if(!scene)return;
    [_media setDrawingOffset:CGPointZero];[_media setDrawingScale:1];[_media resetClip];
    if(!_hudExample)[_media setCameraOffset:CGPointMake(_worldShake.offset.x,_worldShake.offset.y)];
    for(const auto& item:scene->renderItems()){
        if(auto sprite=std::get_if<heavymach::DrawSprite>(&item)){
            _media.resourceScope=_hudExample?@"hud-example":sprite->worldTexture?@"world":@"shared";
            [_media draw:[NSString stringWithUTF8String:sprite->tile.c_str()] x:sprite->position.x y:sprite->position.y columns:sprite->cols rows:sprite->rows tile:sprite->frame scaleX:sprite->scale.x scaleY:sprite->scale.y rotation:sprite->rotation alpha:sprite->alpha blend:sprite->blend];
        }else { _media.resourceScope=_hudExample?@"hud-example":@"shared";[_media drawTrail:std::get<heavymach::TrailMesh>(item)];}
    }
    _media.resourceScope=@"shared";[_media setCameraOffset:CGPointZero];[_media setDrawingOffset:_hudOffset];[_media setDrawingScale:_hudScale];
    [_media setClipRect:CGRectMake(_hudOffset.x,_hudOffset.y,480*_hudScale,320*_hudScale)];
    HMGameHUDSnapshot s;s.slot=_hudExample?&_hudExampleSlot:&_save.slots[_slotIndex];s.data=&_data;s.passives=_hudExample?&_hudExamplePassives:&_passives;s.time=_time;s.ammo=99;s.expGauge=_expGauge;s.reloadGauge=_reloadGauge;
    s.canvasWidth=_canvasWidth;s.canvasHeight=_canvasHeight;s.hudOffset=_hudOffset;s.hudScale=_hudScale;s.selectedWeapon=scene->selectedWeapon();
    if(s.slot->weaponCount()>0){auto w=_data.weapons.find(s.slot->weaponName(s.selectedWeapon));if(w!=_data.weapons.end())s.weapon=&w->second;s.ammo=s.slot->weaponCharge(s.selectedWeapon);}
    [_hud setLayout:[self currentHUDLayout] editing:YES];[_hud draw:s];[self drawAimControls];
    [_media resetClip];[_media setDrawingOffset:CGPointZero];[_media setDrawingScale:1];
    if(!_hudHideGuides){auto c=[self currentHUDLayout];auto r=c.group(_hudSelected).visualBounds;[_glRenderer drawInputOutline:CGRectMake(r.x,r.y,r.width,r.height)];
        if(_hudShowHits)for(size_t i=0;i<heavymach::hud::hits.size();++i){auto q=c.group(heavymach::hud::hitGroups[i]).hitRect(heavymach::hud::hits[i]);float lines[]={float(q.x),float(q.y),float(q.x+q.width),float(q.y),float(q.x+q.width),float(q.y),float(q.x+q.width),float(q.y+q.height),float(q.x+q.width),float(q.y+q.height),float(q.x),float(q.y+q.height),float(q.x),float(q.y+q.height),float(q.x),float(q.y)};[_glRenderer drawAimLines:lines count:8 red:.1 green:.9 blue:.9];}
    }
}
