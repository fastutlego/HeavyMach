# GitHub 구성과 v0.12 소스 릴리스

README와 개발자의 변 첫머리에 같은 도입문을 넣었다. 꿈에서 아이팟터치를 처음 쓰던 나를 보았다는 이야기, Mesabloo보다 시작점이 나았다는 설명, 원작에서 벗어난 Codex의 구현을 지적하며 고친 경험을 담았다. [README](../../README.md) · [개발자의 변](../RESTORATION.md)

## 저장소 소개와 기여 안내

About를 현재 iPhone·iPad·Mac 앱과 공용 C++ 코어·입력·자동조준 소개로 바꿨다. 장르와 실제 구현을 나타내는 Topics 10개를 등록했다. [기여 안내](../../CONTRIBUTING.md), 버그·기능 제안 양식과 PR 양식을 추가했다. 개발 PR은 `develop`을 대상으로 한다.

공유 카드에는 확인받은 플레이 캡처를 비율 그대로 배치하고 게임 이름과 플랫폼을 붙였다. 1280×640 PNG, 772,558바이트다. 원본 캡처는 바꾸지 않았다. GitHub는 이미지를 등록한 적 없는 비공개 저장소에 최초 업로드를 제공하지 않는다. 설정 화면에서도 Social preview 항목이 없었다. 저장소를 공개할 때 이 파일을 등록한다. [GitHub 공식 안내](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/customizing-your-repositorys-social-media-preview)

![GitHub 공유 이미지 후보](media/github-social-preview.png)

## 자동 검사

| 작업 | 실행 내용 | 현재 결과 |
|---|---|---|
| Documentation and resources | 문서 연결, 게시 원고의 로컬 링크, 두 도입문 일치, 보존 원문 해시, 자원 1,482개 | 로컬 통과. GitHub 실행은 계정 결제·사용 한도로 시작 전 차단 |
| Core and Apple builds | CMake·CTest, arm64 Simulator 일반·치트, native Mac 일반·치트 빌드 | 압축본에서 로컬 통과. GitHub 실행은 같은 계정 제한으로 차단 |
| Branch flow | main PR의 출발 브랜치가 같은 저장소의 develop인지 확인 | 기존 규칙 유지 |

문서만 바꾼 커밋은 Apple 빌드를 시작하지 않는다. 코드·자원·프로젝트·빌드 workflow를 바꾸면 빌드 작업이 실행된다. GitHub 빌드 성공 배지는 아직 추가하지 않았다. [문서 실행](https://github.com/gelb2/HeavyMach/actions/runs/40542759494) · [빌드 실행](https://github.com/gelb2/HeavyMach/actions/runs/40542759541)

GitHub가 반환한 사유는 계정의 최근 결제 실패 또는 사용 한도다. 워크플로 코드는 실행되지 않았다. 결제·한도를 해결한 뒤 **Actions → Core and Apple builds → Run workflow**에서 `main`을 선택하면 다시 검사할 수 있다. macOS 작업은 [GitHub의 arm64 macos-15 runner](https://docs.github.com/en/actions/reference/runners/github-hosted-runners)를 사용한다.

## v0.12 소스와 자원

`v0.12`는 `eedf245f859a5224adf8f1c4a4b010eda1b5a5bb`를 가리킨다. 태그를 옮기지 않고 해당 커밋의 추적 파일 2,366개를 묶었다. 이미지·음원은 LFS 포인터 대신 실제 파일을 넣었다.

| 검사 | 결과 |
|---|---|
| 압축본 | `HeavyMach-v0.12-source-with-assets.tar.gz`, 146,187,445바이트 |
| SHA-256 | `06ef5e0f0fb31e7fe4f716293030a9d76dee4d3d0c45ac0d56f7ed4f672d5296` |
| 자원 | 1,482개 크기·해시 일치 |
| 공용 코어 | 21개 통과, 실패 0개 |
| Simulator | HeavyMachRestored·HeavyMachValidation 빌드 통과 |
| Mac | HeavyMachMac·HeavyMachMacValidation 빌드 통과 |
| 도구 | Apple Silicon, Xcode 27.1, Debug·서명 없음 |

검사는 `.git`과 기존 빌드 결과가 없는 새 폴더에 압축을 풀어 실행했다. 이번 결과는 컴파일·링크 검사다. 기기 플레이 결과는 [기존 검증 기록](validation.md)에 있다. [릴리스 검사 근거](../evidence/source-release-v0.12-20261028.json)

## OpenAL 수정 후 재링크

같은 압축본의 OpenAL 1.22.2 버전 문자열에 검사 표식을 넣고 라이브러리와 앱을 다시 빌드했다. iOS Simulator와 Mac 앱의 `debug.dylib`에서 표식을 확인했다. 검사가 끝난 뒤 소스의 버전 문자열을 원래대로 복구했다. Xcode Debug 앱은 실행 진입 파일과 실제 앱 코드가 들어가는 dylib가 나뉘므로 후자도 검사한다.

새 폴더에 푼 소스와 현재 main의 검사 도구로 재현할 수 있다.

```sh
python3 tools/verify_openal_relink.py --source /path/to/unpacked/HeavyMach-v0.12
```

OpenAL 소스는 압축본의 `vendor/openal-soft`에 있고, 빌드 설정은 `tools/build_openal.sh`에 있다. 소스를 수정한 라이브러리로 다시 링크할 수 있음을 확인했다. [재링크 결과](../evidence/openal-relink-20261028.json) · [LGPL 원문](../../LICENSES/OpenAL-Soft-LGPL-2.0.txt)

## 공개 준비

[자원 목록](../rights-manifest.json)에 원작 그림·음원·데이터, 추가 그림, 캡처·영상의 출처와 확보할 공개 범위를 정리했다. 현재 자료에는 원작 자원의 공개·재배포 허락 문건이 없다. 제작자에게 새 메시지를 보내지는 않았다.

외부 구현의 참조 커밋과 실제 파일을 대조하고 touchHLE MPL-2.0, rust-sdl2 MIT 원문을 보관했다. `HMCompatibilityPixels.hpp`에는 MPL-2.0을 표시했다. [외부 코드 고지](../../THIRD_PARTY_NOTICES.md)

Git 전체 참조의 94개 커밋을 Gitleaks로 검사해 탐지된 비밀값은 0건이었다. 추적 파일 중 개인 키·인증서·프로비저닝 파일 확장자는 없었다. 로컬 경로·개발 팀 ID가 있는 문서·설정은 49개이며 [파일 목록](../evidence/publication-source-audit-20261028.json)에 남겼다. 과거 기록과 Git 이력은 보존했다. 저장소의 공개 범위는 비공개다.

남은 일은 GitHub Actions 계정 제한 해소 후 실행, 공유 이미지 등록, 원작·참조 그림의 공개 범위 확정이다. 앱 설치 파일의 일반 배포에는 플랫폼별 서명·설치 검사가 추가로 필요하다.

[전체 문서](../README.md) · [GitHub 구성 계획](github-repository-plan-20261028.md) · [빌드·설치](build-and-install.md)
