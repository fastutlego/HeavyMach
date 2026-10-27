# Heavy Mach 후속 타이틀과 Defense 포크 조사

2026-10-26에 Codex가 내가 전달한 Internet Archive 검색과 그 검색에서 선택한 모음을 조사했다. **Heavy Mach 2, Heavy Mach 2 HD, Mach Defense, Heavy MACH: Defense의 IPA가 모두 있다.** 중단한 Defense 작업도 원격 저장소와 로컬 폴더에서 찾았다.

## 원래 링크에서 확인한 파일

출발점은 [내가 전달한 검색](https://archive.org/search?query=heavy+mach&tab=all), 파일 모음은 [Heavy MACH IPA Archive](https://archive.org/details/heavy-mach-archive_202406)다. 상세 페이지의 브라우징 도구 조회는 실패했으나 [Archive 메타데이터 API](https://archive.org/metadata/heavy-mach-archive_202406)를 직접 조회해 IPA 19개의 이름·크기·SHA-1을 받았다.

| 아카이브 분류 | 확인한 버전 | IPA 수 | 기록된 번들 ID |
|---|---|---:|---|
| Heavy Mach | 1.10, 1.11, 1.12, 1.13 두 판본, 1.15, 1.22, 1.24 | 8 | com.indiean.HeavyMach / kr.connect.touch.HeavyMach.game |
| Heavy Mach 2 | 1.0.0, 1.1.0, 1.3.0, 2.0.1 | 4 | kr.indieapps.HeavyMach2 / com.indiean.HeavyMach2 |
| Heavy Mach 2 HD | 1.0.0, 1.0.3 | 2 | kr.indieapps.HvM2HD |
| Mach Defense | 1.0.3, 1.0.5 | 2 | kr.indieapps.MachDefense |
| Heavy MACH: Defense | 1.0.3 | 1 | com.wemadeent.HeavyMachDefence |
| Heavy MACH: Assault | 1.0.3 표기와 Clutch 표기 파일 | 2 | com.wemadeent.HeavyMachAssault |

번들 ID와 버전은 이번에는 파일명에서 읽었다. 새 IPA의 Info.plist·실행파일을 추출해 확인한 값은 아니다. 모음의 제작자 필드에는 IndieAn, IndieApps, WeMade Creative, WeMadeEntertainment가 함께 기록돼 있다. 이 정보와 파일 분류를 근거로 시리즈 자료임을 확인했으며, 서로 다른 Defense 두 판본의 내용이 같다고 판정하지 않았다.

선택한 네 파일은 다운로드 주소의 HEAD 요청도 확인했다. 모두 HTTP 200과 아래 Content-Length를 반환했다.

| 파일 | 크기(바이트) | 아카이브 SHA-1 |
|---|---:|---|
| HeavyMach2 v2.0.1 | 55,854,756 | 849da2eeef19807f2ab1f4e3111cd92e782a478f |
| HeavyMach2HD v1.0.3 | 69,171,560 | 472bbd0acbe632fe60ffc312c9f2fa5bad6b96b2 |
| Mach_Defense v1.0.5 | 62,698,352 | 72ade18b5c5b62e37e9e71363597136559518cbe |
| MachDefense v1.0.3, WeMade | 62,512,014 | 1e22f0c772cb5ec23525dd34ca5157c733d502bc |

파일 전체를 내려받거나 해독·실행하지는 않았다. 위 SHA-1은 서버가 제공한 값이다. 새 복원에 사용할 때는 실제 다운로드의 SHA-256도 계산하고 아키텍처·암호화 상태·호환 실행 여부를 별도로 확인한다. Heavy MACH: Battle은 모음의 검색어에는 있지만, 현재 19개 IPA 목록에는 없다. 다른 항목이나 `Extra/HeavyMachTrailers.7z` 내부의 존재까지 조사한 결과는 아니다.

전체 파일명과 서버 해시는 [19개 IPA 목록](https://github.com/gelb2/HeavyMach/blob/main/docs/research/related-title-archive-20261026.csv), 네 다운로드 주소와 응답은 [조사 근거 JSON](https://github.com/gelb2/HeavyMach/blob/main/docs/research/android-and-titles-evidence-20261026.json)에 보존했다.

## 중단한 Defense 작업

내 포크는 [gelb2/hm-defense](https://github.com/gelb2/hm-defense), 원본은 [Mesabloo/hm-defense](https://github.com/Mesabloo/hm-defense)다. GitHub API로 부모 저장소 관계를 확인했다. 원격 README는 Desktop·Android용 재개발을 설명하지만 Android 설치 절은 오래된 TODO다. 이 README만으로 로컬 작업 상태를 판단하면 안 된다.

| 위치 | 확인한 상태 |
|---|---|
| 원격 main | a4446660141e78829aa573af2e66de3329a19d00 |
| 원격 dev | 7b8a6419a38e57a4850cf70b485c16e03389bbcb |
| 로컬 | /Users/sokol/Git/hm-defense |
| 로컬 브랜치 | feature/add_android_build |
| 로컬 HEAD | 022d6911679aa2f0792d915cc490530c0714a9e9, 2026-03-03 |
| 원격 main과 비교 | 로컬이 81커밋 앞섬. 로컬 origin/main 값은 이번 GitHub API의 main 값과 일치 |
| 기존 미추적 작업 | .claude/ 유지. 이 조사에서는 수정·커밋·푸시하지 않음 |

로컬은 Kotlin·LibGDX 1.13.1이며 `core`, `desktop`, `android` 모듈이 있다. Android Gradle Plugin 8.2.2, Gradle 8.7, JDK 17, minSdk 21, target/compileSdk 34 설정이다. `android/build/outputs/apk/debug/android-debug.apk`도 남아 있다. 과거 CHANGELOG에는 약 55MB debug APK 생성과 pause/resume 수정이 기록돼 있다. **이번에는 해당 APK를 재빌드·설치·플레이하지 않았다.**

로컬 TODO에는 터렛, 기지 포격, 일부 필살기, GL context 소실 시 텍스처 복구, atlas 해제, 지형 통과 검사 등이 남아 있다. 최근 커밋의 51개 생명주기 검사 추가도 이 미완료 항목을 모두 해결했다는 뜻은 아니다. 재개 시 원격 main만 clone하지 말고 이 로컬 브랜치·미추적 자료·기존 APK를 먼저 보존해야 한다.

## 다음 복원에 쓰는 방법

1. Heavy Mach 2는 2.0.1을 우선 조사 후보로 두고 1.0.0/1.3.0, HD 1.0.3과 자원·데이터·함수 차이를 비교한다. 최신 파일명이 곧 정상 실행본이라는 뜻은 아니다.
2. Defense는 내 포크가 대상으로 삼은 WeMade `HeavyMachDefence` 판본과 먼저 대조한다. `kr.indieapps.MachDefense`의 규칙·에셋을 확인 없이 섞지 않는다.
3. 선택한 IPA별 해시·Info.plist·Mach-O 아키텍처·암호화·자원 목록을 REA 기록으로 남긴다. 실행 가능한 비교판을 확보한 뒤 저장·메뉴·전투·수치를 대조한다.
4. Defense는 현재 Kotlin 소스와 남은 TODO를 이어서 조사한다. 현재 HeavyMach 1의 C++ 소스와는 다른 게임이다. 두 프로젝트의 게임 로직을 합치지 않는다.

이번 요청에서 완료한 것은 타이틀 존재 확인과 중단 작업 위치 조사다. 후속 타이틀 다운로드·디컴파일·복원 개발은 아직 시작하지 않았다.
