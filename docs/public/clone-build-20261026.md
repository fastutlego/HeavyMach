# 새 clone의 빌드 검증

Codex가 앱 빌드에 필요한 자원을 Git에서 제외한 저장소 구성을 수정했다. 이미지·음원·맵·앱 아이콘·이미지 비교 자료를 Git/LFS에 넣었다. 원본 IPA와 기존 Mac의 자원 폴더를 따로 가져올 필요가 없다.

## 빠졌던 파일과 수정

`Resources/` 전체를 `.gitignore`로 제외해 로컬에서는 빌드됐지만 다른 사람이 clone하면 자원이 빠졌다. 기존 IPA 독립 빌드 검사는 준비된 로컬 자원을 복사했으므로 이 누락을 검증하지 못했다.

이번에는 OriginalImages 695개, Images 695개, Audio 31개, RawData 46개, 앱 아이콘 구성 3개, 이미지 비교 자료 1개를 저장소에 추가했다. 기존 확장 그림 4개와 합친 1,475개 파일의 크기·SHA-256을 [자원 목록](https://github.com/gelb2/HeavyMach/blob/main/docs/build-resources.json)에 기록했다. 게임 소스·Xcode 구성·기존 에셋 바이트는 바꾸지 않았다.

정본·작업 지침·README·설치 가이드에서 자원을 Git에서 제외하던 규칙과 별도 복사 요구를 수정했다. 자원 검사 도구는 누락, 손상, 실제 파일 대신 남은 LFS 포인터를 검출한다. 네 가지 입력인 정상 파일·누락·손상·LFS 포인터로 검사 도구의 결과를 확인했다.

## 원격 clone과 빌드

검사한 자원 포함 커밋은 `daf0c178248b262148ee56008ef743cf7be80f73`이다. 새 임시 폴더에서 GitHub SSH 원격의 `fix/self-contained-resources` 브랜치를 clone하고 `git lfs pull`로 자원을 받았다. 기존 프로젝트의 파일을 복사하지 않았다. 새 clone의 자원 1,475개 해시가 모두 일치했다.

빌드는 기존 `/Users/sokol/Git`, `/Users/sokol/Documents/Codex`와 `.ipa` 확장자 파일의 읽기를 차단했다. 실제 차단 검사 3개가 읽기 실패를 반환했고 clone의 소스는 읽을 수 있었다. OpenAL과 Xcode DerivedData도 새 폴더에서 생성했다.

| 실행 | 결과 |
|---|---|
| 일반 앱 · Simulator arm64 | 빌드 성공, 새 Simulator 설치·실행 성공 |
| 치트 앱 · Simulator arm64 | 빌드 성공, 새 Simulator 설치·실행 성공 |
| 일반 앱 · iPhone arm64 | 서명 없는 빌드 성공 |
| 치트 앱 · iPhone arm64 | 서명 없는 빌드 성공 |
| 코어 | 17개 검사 통과 |
| iOS 자원·입력·음향 | 14개 검사 통과 |
| 네 앱의 자원 대조 | 이미지·음원·맵·확장 그림 누락과 해시 불일치 0개 |

각 앱에서 게임 코어·OpenGL·OpenAL 소스 29개를 새로 컴파일했다. iOS 검사는 이미지 695개의 디코딩 결과, OpenAL 재생·비활성 처리, 전체 무기 목표점, 확장 영역 터치, 무기 전환음, 이동 버튼·백그라운드 해제, 프레임 갱신과 난수를 확인했다.

검사 환경은 macOS 27.0.1, Xcode 27.1, iPhone Air Simulator·iOS 26.5다. 이번에 만든 Simulator를 사용했으며 기존 아이폰 저장은 건드리지 않았다.

![원격 clone 일반 앱의 첫 실행 화면](clone/HeavyMachRestored-title.png)

![원격 clone 치트 앱의 첫 실행 화면과 검증 버튼](clone/HeavyMachValidation-title.png)

## 재현 방법과 버전

[빌드와 설치](build-and-install.md)의 clone·LFS·자원 검사·빌드 순서를 따른다. Apple Silicon Mac, Xcode, CMake, Python, Git LFS가 필요하다. 아이폰에 설치하려면 본인의 개발 서명을 설정한다.

`v0.9`는 자원을 Git에 넣기 전의 태그다. 태그를 이동하지 않았다. 현재 main으로 새 빌드를 준비하며, v0.9 소스 복원 시 자원 준비는 [저장소 관리](https://github.com/gelb2/HeavyMach/blob/main/docs/repository-management.md)를 따른다.

## 실행 기록

원시 로그, 입력 해시, 접근 차단 규칙, XCTest 결과와 실행 스크립트는 로컬 `build/clone-build-20261026/`에 보존한다. 이 검사의 iPhone용 빌드는 서명하지 않았으며 실제 아이폰 재설치는 하지 않았다. 내가 직접 한 전체 미션 플레이 결과는 [구현과 검증 범위](implementation-status.md)에 있다.

[빌드 실행 기록](clone/summary.json) · [앱 자원·바이너리 검사](clone/bundle-audit.json) · [Simulator 설치·실행·검사](clone/runtime-summary.json)
