# 앱 아이콘과 최신 기능 이미지

## 글자 없는 앱 아이콘

앱 아이콘에서 `Episode 1 : STORM OF BULLETS`와 `HEAVY MACH` 글자를 제거했다. 글자에 가려져 있던 전차 하부·궤도·지면을 이어 그렸다. 원작의 전차 구도와 황갈색 그림을 기준으로 작업했다.

| 기존 | 변경 |
| --- | --- |
| ![기존 글자 포함 아이콘](media/app-icon-20261029/before.png) | ![글자를 제거한 전차 일러스트 아이콘](media/app-icon-20261029/icon.png) |

iPhone·iPad 정식/치트 앱은 같은 1024px 그림을 사용한다. Mac 정식/치트 앱에도 공용 아이콘을 연결했다. Android용 48~192px와 스토어 512px 파일도 준비했다. Android 앱·APK는 아직 개발 전이다.

게임 안의 타이틀 일러스트와 로고는 그대로다. [원본·생성 지시문·플랫폼 파일](../../Art/AppIcon/20261029/README.md).

## 쉴카와 퉁구스카

쉴카는 흰색 총탄 6발을 연속 발사한다. 최종 포탑 높이는 퉁구스카와 같은 29다. 퉁구스카는 흰색 총탄 4발과 미사일 2발을 연속 발사한다. 두 유닛 모두 원작 지상 유닛의 이동·파츠 동작을 재사용하고 레이더는 독립적으로 회전한다. 기존 적 배치를 유지하며 35개 스테이지에 총 152대를 추가했다.

![최종 포탑 높이를 적용한 쉴카가 경사면에서 총탄을 발사한다](media/ground-aa/shilka-height/shilka.png)

![경사면에서 미사일을 발사하는 퉁구스카](media/ground-aa/integrated/ipad-tunguska.png)

위 그림은 실제 기기의 치트 앱에서 조립·사격을 검사하며 저장했다. 쉴카는 마지막 높이 수정본, 퉁구스카는 현재 유지 중인 최종 파츠 적용본이다. [파츠와 수정 내역](ground-aa-art-20261028.md).

## HUD 위치 편집

미션 선택 또는 일시정지의 **SET GAME UI**에서 버튼·계기판·상단 장식을 옮긴다. 그림과 터치 판정이 같은 위치로 이동한다. 편집 중에는 전투가 멈추며, **APPLY**로 저장하고 **RESET LAYOUT**으로 기본 배치로 돌린다.

![아이폰 치트 앱에서 상단 금속 장식을 아래로 옮긴 화면](media/hud-editor/release-iphone-trim.png)

![MENU를 오른쪽으로 옮긴 뒤 새 위치를 눌러 연 멈춤 메뉴](media/hud-editor/release-iphone-menu.png)

상단의 방향키는 치트 앱의 검증용 버튼이다. 정식 앱에도 같은 HUD 편집 기능이 들어 있다. [편집 기능과 검사](hud-improvement-plan-20261028.md).

## 빌드와 설치 확인

iOS 시뮬레이터와 native Mac의 정식·치트 앱 4종을 빌드했다. Mac 앱의 아이콘 묶음과 iPhone 시뮬레이터 홈 화면에서 글자 없는 그림을 확인했다. 서명한 정식·치트 앱을 실제 iPhone과 iPad에 업데이트 설치하고 정식 앱을 실행했다.

## GitHub와 SaaS에 사용할 자료

GitHub README에는 신규 지상 유닛과 HUD 편집 절을 추가했다. 기존 플레이 이미지 8장과 영상은 유지한다. SaaS 담당 세션은 아래 목록에서 기능별 이미지·캡션·촬영 조건을 가져갈 수 있다.

[장면 목록](media/updates-20261029/README.md) · [파일별 출처·해시·캡션](media/updates-20261029/media-scenes.json) · [Apps·Mags 집필 안내](publication-inventory.md).
