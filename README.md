# HeavyMach

이와 비슷한 프로젝트를 진행한 [mesabloo](https://github.com/Mesabloo/hm-defense)라는 사람이 있었다. 그는 HeavyMach Defense의 에셋을 구하고 소스코드를 밑바닥부터 새로 작성했다. 유닛 간 교전 알고리즘을 구현하다가 멈춘 듯했다. 나는 소스코드를 밑바닥부터 새로 작성해야 했던 그 보다는 시작점이 더 나은 상황이었다.

주말에 잠을 자다가 꿈을 꿨다. 꿈속에서 아이팟터치와 Cydia를 처음 접하던 나를 제3자의 시점으로 보고 있었다. 일어나서 구글에 heavy Mach을 쳐봤다. 구글 검색의 Gemini가 HeavyMach 원본을 받을 수 있는 웹 아카이빙 전문 사이트를 알려줬다. 그게 이 복원 프로젝트의 시작이다.

그러므로 나는 내가 한 일이 거의 없다고 생각한다. 이것으로 돈을 벌 생각을 하면 더더욱 안 된다고 생각하는 것도 이 때문이다. 내가 한 것은, 내 표현을 빌리자면 Codex가 원본의 소스코드와 원작자의 미학적 성취에서 벗어나 자의적으로 잘못된 행동을 할 때 멱살을 쥐어 잡아서 따귀를 올려붙인 것뿐이다.

2009년의 횡스크롤 전차 게임 **HeavyMach**를 현재의 iPhone·iPad·Mac에서 실행하도록 다시 만들었다. 전차를 움직이고 뛰어오르며, 무기를 바꿔 지상·공중의 적과 싸운다. 원작의 전투와 성장 구조에 넓은 화면, 키보드 조작, 자동조준과 새로운 항공기를 추가했다.

[![Git tag v0.12](docs/public/media/badges/version.svg)](https://github.com/gelb2/HeavyMach/tree/v0.12)
[![Platforms iOS iPadOS macOS](docs/public/media/badges/platforms.svg)](docs/public/validation.md)
[![Architecture arm64](docs/public/media/badges/arch.svg)](docs/public/build-and-install.md)
[![C++20 and Objective-C++](docs/public/media/badges/code.svg)](docs/public/architecture.md)
[![Code license MIT](docs/public/media/badges/license.svg)](LICENSE-SCOPE.md)

대학생 때 iPod touch로 하던 HeavyMach를 2026년의 아이폰에서 다시 실행하고 싶었다. 이 게임은 내가 2017년 7월 iOS 개발자로 전업하는 데 영향을 준 작품이다. 구현은 Codex에 맡겼다. 나는 직접 플레이하면서 원작과 달라진 움직임·성능·그림을 확인하고 수정을 지시했다. [개발자의 변 전문](docs/RESTORATION.md)

![공중의 적을 향해 사격하는 탱크. 녹색 모서리와 LOCKED 문구가 표적을 표시한다.](docs/public/media/gameplay-20261027/01-auto-aim.png)

**현재 게임 버전은 [v0.12](https://github.com/gelb2/HeavyMach/tree/v0.12)**다. 일반 앱과 치트 검증 앱을 각각 설치할 수 있다. 치트 앱에서는 미션·무기·강화 조건을 지정해 같은 게임을 검사한다.

[설치](docs/public/build-and-install.md) · [조작](docs/public/gameplay-guide.md) · [기능](docs/public/features.md) · [트러블슈팅](docs/public/troubleshooting.md) · [전체 문서](docs/README.md)

## 전투와 조작

iPhone·iPad에서는 기기를 기울여 이동하고 화면을 터치해 조준·발사한다. 점프와 무기 선택은 화면 버튼으로 조작한다. 키보드를 연결하면 이동·점프·조준·발사·무기 전환과 메뉴를 모두 키로 다룰 수 있다.

![경사로 위로 뛰어오른 탱크가 오른쪽 적을 향해 사격한다.](docs/public/media/gameplay-20261027/02-jump.png)

자동조준을 켜면 전방의 적을 추적하고 무기의 탄도를 계산해 포신을 조절한다. 플레이어는 이동·점프·무기 선택·발사를 맡는다. 표적 전환 버튼으로 다른 적을 선택할 수 있다.

![자동조준한 공중 적에게 사격하는 5초 장면](docs/public/media/gameplay-20261027/01-auto-aim.gif)

[소리 있는 자동조준 영상 — 7초](docs/public/media/gameplay-20261027/01-auto-aim.mp4)

![공중 적과 싸우며 점프하는 탱크](docs/public/media/gameplay-20261027/08-jump-clip.png)

![사격 중 점프하고 착지하는 6초 장면](docs/public/media/gameplay-20261027/08-jump-clip.gif)

[점프·사격·무기 목록 영상 — 7초](docs/public/media/gameplay-20261027/08-jump-clip.mp4)

## 무기와 성장

무기마다 탄속·분산·중력·연사 간격이 다르다. 자동조준도 같은 탄환 규칙을 사용하므로 적의 움직임과 무기 분산에 따라 빗나갈 수 있다. 무기를 바꾸고 위치를 옮기며 공격 방법을 고른다.

![상단 무기 목록과 선택 테두리. 흰 궤적이 전장을 가로지른다.](docs/public/media/gameplay-20261027/03-weapon.png)

![푸른 광선 여러 줄이 상공에서 지상으로 내려온다.](docs/public/media/gameplay-20261027/04-special.png)

경험치를 얻어 레벨이 오르면 이동·방어·공격을 강화한다. 미션을 끝내면 결과 그림과 다음 스테이지 선택 화면이 나온다.

![MOVEMENT, DEFENSE, ATTACK을 고르는 업그레이드 창](docs/public/media/gameplay-20261027/05-upgrade.png)

![전차 일러스트 위에 STAGE CLEAR와 다음 스테이지 버튼이 표시된다.](docs/public/media/gameplay-20261027/06-stage-clear.png)

## 추가한 항공기

Yak-141·Harrier GR3·F-35B는 기존 적 배치 사이에 추가했다. F-22·Su-57·MiG-31은 보스전과 직전 미션의 마지막 구간에 증원으로 등장한다. 기존 헬기의 무기를 재사용하고, 전투기에는 미사일 묶음 발사와 금색 조종석 유리를 적용했다.

![화면 오른쪽의 검은 항공기 여러 대를 향해 탱크가 광선을 발사한다.](docs/public/media/gameplay-20261027/07-fighters.png)

[추가 항공기 전투 영상 — 13초](docs/public/media/gameplay-20261027/07-fighters.mp4) · [기획·수치·등장 미션](docs/public/features.md)

이미지 8장과 영상은 Codex가 내 플레이 녹화에서 추출했다. 나는 이미지를 확인하고 모두 README에 넣도록 했다. GIF는 원래 속도로 재생하며 소리는 MP4에 있다. [원본 시간·캡션·파일 목록](docs/public/media/gameplay-20261027/README.md)

## 플랫폼과 개발 상태

| 대상 | 지원 내용 | 확인한 범위 |
|---|---|---|
| iPhone | 가로 회전, 터치·틸트, 키보드, 일반·치트 앱 | iPhone Air 설치·입력·전투 검사, 내 플레이 |
| iPad | 화면 비율에 맞춘 캔버스, 터치·틸트·키보드 병용 | iPad Air 13형 M2 설치·입력·전투 검사, 내 플레이 |
| Mac | AppKit 앱, 창·전체 화면, 키보드 조작 | Mac mini 실행·33개 검사, 개발 서명 앱 누수 0건 |
| Android | 공용 C++ 코어를 활용하는 개발 계획 | 앱·APK 미구현 |

설치한 iPhone·iPad 앱을 직접 플레이했을 때 프레임 드랍이나 오작동은 보이지 않았다. 기기별 자동 검사에는 표시시간 기준 실패가 남아 있다. [측정 조건과 결과](docs/public/validation.md)

자동조준·공용 모듈·Apple 세 플랫폼·항공기 6종·전체 화면 도어는 구현됐다. HUD 위치 편집, 자동조준 상태 안내 개선, 튜토리얼 0-0~0-4, 글자를 뺀 앱 아이콘과 Android는 후속 과제다. [완료·진행·남은 과제](docs/public/implementation-status.md)

**빌드 설정과 검사 환경**

| 플랫폼 | 프로젝트 최소 OS | 실제 검사 기록 |
|---|---|---|
| iPhone·iPad | iOS/iPadOS 15.0 | iPhone Air·iPad Air 13형 M2, OS 27.0.1 |
| Mac | macOS 12.0 | Mac mini M4 Pro, macOS 27.0.1 |

현재 빌드는 Apple Silicon·arm64 대상이다. 최소 OS 설정과 같은 버전의 실기기를 모두 검사한 것은 아니다. 빌드 도구 버전은 [설치 문서](docs/public/build-and-install.md)에 있다.

## 빌드하기

Apple Silicon Mac, Xcode, CMake 3.20 이상, Python 3, Git LFS를 사용한다. 소스·게임 이미지·음원·맵·빌드 프로젝트는 이 저장소에 들어 있다.

```sh
git clone https://github.com/gelb2/HeavyMach.git
cd HeavyMach
git lfs install --local
git lfs pull
python3 tools/verify_build_resources.py
open HeavyMach.xcworkspace
```

Xcode에서 본인의 개발 팀을 지정하고 실행 대상을 선택한다. iPhone·iPad 일반 앱은 `HeavyMachRestored`, 치트 앱은 `HeavyMachValidation`이다. Mac은 `HeavyMachMac`, `HeavyMachMacValidation`이다. 현재 저장소 접근에는 GitHub 권한이 필요하며 설치 파일은 Releases에 등록하지 않았다.

[시뮬레이터 명령·기기 설치·서명·오류 해결](docs/public/build-and-install.md)

## 개발 중 고친 문제

초기 구현에는 사격·피격 때마다 파일을 쓰는 경로, CPU에서 그림을 다시 만드는 렌더링, 음원 재시작 지연이 있었다. 나는 정상적으로 플레이한 기준과 비교해 왜 동작이 달라졌는지 물었고, Codex에 저장·그래픽·음향·시간 계산을 각각 대조하게 했다.

Codex는 파일 쓰기를 저장 시점에만 실행하도록 분리했다. 게임 그림은 OpenGL 텍스처로 그리고, 효과음은 OpenAL 버퍼와 소스를 재사용하도록 바꿨다. 화면 확장 뒤 생긴 특수무기 좌표·보스 접근·그림 경계 문제도 같은 방식으로 수정했다.

[트러블슈팅 17개 항목](docs/public/troubleshooting.md)에는 내가 본 증상과 질문, 코드 원인, Codex의 변경과 검사 결과를 정리했다. [개발 과정](docs/public/development-history.md)에는 비교 순서와 사용한 분석·구현·검사 도구를 정리했다.

## 코드 구조와 버전

하나의 workspace에서 공용 모듈·iOS/iPadOS 앱·native Mac 앱을 관리한다. C++ 게임 코어와 이미지·음원·맵은 공유하고, 플랫폼별 창·입력·그래픽 컨텍스트를 연결한다. 공용 코드는 정적 framework, 자원은 `HeavyMachAssets.bundle`로 앱에 포함한다.

[구조와 모듈화 이유](docs/public/architecture.md) · [검증](docs/public/validation.md) · [변경 이력](CHANGELOG.md)

작업 브랜치는 `develop`에 병합하고 `develop`만 `main`에 병합한다. 출시 태그는 `main`의 병합 커밋에 붙인다. [태그에서 다시 빌드하는 방법](docs/repository-management.md)

[원작 제작자·추가 작업·라이브러리](CREDITS.md) · [SaaS Apps·Mags 집필 자료](docs/public/publication-inventory.md)


## 라이선스와 자원

이 프로젝트에서 새로 작성한 코드에는 [MIT](LICENSE)를 적용한다. [적용 범위](LICENSE-SCOPE.md)에 원작 유래 자료·외부 코드·문서를 구분했다. 원작 그림·음원·게임 데이터와 추가 그림은 [자원 이용 조건](ASSET-LICENSES.md), OpenAL Soft·stb 등은 [외부 라이브러리 고지](THIRD_PARTY_NOTICES.md)를 따른다.

코드와 에셋을 함께 공개해 누구나 빌드하고 플레이할 수 있도록 준비하고 있다. 원작 에셋의 공개 이용허락은 아직 확인 중이다. 나는 이 복원판으로 돈을 벌 생각이 없으며, 향후 허락받은 앱스토어 배포도 무료로 제공할 생각이다.
