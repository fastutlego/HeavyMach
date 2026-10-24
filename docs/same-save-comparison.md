# 동일 저장으로 원본·복원 슬롯 비교

2026-10-24, iPhone Air 시뮬레이터 iOS26.5. 물리 기기 조작 없이 진행했다.

## 입력과 격리

검증 앱의 실제 EXP+1 → LV02 → 방어 강화 → SAVE & EXIT로 만든 저장을 사용했다. 시작 조건은 EXP299, 부품5/5/5 검증 fixture였다. 저장 결과는 EXP300, 내부level1, body5/top6/weapon5다. 파일3624bytes, SHA256 `2d91364a880fcf1edd7d1ea7591ed92fa4d3e5a9bd94b44a235f19141e83960e`.

기존 호환 실행 앱을 `/tmp/heavymach-original-comparison/HeavyMachReference.app`으로 복사하고, 바깥 앱의 bundle ID·표시 이름만 `com.gelb2.heavymach.reference` / HM Original Compare로 바꿔 ad-hoc 서명했다. 원본 ARMv6 게임 코드와 자산을 변경하지 않았다. 원본 비교 앱의 별도 Documents sandbox에만 검증 저장을 복사했다. 기존 일반 복원 앱과 기존 호환 앱의 저장은 덮어쓰지 않았다.

## 직접 확인

원본 실행본의 타이틀 → 슬롯 화면과, 최신 복원 검증 앱의 재설치·콜드 실행 → 슬롯 화면을 CUA로 관측했다. 두 앱 모두 첫 슬롯 LV02, 이동06·방어07·공격06을 표시했다. 강화한 기체의 차체·상부·무기 형태도 두 슬롯 미리보기에서 확인했다. 복원 저장을 원본이 읽는 경로와 복원 앱의 재실행 후 강화 상태 유지를 확인한 결과다.

원본 실행본과 복원 앱의 화면 표시 크기는 다르다. 호환 실행본의 프레임과 복원 앱의 안전 영역 반영 차이를 포함한다. 이미지 전체 픽셀 일치·프레임별 움직임·모든 강화 조합·음향 동등성은 이번 관측으로 확인하지 않았다.

## 증거

- `build/original-comparison-app.json`: 비교 앱 복사·식별 기록.
- `build/original-comparison-save.json`: 복사 입력·목적지·해시.
- `build/original-comparison-console.log`: 원본 비교 실행 로그.
- `build/original-same-save-slot.png`: CUA 원본 슬롯 화면.
- `build/native-same-save-slot.png`: CUA 복원 슬롯 화면.
- `build/offscreen-bullet-simulator-build.log`: 이번 복원 앱 빌드.
