# Android 병합과 v0.17 태그 준비

## Apple 코드 보존 검사 · 2026-10-30

Android 작업 전 기준은 `d46e122a`, 검사 당시 원격 main은 `3962d7b7`다. 두 브랜치의 파일 트리는 같았다. 병합 준비 첫 검사에서 보호 목록 2,166개 파일의 해시가 기준과 같았다. 이후 이 목록에 포함된 국문·영문 README 2개의 Android 현황을 갱신했다. 나머지 2,164개 파일의 해시는 그대로다. `Sources/`, `Resources/`, `Art/`, `Modules/`, `iOS/`, `macOS/`, `vendor/`, workspace, 루트 CMake, 기존 도구·CI·Git 설정의 변경은 0개다.

Apple 프로젝트는 기존 소스와 자원 경로를 명시한다. Android 디렉터리를 포함하는 참조나 전체 저장소 검색 규칙이 없다. Android는 기존 코어와 자원을 읽고 자체 `app/.cxx/`와 `app/build/`에 빌드 결과를 만든다. Apple의 자원·생성물 경로에 쓰지 않는다.

일반·치트 앱을 iOS Simulator arm64와 네이티브 Mac arm64로 다시 빌드했다. 네 빌드가 모두 성공했다. iOS·iPadOS는 같은 target의 기기군 1·2를 사용한다. 컴파일 의존 파일과 링크 목록 164개에서 Android 참조는 0개였다. 이번 검사는 무서명 재빌드이며, 새 Apple 실기기 플레이 검사는 실행하지 않았다. [파일·빌드 증거](evidence/android-main-readiness-20261030.json).

## 병합 순서

`feat/android-app` → `develop` → `main` 순서로 병합한다. 각 병합 직전 원격 상태를 다시 확인한다. 현재 최신 게임 태그는 v0.16이며 다음 후보는 **v0.17**이다. 이번 요청 범위는 main 병합과 태그 준비다.

## v0.17 릴리스 노트 초안

### Android 앱 추가

기존 C++ 전투 코어와 게임 자원을 사용하는 Android 일반·치트 앱을 추가했다. Kotlin·JNI·C++ 호스트가 화면, 터치·틸트·키보드 입력, 메뉴·HUD, EGL/GLES 렌더링, OpenAL 음향과 저장을 연결한다. 백그라운드 전환·오디오 포커스 상실 시 게임을 멈추고, 그래픽 컨텍스트 재생성 시 자원을 복구한다.

Galaxy SM-S731N에서 USB·Wi-Fi 설치와 실행을 확인했다. 이동·점프·20종 무기 검사 31개, HUD·GPU 복구 9개, 35스테이지 진입 등을 포함한 캠페인 검사 39개, USB 분리 후 무선 검사 11개를 통과했다.

첫 보스 클리어 뒤 2-1 진입 시 업그레이드 창 뒤로 전투가 계속되고 버튼이 반응하지 않던 오류를 수정했다. 에뮬레이터와 Galaxy에서 각각 12개 회귀 검사를 통과했다. 수정본으로 이어서 플레이한 범위에서 개발자가 오작동을 발견하지 못했다고 보고했다.

[Android 설치](../Android/README.md), [개발 과정](public/android-development-20261030.md), [Galaxy 플레이 이미지·GIF·영상](public/android-gameplay-20261030.md).

### Apple판

기존 Apple 소스·공용 코어·자원·프로젝트·빌드 도구를 유지했다. iOS Simulator와 Mac의 일반·치트 앱 재빌드를 통과했다.

### 남은 검사와 배포

Android 전체 캠페인 클리어, 장시간 발열, Android 태블릿 실기기, 16KB 페이지 기기 실행과 배포 서명이 남아 있다. 현재 설치 APK는 최적화한 debug 서명본이다. Android 앱 내부 버전은 `0.16-android-dev`이며 Git 태그 후보 v0.17과 별도로 관리한다.

## 태그를 붙일 때

이 초안의 확정 내용을 국문·영문 CHANGELOG에 넣고 번역 명세를 갱신한다. develop을 거친 main 병합 커밋에 v0.17 주석 태그를 붙인다. [릴리스 절차](release-notes-plan.md)에 따라 소스·자원 압축본과 고지 묶음을 만들고 GitHub Releases에 게시한다. 기존 v0.16 태그와 Release는 유지한다.
