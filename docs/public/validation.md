# HeavyMach 검증 결과

<!-- language-nav:start -->
[한국어](validation.md) | [English](../en/public/validation.md)
<!-- language-nav:end -->

현재 게임 버전은 v0.16이다. 아래 표에는 각 버전에서 실행한 검사와 당시 측정값을 기록했다. 자동 검사·계측은 Codex가 실행했고, 전체 미션 플레이는 내가 직접 했다.

## 플레이와 기능 검사

| 시점·대상 | 실행한 검사 | 결과 |
|---|---|---|
| v0.9 직접 플레이 | 설치한 iPhone 앱의 전체 미션 | 크래시·오동작·적 이동 이상 미관측 |
| 10월 26일 새 원격 clone | 빌드 4개, 코어 17개, iOS 14개, 새 Simulator의 일반·치트 실행 | 통과. 당시 이미지 695개 디코딩 비교 포함 |
| 10월 26~27일 native Mac | 개발 서명, 잠금 해제, 이동·점프·조준·사격·무기20종·메뉴·GL·음향·전체 화면 | 33/33 통과. 무기 선택 280조합 포함 |
| 10월 27일 iPhone Air | iOS 27.0.1, 공용 모듈 치트 앱 전체 suite | 84/85 통과. 표시시간 비교 1개 실패 |
| 10월 27일 iPad Air 13형 M2 | iPadOS 27.0.1, 같은 suite | 84/85 통과. 표시시간 비교 1개 실패 |
| C 삭제 `15968772` | 코어 20개, Simulator·iPhone·iPad 각 입력 4개 | 통과 |
| C 삭제 뒤 수동 사격 | 추가 전 `8fdeb629`와 20무기 × 3시드 × 420프레임 | 25,200프레임 바이트 일치 |
| VTOL | 기존 적 1,182항목 보존, 새 113대, 이동·공격·자세·정지·격파, 60개 무기 충돌 조건 | 통과 |
| 전투기 `45befedc` | 코어 21개, Simulator·iPhone·iPad·Mac 전투/입력 각 4개와 HUD 검사 | 통과. 후속 화면 밖 진입 검사 포함 |
| v0.16 진입 배치 | 140조건, 코어 23개, iPhone·iPad 기능 각 3개 | 통과. iPad 진입 p95 21.39ms는 기준 20ms 초과 |
| 최종 설치 앱 직접 플레이 | iPhone·iPad 일반 앱 | 프레임 드랍·오작동 미관측 |

실기기의 84개 통과 항목에는 키보드 20무기 선택·발사, 터치 병용, 자원·생명주기·음향·전투 중 저장 호출 0회가 포함된다. Codex는 Device Hub의 OS 키 입력으로 이동·점프·조준·사격·무기 전환을 확인했다. 실제 Bluetooth·USB 키보드로 하는 조작 검사는 남아 있다.

[새 clone 기록](clone-build-20261026.md) · [Apple 검사](apple-platform-progress-20261026.md) · [자동조준](auto-aim-ipad-20261027.md) · [VTOL](vtol-art-and-loadout-20261027.md) · [전투기](lux-lunae-design-20261027.md)

## 프레임·메모리 계측

| 대상·조건 | 관측값 | 해석·남은 일 |
|---|---|---|
| native Mac, 개발 서명·잠금 해제·계측기 미연결, 30프레임 준비 후 180프레임 측정 | 1200×800·1920×1080 여섯 장면 p95 10.86~14.29ms, 평균 호출 간격 16.67~16.69ms | 해당 장면의 16.67ms 처리 기준 통과 |
| Mac 일반·치트, 개발 인증서 | 누수 0건 | 이전 ad-hoc 앱의 417개·20,016바이트 연결 누수 해소 |
| iPad Simulator, Apple Software Renderer | 작업본 p95 44.311~48.133ms, 이전 main 39.972~40.580ms | 두 빌드 모두 기준 실패. 최종 확대 표시에서 소프트웨어 픽셀 처리 비용 집중 |
| iPhone 후속 메뉴 계측 | p95 17.309ms, 재측정 17.629ms | 16.67ms 기준 초과. 원인·재현 조건 추가 확인 필요 |
| 초기 iPhone 전투 127.77초 | 평균 59.71회/초, p95 간격 16.67ms, 최대 50.02ms, thermal Serious | 대부분 정지 위치에서 사격. 장시간 이동 전투·전력 비교 미실행 |
| iPad 짧은 계측 | leaks 0건, physical footprint 493.2MB | XCTest·화면 재생성 포함. 장시간 일반 플레이 메모리 검사 필요 |

Simulator는 Apple Software Renderer로 측정했다. iPhone·iPad의 기기별 측정값과 직접 플레이 결과는 위 표에 있다.

Mac 전체 화면은 OS의 3840×2160 캡처에서 네 가장자리까지 게임이 표시됨을 확인했다. CUA 창 캡처에 보인 흰 여백은 실제 디스플레이 캡처에 없었다. [계측 JSON](../evidence/apple-platform-performance-20261026.json), [전체 화면 JSON](../evidence/mac-fullscreen-capture-20261027.json)

## 설치·무선 디버깅

Codex는 일반·치트 앱을 iPhone·iPad에 각각 설치했다. 케이블을 분리한 상태의 기기 연결·설치·실행·LLDB 연결도 확인했다. iPad에서는 무선 검사 8개와 시작 구간 CPU 21.716초 기록을 남겼다. iPhone 무선 XCTest·Instruments 전체 실행과 재부팅 후 반복 검사는 남아 있다.

[무선 절차](../wireless-device-workflow.md) · [결과 JSON](../evidence/wireless-apple-20261027.json)

## 문서·미디어 확인

현재 자원 검사에서 빌드·검사 입력 1,504개의 SHA-256이 일치했다. Codex가 내 플레이 영상에서 그림 8장과 MP4 3개를 추출했고, GIF 2개는 같은 장면의 짧은 구간이다. 원본 파일 해시·시간·출력 해시는 [장면 명세](media/gameplay-20261027/media-scenes.json)에 있다. 촬영 기종·앱 종류·빌드 번호는 제공되지 않았다.

## 남은 검사

- 표시시간: iPhone 메뉴 기준 초과와 iPhone·iPad 전체 suite의 해당 실패 재현·원인 분리.
- 장시간: 이동·점프·전투·미션 전환을 포함한 발열·전력·메모리 변화.
- 입력: 실제 Bluetooth·USB 키보드 연결, 분리·재연결·키 유지·터치 병용.
- 난이도: 추가 항공기가 등장하는 전체 미션의 탄약·피해·클리어 난이도.
- 무선: iPhone XCTest·Instruments, 두 기기의 재부팅 후 반복 설치·디버깅.

[전체 무기 검사 절차](../weapon-validation-procedure.md) · [현행 과제](implementation-status.md)
