# 미션 클리어 일러스트 누락 수정

사용자 보고: 미션 클리어 후 일러스트가 나오지 않는다. 비교 기준은 사용자가 정상 동작을 확인한 IPA 포팅판이다. 이번에는 포팅판의 원본 함수·자원 로딩 코드와 새 앱을 대조했다. 폰은 사용자 지시에 따라 분리한 상태이므로 새 실기기 재현·설치는 하지 않았다.

## 원인과 수정

원본 `GobWorld.setNextStage` 0xb8f0은 미션 종료에 `GameLoading_%02dE.jpg`, 스테이지 이동에 `GameLoading_%02d.jpg`를 선택하여 ResMgr_Tile에 전달한다. `setGameOver` 0xbf74는 `StageClear.jpg`를 사용한다. 근거는 `Analysis/functions/0xb8f0.pseudo.c`·`.asm`, `0xbf74.pseudo.c`·`.asm`이다. 포팅판 `touchHLE/src/frameworks/uikit/ui_image.rs`의 `initWithContentsOfFile`은 요청 경로의 바이트를 읽어 `Image::from_bytes`에 전달한다. `src/image.rs`는 stb로 JPEG를 직접 해석한다. 원본 식별은 [출처](source-provenance.md)를 따른다.

새 앱의 World도 원본 JPEG 이름을 선택하지만 HMGameHUD.drawPresentation이 `.png`를 추가했다. 예전 변환 이미지 `Images/GameLoading_01E.jpg.png`를 사용하던 코드가 남은 것이다. 현재 HMMedia는 `OriginalImages`에서 원본을 직접 읽으므로 해당 이름을 찾지 못하고 draw를 건너뛰었다. JPEG 디코더 실패나 일러스트 자원 부재가 원인이 아니다.

HUD의 접미사 추가 두 줄을 제거했다. 원본 이름과 원본 JPEG를 그대로 사용한다. 에셋·미션 순서·전환 타이밍·좌표·저장 코드는 변경하지 않았다. [그래픽 정책](rendering-policy.md)에 원본 확장자 보존을 명시했다.

## 회귀 검사

`HMResourceLifecycleTests.testResultScreensRenderOriginalJPEGIllustrations`는 실제 HMMedia/HMGameHUD/OpenGL ES 경로로 15개 결과 화면을 그린다. 미션7개, 스테이지7개, 게임오버1개를 검사한다. 원본 JPEG 직접 표시와 HUD 결과 화면의 일러스트 영역 픽셀이 같고 비어 있지 않은지 확인한다. 파일 존재나 호출 이름만 검사하지 않는다. 미션1 결과의 실제 렌더 이미지를 xcresult에 첨부한다. 전체 원본 픽셀 동등성 인증으로 확대하지 않는다.

최초 `before.xcresult`에서 15개 모두 실패했고 `Missing original image …jpg.png` 로그가 재현됐다. 첫 수정 후 `after.xcresult`에는 이미지가 나왔지만 검사에서 지정하지 않은 기본 typo 회전5가 비교 영역을 침범하여 15개 비교가 실패했다. 첨부 이미지로 회전한 글자와 정상 일러스트를 확인했다. 검사 fixture의 글자 회전을0으로 명시하여 중앙 일러스트 영역에서 제외했다. 제품 회전이나 비교 오차 허용값을 바꾸지 않았다. 두 실패 기록을 모두 보존한다.

증거 폴더: `build/clear-illustration-20261025/`. 최종 검사·빌드 결과는 [현재 상태](current-status.md)에 기록한다.
