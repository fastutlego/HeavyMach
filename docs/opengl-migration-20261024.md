# OpenGL 복원·시뮬레이터 성능 검증 · 2026-10-24

사용자 요청에 따라 정상 IPA 호환판의 기술을 기준으로 소스 복원판의 화면·음향 경로를 수정했다. Metal로 전환하지 않았다. 사용자 원본·자원·저장·Core 게임 규칙과 Unity 작업은 변경하지 않았다. 새 커밋·push·실기기 설치는 하지 않았다.

## 실행 결과

Xcode27.1, iPhone Air/iOS26.5 Simulator, Apple M4 Pro. 최종 XCTest34/34 통과 (`build/opengl-migration/complete.log`, `complete.xcresult`). 일반 iPhone Debug 빌드 서명제외 성공 (`final-device-build.log`). 실기기 실행 성공을 뜻하지 않는다.

각 장면30프레임 준비 뒤180프레임, 실제 CADisplayLink와 화면 표시 및 음향을 포함했다. 각 장면3초 정도의 짧은 검사이며 장시간·전체 미션 성능은 미검증이다.

| 장면 | 프레임 처리 평균 ms | 처리 p95 ms | 평균 콜백 Hz | 최대 콜백 간격 ms |
|---|---:|---:|---:|---:|
|title_animation|9.391|9.765|60.00|16.911|
|slot_preview|11.242|12.352|59.99|16.981|
|stage01_firing|11.969|12.477|60.00|16.685|

프레임 처리 시간과 화면 콜백 간격은 별도 수치다. 콜백 지연이 있으면 이를 무시하고 무드랍으로 보고하지 않는다. 콜백 Hz는 실기기 화면 출력 FPS가 아니다. 첫30프레임 비용·개별 최초 오프스크린 프레임 비용도 complete-metrics.json에 보존했다.

## 원인과 변경

1. 기존 화면 경로는 매 프레임 CPU 래스터·행 뒤집기·이미지 생성·합성을 반복했다. 현재 일반 앱은 CAEAGLLayer에 OpenGL ES1.1 명령으로 직접 표시한다. 원본의 POT 텍스처·RGB565·LINEAR/REPEAT/REPLACE·혼합 계수·변환 순서·궤적을 유지하고480×320에서 합성한 뒤 확대한다. CPU 경로와 snapshot readback은 비교 검사에만 사용한다.
2. 그래픽 변경 뒤에도 사격 지연이 남았다. AVAudioPlayer 음향 재시작 호출은 평균52.887ms/p9558.549ms, 전체 사격 프레임 p9565.845ms였다(`audio-isolation.log`, 실패 보존). 정상 호환판의 수정 없는 OpenAL Soft1.22.2 소스를 vendor에 보존하고31개 WAV를 고정 buffer/source로 사전 준비하도록 바꿨다. 재생마다 버퍼를 다시 만들지 않는다. sourceX·거리감쇠도 원본 규칙을 사용한다.
최종 사격 음향 호출 평균0.007053ms, p95 0.008167ms.

OpenAL Soft는 호환 실행기의 음향 구현 소스를 재사용했다. 그래픽은 원본 GLES 호출/상태 규칙을 복원한 새 소스이며 호환 실행기의 전체 그래픽 브리지를 복사했다고 주장하지 않는다. 이전 Apple OpenAL 시도에서 다중 장치·컨텍스트/offset 검사 실패가 있었고 기록을 보존했다. 최종 앱은 Apple OpenAL framework를 연결하지 않는다.

## 검증 범위

- 기존 실제 CGL 기준8개 사례: 투명도·가산 합성·회전/반전·축소·atlas 경계·RGB565·trail. 고정 원본 샘플에서 채널 오차1 이내. 별도 기존 alpha probe 검사는 정확 일치. CPU 결과를 새 기대값으로 만들지 않았다.
- 실제 UIWindow 표시, 원래 CADisplayLink, 사격으로 생긴 탄환·실제 음향 재생, 비활성화 후 그리기 차단·재개.
- source/buffer31개 사전 준비, ID 재사용·sample offset 재시작, 실제 CoreAudio 출력 backend, 컨텍스트 복원, 잘못된 RIFF 거부.
- synthetic interruption notification으로 음향 중단·재개 검사. 실제 전화/알람 검증은 아니다.
- 독립 검토에서 발견한 GPU background drain·오디오 세션 재활성화·interruption 메인 큐 직렬화 결함을 수정했다.

## Xcode 성능 제어 조사와 한계

실제 조회한 GL_RENDERER는 **Apple Software Renderer**다. 이 시뮬레이터의 OpenGL ES1.1 측정은 아이폰 GPU 측정이 아니다. Mac에는 M4 Pro GPU 하나가 있다. Device Hub의 File/Device/Settings 메뉴와 simctl io/ui에는 특정 iPhone CPU/GPU 성능을 맞추는 설정이 없었다. Instruments Game Performance의 실제 recording-options 조회에는 Time Profiler/Hangs/Points of Interest만 나타났다. 이 작업에서 Instruments trace 자체를 수집한 것은 아니다.

Apple은 Simulator가 Mac 자원을 사용하고 실제 iOS GPU 에뮬레이터가 아니라고 설명한다. Metal GPU 선택·성능 상태 제어를 이 OpenGL software renderer의 아이폰 성능 재현 기능으로 사용할 근거는 없다. GPU Performance State와 Thermal device condition은 연결된 장치 조건 검사 기능이다.

- [Apple Simulator와 GPU 설명](https://developer.apple.com/videos/play/wwdc2019/418/)
- [GPU Performance State 설명](https://developer.apple.com/videos/play/wwdc2021/10157/)
- [기기 조건 적용](https://help.apple.com/xcode/mac/current/en.lproj/devdf21fe239.html)

따라서 이번 검증은 시뮬레이터에서 기존 병목을 재현하고 동일 경로의 개선과 프레임 예산을 확인한 결과다. 아이폰의 정확한 FPS·발열·전력·GPU 시간은 미확인이다. 정상 IPA 호환판의 실기기 결과는 사용자 확인으로 보존한다. 이번에 호환판의 동일 장면 FPS를 시뮬레이터에서 새로 계측하지 않았다.

## 재현

```sh
xcodegen generate --spec HeavyMach/iOS/project.yml
xcodebuild -project HeavyMach/iOS/HeavyMachRestored.xcodeproj -scheme HeavyMachValidation -destination 'platform=iOS Simulator,id=97CA5646-36BB-47A9-919A-D78DD632A439' -derivedDataPath /tmp/heavymach-opengl -resultBundlePath /tmp/hm-new-result.xcresult test
```

OpenAL은 Xcode prebuild에서 `tools/build_openal.sh <sdk>`로 보존 vendor 소스를 빌드한다. arm64 Simulator/iphoneos 출력은 build/openal 아래에 분리한다. 앱만 정적 라이브러리를 연결하고 테스트는 HM_VALIDATION bridge로 같은 인스턴스를 검사한다. 라이선스·출처는 vendor/openal-soft/HEAVYMACH-PROVENANCE.txt에 있다. 기기 ID는 현재 simctl 목록으로 확인하며 결과 경로는 기존 실패를 덮지 않는 새 경로를 사용한다.

기존 실패·중간 결과와 최종 소스 해시는 build/opengl-migration/manifest.json 및 각 xcresult/log에 보존한다. 최종 수정은 미커밋 상태다.
