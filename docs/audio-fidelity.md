> 과거 구현·검사 기록이다. 현행 출력 경로는 [그래픽 정책](rendering-policy.md)과 [시간·저장·음향 정책](runtime-policy.md)을 따른다. 이 문서의 UIKit/AVAudioPlayer 설명을 새 작업의 구현 기준으로 사용하지 않는다.

# HeavyMach 1.13 음향 계약

2026-10-24. REA export와 현재 HMMedia를 대조했다. 원본 SHA는 Analysis/functions.json에 보존한다. UI·설치·실제 음향 캡처는 수행하지 않았다. 후속 승인에 따라 **이름당 단일 source 재시작과 거리 gain**을 복원했다. 후속으로 원본 초기 요청31개의 생성·준비 시점도 복원했다([검사 근거](audio-preload.md)). 스테레오 공간 배치의 완전한 일치는 남아 있다.

## 원본에서 확인한 계약

| 범위 | 주소와 관측 |
|---|---|
| 이름 캐시 | SoundManager::getSound 0x13c08: 이름→SoundID dictionary. 같은 이름은 기존 ID 반환 |
| source 생성 | OpenALObject::LoadEffect 0x1f2b8 → SoundEngineEffect ctor 0x1e6ea: effect마다 alGenSources(1), non-looping flag0. Attach 0x1e8b4: AL_BUFFER(0x1009)에 WAV buffer 연결 |
| 재생 | SoundManager::play 0x13cee: source 위치(0,0,0), StartEffect. play:X:Y: 0x13d0c assembly: soundID r2, X r3, Y stack를 SetEffectPosition(soundID,X,Y,0)로 전달. Pseudocode의 인자 타입은 잘못 복구됐으므로 assembly를 우선 |
| 같은 음원 반복 | StartEffect 0x1e094 → effect Start 0x1e9e0: alSourceStop 후 non-looping이면 alSourcePlay. 기존 소리를 끊고 처음부터 재생. 같은 이름 8개 동시 voice나 voice stealing pool은 없음 |
| listener | SoundManager init 0x13b1c에서 Initialize(22050), listener(0,0,0). setListnerPos 0x13bd0 → alListener3f(AL_POSITION). clearAllScreen 0x3778도 (0,0) 설정. 관측한 game caller에 이동 listener 없음 |
| 최대 거리 | GameMain init 0x2ecc: GlobalInfo 생성 후 setMaxDistance(800). 0x1f458은 그 순간 존재하는 source에만 AL_MAX_DISTANCE(0x1023) 설정. 미래 source용 값을 저장하지 않음 |
| source 수명 | SoundManager removeSound 0x13cec는 빈 함수. removeAllSounds 0x13cd0은 dictionary만 비움. source 파괴는 effect destructor 0x1e720의 alDeleteSources, SoundManager dealloc 0x13b90→engine teardown 경로. 화면 교체가 기존 effect들을 일괄 stop하는 코드 없음 |

source 개수의 게임 고유 상한은 발견하지 못했다. 실제 OpenAL 장치의 생성 가능 수는 별도 플랫폼 제한이다. 8개 같은 음원 중복 재생이라는 제한은 원본에 없다. SoundManager 캐시를 정상 경로에서 유지하면 사용한 이름마다 source 하나씩 유지한다.

### 최대 거리 로드 순서

GlobalInfo init 0xfd44는 UI/Game/SFX 공통 15개를 읽고, 말미 openWeaponInfo→readWeaponInfo 0x10e00에서 fireSFX 11종을 읽는다. 이 호출은 GameMain의 max800 설정보다 앞선다. 따라서 이 **26종은 max800**이다. 그 뒤 로드하는 Loading_Open/Close와 Title_TitleIn/Bang/TapToStart **5종은 OpenAL 기본 최대 거리**를 유지한다. 최초 중간 조사에서 weapon 로드 순서를 빠뜨렸으나 여기서는 전체 호출 순서를 반영했다.

현재 Resources/Audio의 31개 WAV 헤더를 읽었다. 모두 mono16bit이며 27개 22050Hz, 2개 44100Hz, 2개 48000Hz다. 자산별 내역은 Analysis/audio-contract-evidence.json에 있다. 실제 원본 샘플 변환·출력장치 mixer 응답의 일치까지 이 검사로 인증하지 않는다.

## 게임에서 전달하는 공간 값

다음 네 경로는 `sourceX=(음원 소유자 또는 충돌 x - player.x)/100`, Y=0을 전달한다.

- GobHvMach damage 0x54b8: 무작위 공통 SFX ID8..10.
- GobHvMach destroy 0x55fc: SFX ID14.
- GobBullet hit 0xa198: 폭발 SFX ID13, 해당 particle 분기에서 재생.
- GobMachWeapon fire 0x11e64: WeaponInfo.fireSFX.

