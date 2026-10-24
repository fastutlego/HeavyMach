> 과거 구현·검사 기록이다. 현행 출력 경로는 [그래픽 정책](rendering-policy.md)과 [시간·저장·음향 정책](runtime-policy.md)을 따른다. 이 문서의 UIKit/AVAudioPlayer 설명을 새 작업의 구현 기준으로 사용하지 않는다.

# 원본 음원 초기 로드 시점 복원

2026-10-24. 첫 재생 때까지 AVAudioPlayer 생성을 미루던 HMMedia를 수정했다. 원본이 첫 타이틀 프레임 전에 요청하는31개 이름을 같은 첫등장 순서로 생성하고 prepareToPlay를 호출한다. 디렉토리의 모든WAV를 추정해 읽는 방식은 쓰지 않는다. 원본·음원 파일은 변경하지 않았다.

## 원본 요청과 시점

| 순서 | 원본 근거 | 처음 요청하는 이름 |
|---|---|---|
|1..15|GlobalInfo init0xfd44|UI_Click, UI_Btn_OK, UI_Btn_Cancel, Game_GetItem, Game_ChangeWeapon, Game_WeaponEnd, Game_LevelUp, Game_LowHP, SFX_HitBody01..04, SFX_GroundHit, SFX_ExplGround, SFX_ExplMach|
|16..26|GlobalInfo의openWeaponInfo→readWeaponChunk0x10e00; 원본WeaponInfo160개에서 첫등장 순서|Wpn_Gun01, Wpn_Gun02, Wpn_Gun03, Wpn_Shotgun, Wpn_Canon, Wpn_Missile, Wpn_Laser, Wpn_Default, Wpn_EnGun, Wpn_EnCanon, Wpn_EnMissile|
|27..28|GameMain init0x2ecc|Loading_Open, Loading_Close|
|29..31|GameMain setScreen_Title0x390c→GuiTitle init0xefbc|Title_TitleIn, Title_Bang, Title_TapToStart|

SoundManager getSound0x13c08은 이름이 없으면 즉시SoundEngine_LoadEffect를 호출하고ID를 캐시한다. LoadEffect0x1f2b8→AttachFilesToSource0x1e8b4는 WAV데이터를 읽어 AL_BUFFER를 연결한다. play를 처음 호출할 때까지 파일 로딩을 미루지 않는다. 같은 이름을 나중에getSound하는 로딩 화면의0xb8f0/0xbf74/0xc228은 기존ID를 재사용한다.

GameMain의maxDistance800 설정은GlobalInfo 로드 뒤, loading/title 요청 앞이다. 기존26종max800 및 후행5종기본값 계산은 유지했다. 이제AudioContract의원본 초기 목록을 재사용해 이름 집합의 중복 정의를 제거했다.

현재 HMGameView는 초기화 초기에 HMMedia를 한 번 만들며 타이틀tick/display 전에 완료한다. 따라서 HMMedia init에서 이31개를 준비한다. 원본의 객체별 생성 단계 사이에 걸리는 시간을 재현했다는 뜻은 아니며, 확인된 첫 프레임 전 로드 순서와 음원별단일객체를 보존한다.

## 현대 재생 API 대응

이전코드의무조건AVAudioPlayer.stop은첫재생직전에prepare를취소할수있다. 설치된공식SDK `AVFAudio.framework/Headers/AVAudioPlayer.h`의53행은pause가ready상태를유지한다고명시하고,56..58행은stop이play/prepare가획득한hardware를해제한다고명시한다.

원본 non-looping Start0x1e9e0는alSourceStop→alSourcePlay를호출하며AL_BUFFER를떼거나다시로드하지않는다. 대응코드는**같은AVAudioPlayer의pause→currentTime0→play**다. 같은음원은겹치지않고다시처음부터시작하며,다른이름은각각의player를유지한다. volume 거리감쇠와기존pan근사식은변경하지않았다.

