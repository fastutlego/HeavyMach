# HeavyMach에 기여하기

현재 기능과 남은 과제는 [개발 상태](docs/public/implementation-status.md), 실행 방법은 [빌드·설치](docs/public/build-and-install.md)에 있다.

## 오류 제보

앱 종류(일반·치트), 버전, 기기·OS, 미션과 재현 순서를 적는다. 예상한 동작과 실제 동작을 나누고 가능하면 화면 녹화와 로그를 첨부한다. 저장 파일을 첨부할 때는 계정·기기 식별정보를 확인한다.

## 코드 변경

`develop`에서 작업 브랜치를 만들고 PR의 대상도 `develop`으로 지정한다. `main`에는 같은 저장소의 `develop`만 병합한다.

```sh
git switch develop
git pull --ff-only
git switch -c fix/short-description
git lfs pull
python3 tools/verify_build_resources.py
cmake -S . -B build/core -DCMAKE_BUILD_TYPE=Debug
cmake --build build/core -j 4
ctest --test-dir build/core --output-on-failure
python3 tools/document_contract.py check
python3 tools/check_publication.py
```

변경한 기능의 요구·정책·코드·검사를 함께 확인한다. 기존 미션 배치, 탄속·분산·피격 수치, 저장 시점의 변경은 PR에 전후 차이와 이유를 적는다. 자원을 변경하면 `docs/build-resources.json`과 관련 미술 기록도 갱신한다. 실패한 검사는 로그와 함께 남긴다.

원고는 [개발 배경](docs/developer-background.md)과 [집필 자료](docs/public/publication-inventory.md)를 따른다. 코드·원작 자원·외부 자료의 조건은 [라이선스 범위](LICENSE-SCOPE.md)에서 확인할 수 있다.