listener와 Z가 0이므로 거리는 abs(sourceX)이다. Core의 `pan` 필드는 이 위치 값을 그대로 유지했지만 이전 HMMedia는 이를 단순 stereo pan으로 해석하여 [-1,1]로 제한하고 volume은1로 뒀다. 예를 들어 200px 떨어진 음원은 sourceX=2여서 원본 거리 감쇠가 발생하지만 기존 native에서는 최대 pan만 바뀌었다.

OpenAL 1.1의 기본 inverse-distance-clamped, reference distance1, rolloff1, source/listener gain1을 적용하면 `gain=1/clamp(abs(sourceX),1,maxDistance)`다. 기본 maxDistance는 MAX_FLOAT다. 실행 파일의 import와 모든 export를 검색해 distance model·reference distance·rolloff·gain을 바꾸는 호출은 찾지 못했다. 따라서 이 기본값 적용은 **명세와 정적 호출 조사에 근거한 복원**이며 구형 iPhone mixer 출력 캡처 결과는 아니다. [OpenAL 1.1 명세 §3.4, §4.3](https://www.openal.org/documentation/openal-1.1-specification.pdf)

## 현대 플랫폼 구현과 남은 차이

`AudioContract.hpp`는 26개 선행 로드 이름과 gain 계산을 보존한다. HMMedia는 이름당 AVAudioPlayer 하나를 캐시하고 pause→position gain/pan→currentTime0→play 순서로 재시작한다. AVAudioPlayer의stop은 준비한 hardware를 해제하지만 원본 alSourceStop은 연결한 buffer를 유지하므로, 준비 상태를 보존하는pause로 대응한다. 같은 이름의 재생을 끊고 위치0부터 다시 시작하는 계약은 유지한다. 다른 이름은 서로 겹칠 수 있다. 기존 이름당8개 pool과 8개 재생 중 추가 요청 무시는 제거했다. 일반 UI/타이틀 호출의 sourceX0은 volume1이다.

**stereo pan은 아직 근사다.** OpenAL source 위치는 pan ratio가 아니다. 구형 Apple OpenAL의 출력장치별 spatial mixer curve는 게임 실행 파일에 들어 있지 않다. 현재 AVAudioPlayer.pan=clamp(sourceX,-1,1)을 유지하며 이를 원본 곡선이라고 주장하지 않는다. 특히 sourceX가0에 가까울 때, 좌/우에 있을 때의 채널 gain은 원본 캡처와 비교해야 한다. 근거 없이 sign(sourceX)나 새 equal-power curve를 만들지 않았다.

원본 mixer 초기화는22050 요청이며 HMMedia는 현대 AVAudioSession의 출력 sample rate를 따른다. 원본 초기 getSound 요청31개는 HMMedia 초기화에서 같은 첫등장 순서로 생성하고 prepareToPlay를 호출한다. 실제 출력 latency의 수치적 일치까지 이 변경으로 입증하지 않는다. background/interruption/오디오 route 변화와 여러 이름 동시 합성 clipping은 별도 검증 대상이다.

## BGM와 우선순위

playBGM 0x13d2c는 stop→unload→WAV load→start 경로가 있다. 기존 전체 pseudocode 검색에서 직접 게임 caller는 확인하지 못했다. 이번 조사에서도 이를 재사용하며 숨은 호출이 없다고 완전 증명하지 않는다. BGM API 존재만으로 현재 앱에 들려야 할 배경음악이 누락됐다고 결론 내리지 않는다. 현재31개 파일은 이름상 UI/게임/SFX/무기/타이틀/loading이며 독립 음악 track은 식별되지 않았다.

1. 복원: 같은 이름 source1개 restart, 공간 거리 gain과 로드 순서별 maxDistance, 초기 요청31개 eager 생성·prepare. 실제 앱 호스트 검사 결과는 [audio-preload](audio-preload.md)에 구분한다.
2. 남음: 구형 원본 출력과 native를 동일 소리·거리·반복 간격으로 녹음해 pan·latency·합성 결과 비교.
3. BGM는 호출 근거가 생길 때 기능 요구를 추가한다.

## 수행한 검사

AudioContractTests는 중앙/근거리 gain1, ±거리 대칭, sourceX2→0.5,4→0.25, max800 clamp, 후로드 max 기본값, NaN API guard를 검사한다. ASan/UBSan 실행 exit0. HMMedia Objective-C++ simulator 및 device ARM64 syntax 검사 exit0. 기존 CoreGraphics enum 조합 경고3개는 남아 있다. 이 검사는 귀로 들은 원본 동등성이나 실기기 설치 성공을 의미하지 않는다.
