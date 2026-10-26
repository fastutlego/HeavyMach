---
name: apple-wireless-device
description: HeavyMach iPhone·iPad를 Xcode와 무선으로 연결해 빌드·설치·LLDB·XCTest·Instruments를 검증하거나 연결 실패를 진단할 때 사용한다.
---

# Apple 실제 기기 무선 작업

프로젝트의 `docs/apple-platforms.md`와 `docs/wireless-device-workflow.md`를 읽는다. 빌드는 Mac에서 수행하고 설치·실행·디버깅을 네트워크로 연결한다. 실제 사용 가능한 Xcode·devicectl·CUA를 우선한다. 외부 스킬의 도구 이름만으로 설치·연결돼 있다고 판단하지 않는다.

- `xcrun devicectl list devices`에서 실제 기기를 고르고 `device info details`의 `connectionProperties.transportType`을 설치 전후 기록한다. `localNetwork`일 때만 무선으로 보고한다. `wired`인 상태에서 기기 이름·IP를 지정한 것만으로 무선 검증이라고 쓰지 않는다.
- 기기 UDID, 네트워크 주소, 개인 기기 이름은 로컬 원시 자료에만 보존한다. 게시 자료에는 모델·OS·transport·실행 결과·원시 파일 해시를 쓴다.
- `HeavyMachRestored`는 일반 앱, `HeavyMachValidation`은 치트 앱이다. 기존 앱을 지우지 않고 업데이트 설치하며 일반 저장을 보존한다. 기능 검사는 별도 저장을 쓰는 치트 앱으로 수행한다.
- 기존 페어링·개발자 모드·서명이 유효하면 재설정하지 않는다. 연결 실패는 기기 잠금, 네트워크 transport, 페어링, 개발 서비스, 서명, 테스트 묶음 순으로 실제 오류와 대조한다. 재부팅 직후에는 최초 잠금 해제가 필요할 수 있다.
- CLI 서명이 `errSecInternalComponent`로 실패하면 같은 스킴·기기를 Xcode에서 빌드해 본다. 이 환경에서는 기존 키체인 권한으로 GUI 빌드가 성공했다. 비밀을 출력하거나 임시 서명으로 대체하지 않는다.
- Xcode 스킴/대상 팝업은 검색란을 클릭하고 `typeText`로 이름을 입력한 뒤 선택된 행을 Return으로 확정한다. 이 환경의 AX `setValue`와 행 클릭만으로는 선택이 적용되지 않은 사례가 있다. 툴바의 실제 스킴·대상을 확인한 뒤 실행한다.
- LLDB는 실행 중단, 게임 함수의 one-shot 중단점, 변수 조회, 재개를 확인한다. 콘솔 자동 완성에 Return이 소비될 수 있으므로 명령별 결과와 새 프롬프트를 확인한다. Debug navigator의 변수·호출 스택도 증거로 사용할 수 있다.
- `test-without-building` 전에 `.app/PlugIns/*.xctest`와 해당 `.xctestrun`을 확인한다. Xcode Run이 앱을 다시 만들면 테스트 묶음이 사라질 수 있다. 묶음 생성 실패는 게임 검사 실패와 구분하고 Build For Testing 후 재실행한다.
- 프레임 성능은 debugger/Instruments 부착 중 수치와 구분한다. 기능 확인용 짧은 무선 trace를 장시간 발열·누수 증명으로 쓰지 않는다.
- 연결·설치·실행·디버거·자동 검사·직접 조작을 각각 기록한다. 기존 실패한 게임 검사를 무선 연결 성공으로 덮지 않는다.