`prepareOriginalSounds`는이미성공적으로준비한이름을재생성하거나중복prepare하지않는다. 파일누락·decode실패·prepare실패는로그에남기고반환값NO로표시한다. decode된player는이름당하나만보존한다. 아직준비되지않은player는후속준비요청에서다시prepare할수있다. 임의새이름의sound요청은기존호환경로처럼그이름만로드한다.

오디오route변화·interruption·자연종료뒤기기의내부ready상태와원본latency의수치적동등성까지이번초기준비검사로보증하지않는다.

## 실제 실패 재현

제품수정전에부모가AirSimulator의실제UIKit app-hosted XCTest를실행했다.

- `build/audio-preload-before.log` / `audio-preload-before.xcresult`: exit65.
- 원본31WAV존재와실제AVAudioPlayer initWithContentsOfURL decode검사는통과했다.
- init직후캐시31개검사는실제0개로실패했다. 원본순서및생성검사와반복준비검사도실패했다.
- 총3tests 중2tests가실패했고assertion failure35개를기록했다.
- 수정전test사본은`build/HMAudioTests-before.mm`에보존했다. macOS shim이나추출코드를실제HMMedia실행으로대체하지않았다.

## 새 검사 범위와 현재 결과

`Tests/iOS/HMAudioTests.mm`은원본주소와RawData에서따로옮긴31개기대목록을사용한다. 제품배열을그대로검사oracle로쓰지않는다.

1. 앱bundle의31개WAV존재·실제decode·mono채널·양수duration을검사한다.
2. HMMedia init직후31개voice생성순서와실제AVAudioPlayer subclass의super.prepareToPlay성공을검사한다. 재생은하지않는다.
3. 반복준비와동일음원2회요청에서객체identity31개·prepare1회·pause2회·stop0회·재생요청위치0·기존거리gain/pan을검사한다. 두 번째 요청 전 currentTime을 실제 양수로 설정하고 읽어 확인하므로 위치 초기화를 누락하면 실패한다. 각 재생 요청의 준비 상태도 따로 확인한다.

play만spy에서가로채실제스피커출력을막는다. URLdecode와prepareToPlay/pause는실제AVFoundation을호출한다. spy의readyflag는실제prepare성공및stop관측에따른검사계기이며privatehardware상태를읽었다고주장하지않는다. 샘플믹싱·실제출력nonoverlap·음질·채널곡선·지연시간은이검사범위에없다.

이담당자가수행한AudioContractTests ASan/UBSan은exit0이다. HMMedia의iOS Simulator ObjC++ syntax검사도exit0이며기존그래픽enumOR warning1개가남았다. 후속 실제 app-hosted XCTest7개(음향3+렌더4) 모두통과했고 일반iphoneos서명제외빌드도성공했다. 아래통합결과를따른다.

## 통합 실행과 독립 검토

- `build/audio-preload-after.log` / `audio-preload-after.xcresult`: 실제 Air iOS26.5 시뮬레이터에서7tests,0failures,TEST SUCCEEDED. 음향3개와 기존 UIKit 렌더4개를 포함한다.
- `build/audio-stage-ctest.log`: CTest15/15통과. 새StageTransition 계약과 기존AudioContract/게임코어검사를 포함한다.
- `build/audio-preload-device-unsigned-build.log`: 일반앱 iphoneos BUILD SUCCEEDED. 서명을제외한빌드이며물리기기에추가설치하지않았다.
- `build/audio-preload-evidence.json`: 위결과와소스·테스트·구성SHA256.

독립검토가원본15+11+2+3순서,WeaponInfo원시160레코드의음원첫등장순서,중복0,AL_BUFFER수명과SDK의pause/stop계약을확인했고필수수정결함을찾지못했다. 실패주입·실제스피커출력·반복재생지연·interruption은이번검사범위밖이다. 준비성공캐시는하드웨어의현재내부상태를증명하지않는다.

원본대응자산의보존도확인했다. WAV31개와파생이미지695개의SHA256이기존manifest와모두같다. `build/audio-preload-assets-check.json`에검사시점과불일치0개를기록했다.
