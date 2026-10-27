# 변경 이력

## 2026-10-28 — 도입문·기여 안내·자동 검사·소스 릴리스 준비

README와 개발자의 변에 같은 도입문을 추가했다. About·Topics, 버그·기능 제안·PR 양식, 문서·자원·코어·Apple 빌드 workflow를 구성했다. v0.12 압축본의 자원1,482개·코어21개·앱 빌드4개·OpenAL 재링크2개를 확인했다. 외부 고지와 공유 이미지, 자원 공개 범위 목록을 추가했다. [검사 결과와 남은 항목](docs/public/github-followup-20261028.md)

버전은 Git의 출시 태그다. 앱 번들 버전과 별도로 관리한다. 출시 태그는 develop을 main에 병합한 커밋을 가리킨다.

## 2026-10-28 — GitHub 문서 main 반영

`docs/github-publication`에서 README와 기능·조작·설치·구조·트러블슈팅·개발 과정·검증 문서를 작성했다. 플레이 이미지 8장, GIF 2개와 MP4 3개를 배치했다. 게임 소스·에셋은 v0.12와 같다. develop `0a2c0e33`을 거쳐 main `db634ce6`에 병합했다.

후속 문서 변경에 새 코드용 MIT·적용 범위·자원 조건·외부 고지와 기술 배지5개, 환경 표, GitHub 구성 점검·계획을 추가했다.

## v0.12 — 2026-10-27

[main 병합 eedf245f](https://github.com/gelb2/HeavyMach/commit/eedf245f)

- F-22·Su-57·MiG-31 추가. 금색 조종석, HP 765, 미사일 3연사·2발씩 3묶음.
- 보스전에 2·4·6·8대, 직전 미션 마지막 구간에 4대씩 증원.
- 화면 밖에서 진입하고 진입 중에는 발사하지 않도록 수정.
- 연사 속도 증가를 25%에서 10%로 조정. 미사일 이동 속도 25% 증가 유지.
- 아이콘·자동조준 상태 안내·튜토리얼·플레이 영상 과제 기록.

제품 구현은 `45befedc`다. 코어 21개와 Apple 플랫폼별 전투·입력·진입 검사, iPhone·iPad 일반·치트 설치를 완료했다. [기능·검사](docs/public/features.md)

## v0.11 — 2026-10-27

[main 병합 f0c4432f](https://github.com/gelb2/HeavyMach/commit/f0c4432f)

- 전체 화면 도어 전환과 경계 틈 수정.
- 자동조준 ON/OFF·표적 선택·전환·포신 조정·발사 버튼 추가.
- 기존 HUD에 맞춘 버튼과 LOCKED·TRACK·BLOCKED 표지 추가.
- 일반 앱에도 자동조준 적용.
- C 명중 보정 삭제. 자동조준 OFF의 25,200프레임 사격 기록이 추가 전 코드와 같음을 확인.

[도어](docs/public/fullscreen-transitions-20261027.md) · [자동조준 기록](docs/public/auto-aim-ipad-20261027.md)

## v0.10 — 2026-10-27

[main 병합 76cbd21c](https://github.com/gelb2/HeavyMach/commit/76cbd21c)

- 필요한 게임·검사 자원을 Git/LFS에 포함하고 새 clone 빌드 검사.
- 공용 코어·Apple 런타임·자원 bundle과 workspace 구성.
- iPad 지원, AppKit Mac 앱, 키보드 전투·메뉴 조작과 리매핑 추가.
- Mac 개발 서명·표시 대기·전체 화면 문제 수정, 누수 0건과 33개 검사 확인.
- iPhone·iPad 실기기·무선 설치·LLDB 검사.
- Yak-141·Harrier GR3·F-35B 113대 추가. 외곽선·크기·후방 불꽃 수정.

[Apple 검사](docs/public/apple-platform-progress-20261026.md) · [VTOL](docs/public/vtol-art-and-loadout-20261027.md)

## v0.9 — 2026-10-26

[태그 v0.9](https://github.com/gelb2/HeavyMach/tree/v0.9)

전투·성장·미션·보스·메뉴·저장과 iPhone 일반·치트 앱의 정상 플레이 기준을 기록했다. 렌더링·음향·저장·시간·틸트·필살기·결과 그림·넓은 화면·메뉴 배경을 수정한 버전이다. 전체 미션을 직접 플레이했을 때 크래시·오동작·적 이동 이상은 보이지 않았다.

v0.9는 게임 자원을 Git에 넣기 전 태그다. [당시 버전 재빌드](docs/repository-management.md)
