# REA 사용 가능 여부 — 2026-10-26

현재 Codex 세션에서 REA를 이용한 헤비멕 분석은 실행된다. 원본 실행 파일을 열고 경험치 함수의 디컴파일과 어셈블리 조회를 실제 호출해 성공했다. 정책 거부·승인 차단·연결 실패는 발생하지 않았다. 이 프로젝트의 분석과 소스 복원 작업을 계속할 수 있다.

## 실제 확인

- 설치 스킬: reverse-engineer-anything, metadata version 42. 파일 SHA-256 `fbcd39d6abe803cf7aa49c1880815bc8e22ba2af87a23681944f687bb7629eb1`. .agents와 .codex의 사본이 같다.
- 설치 CLI: rea-agents 6.7.0. 현재 세션에 REA MCP 도구 207개가 노출된다.
- 원본: HeavyMach 1.13 ARMv6 Mach-O. SHA-256 `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`.
- `open_binary`에 원본 경로와 provider_id=ghidra를 전달했다. 기존 분석 snapshot을 입력하지 않았다.
- `procedure_pseudo_code(0x106e0)` 성공. Ghidra 12.1.4가 `GlobalInfo::addExp`의 의사 코드를 반환했다. Evidence `ev_ee96914bf20181a47dc856df63729955f699db45e5ddb8aa51ab6b7607bb7e34`.
- `procedure_assembly(0x106e0)` 성공. 123줄의 반환 문자열을 받았다. Evidence `ev_c6927c60df737146ff6313cc9b46327d77e498aba78720c70c468f8169c7a2be`.
- 분석 뒤 `close_binary`가 성공했고 원본 실행 파일 해시는 그대로다. 설정·승인 정책·분석 엔진을 변경하지 않았다.

반환된 원본의 경험치 누적, 레벨 99 미만 조건, 호출당 한 번의 레벨 증가, 강화 포인트 증가, 경험치 게이지 계산을 기존 `Sources/Core/GlobalInfo.cpp`의 `addExperience`와 대조했다. 이번 확인을 위해 정상 게임 코드를 바꾸지 않았다. 이 함수의 의사 코드는 원본 개발 소스와 구분한다.

원시 응답은 Git 제외 `build/rea-availability-20261026/pseudo.json`과 `assembly.json`에 보존했다. 이번 결과는 현재 계정·세션·지정한 원본에서 분석이 실행됐다는 증거다.

## 도구 연결과 정책의 구분

현행 REA 스킬은 네이티브 디컴파일·비교·복원을 사용 범위로 안내하며, 사용자의 조사 범위에서 제공 도구를 직접 호출하도록 한다. 도구 누락, 엔진 미설치, 클라이언트 승인 거부는 별도 원인으로 구분한다.

[OpenAI 공식 MCP 문서](https://learn.chatgpt.com/docs/extend/mcp)는 Codex의 외부 도구 연결과 서버·도구별 승인 설정을 설명한다. 이 일반 연결 문서만으로 모든 역공학 요청의 허용 여부를 단정하지 않았다. 이번 헤비멕 요청은 실제 도구 호출 결과로 확인했다. Claude에서 발생한 거부의 원인은 이번 검사에서 확인하지 않았다.
