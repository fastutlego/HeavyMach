# HeavyMach 안드로이드 APK 개발 계획

2026-10-26 조사·계획. 현재 정상 플레이가 확인된 HeavyMach 1의 C++ 게임 코어를 그대로 빌드하고, iOS에 묶인 화면·입력·음향·저장 연결을 Android로 옮긴다. **게임 규칙을 다시 작성할 필요는 없다.** 이번 조사에서는 코어 20개 C++ 파일을 수정 없이 Android ARM64 정적 라이브러리로 컴파일했다. APK와 Android 플레이는 아직 미구현이다.

## 현재 코드에서 확인한 범위

기준은 HeavyMach main `7c73e06fe5405a880226e554ef8de0174b7238f0`과 정상 플레이 기준 태그 `v0.9`다. 현재 앱은 C++20·Objective-C++로 구현돼 있다. `Sources/Core`의 전투·수치·진행·저장 바이트 해석을 재사용한다. `Sources/iOS`의 Objective-C++ 9개 파일, 총 2,320줄은 플랫폼 API와 메뉴·프레임 조립이 섞인 범위다. 줄 수를 개발 기간으로 환산하지 않는다.

| 현재 코드 | 재사용 또는 이식 방법 |
|---|---|
| Sources/Core, CMakeLists.txt | 같은 C++ 파일을 NDK로 컴파일. Android용 게임 규칙 복사본을 만들지 않음 |
| HMGLRenderer.mm | OpenGL ES 1.1 그리기 순서를 유지. EAGL/CAEAGLLayer를 EGL/ANativeWindow로 교체 |
| HMGameView.mm | 타이머·센서·저장·화면 수명과 게임 진행 연결을 분리. 순서·난수·정지 조건을 먼저 기록 |
| HMMenus.mm, HMGameHUD.mm | 게임 UI 상태·레이아웃·버튼 동작을 공통 C++로 옮기고 GL로 표시. Android 전용 메뉴 규칙을 새로 만들지 않음 |
| HMMedia.mm, HMCompatibilityPixels.hpp, vendor/stb | 동일 디코더·POT·색·UV 처리. NSBundle/NSData/UIImage 보관 부분을 Android 자원 읽기와 C++ 메타데이터로 교체 |
| HMOpenALAudio.mm, AudioContract.hpp | OpenAL Soft 1.22.2와 source 재시작·거리 모델 유지. Objective-C 래퍼만 C++화하고 Android 출력 backend 연결 |
| HMValidationPanel/Controls | 치트 APK에 조건 설정·이동·점프·발사·무기 전환 제공. 일반 APK에는 컴파일하지 않음 |
| Resources, Art | 원본·채택 확장 에셋과 파일명·해시 유지. 빌드 때 로컬 원본에서 APK assets를 준비 |

메뉴와 프레임 조립은 코어 밖에도 있다. 따라서 정적 라이브러리 성공만으로 앱 대부분이 완성됐다고 보지 않는다. 공통화할 때마다 같은 iOS 경로의 회귀 검사를 통과시켜 기존 정상판을 보존한다.

## Android 구성안

HeavyMach 저장소 안에 `Android/` Gradle 프로젝트를 두고 `Sources/Core`를 직접 참조한다. `Sources/Platform`에는 공통 프레임 조립·메뉴·자원·음향을, `Sources/Android`에는 JNI·EGL·센서·파일·생명주기를 둘 계획이다. 이 경로들은 아직 생성하지 않았다. iOS와 Android가 같은 게임 코드를 소비하게 하고 허브 아래 별도 게임으로 분리하지 않는다.

앱 진입은 얇은 Java Activity와 SurfaceView, 게임 실행은 하나의 native thread를 기본안으로 한다. JNI는 수명·입력·화면 정보 전달에 한정하고 sprite마다 호출하지 않는다. 입력·센서는 timestamp와 함께 큐로 전달하고 게임 상태·GL context는 지정한 스레드만 변경한다. Gradle의 externalNativeBuild로 CMake와 shared library를 묶을 수 있다. [공식 CMake 연결 안내](https://developer.android.com/studio/projects/gradle-external-native-builds).

GameActivity도 후보지만 초기 UI·센서·회전 연결에 필요한 범위부터 비교한다. 초기 단계에서 Activity 방식과 함께 검토하고 하나로 고정한다. 두 실행 루프를 동시에 유지하지 않는다. [공식 GameActivity 안내](https://developer.android.com/games/agdk/game-activity).

첫 대상은 minSdk 26, arm64-v8a 휴대폰으로 제안한다. 이는 조사용 코어 컴파일 조건이며 지원 하한을 확정한 출시 약속은 아니다. x86_64는 필요 시 에뮬레이터용으로 추가한다. 일반 `com.gelb2.heavymach.restored`, 치트 `com.gelb2.heavymach.validation` 패키지를 제안하며 두 앱의 데이터 디렉터리를 분리한다.

## 그래픽·프레임·입력

Android NDK는 GLES 1.x와 `libGLESv1_CM`, EGL을 제공한다. 따라서 첫 기준판은 현재 GLES 1.1 경로를 유지할 수 있다. 다만 `OES_framebuffer_object`, RGBA8 renderbuffer 등 실제 사용 확장과 EGLConfig를 기기에서 확인해야 한다. 지원이 부족하면 먼저 해당 GL 호출·픽셀 차이를 기록한다. ES 2/3 셰이더나 Vulkan 전환은 구체적인 호환 실패와 별도 비교 후 결정한다. [NDK 그래픽 API](https://developer.android.com/ndk/guides/stable_apis).

논리 W×320, 원본 비교 480×320, sprite blend·GL_REPLACE·RGB565 변환·LINEAR/REPEAT와 최종 LINEAR/CLAMP_TO_EDGE를 유지한다. Android Surface에는 검증된 같은 최종 quad를 표시한다. 게임 프레임을 Canvas/Bitmap/Compose로 매번 합성하지 않는다. Surface 소멸과 context 소실을 구분하고, GPU 자원은 원본에서 재생성하되 진행 상태를 초기화하지 않는다.

현재의 한 논리 프레임당 `0.01666666753590107` 증가·한 번 tick/draw·별도 50Hz 센서 정책을 유지한다. 표시 동기화는 Choreographer/EGL을 조사하고, 90/120Hz에서 게임이 1.5/2배 진행하지 않게 60Hz 논리 프레임만 선택한다. 놓친 프레임이나 백그라운드 시간을 몰아서 tick하지 않는다. Swappy는 표시 간격을 다루는 후보이며 게임 시간 누적기를 추가하는 이유로 쓰지 않는다. 실제 60/90/120Hz callback·tick·present 수를 대조한 뒤 적용한다. [Frame Pacing](https://developer.android.com/games/sdk/frame-pacing).

터치 좌표는 Surface 실제 픽셀→현재 viewport→월드/HUD 변환을 한 번만 적용한다. dp·상태바·cutout·가로 회전·멀티터치 pointer ID와 CANCEL을 다룬다. 방향 버튼은 치트판에서 실제 입력 경로를 호출한다. SensorManager의 가속도는 m/s²이므로 g 단위로 변환하고 기기 기본 방향과 현재 display rotation으로 축을 바꾼다. 센서 축은 OS 회전으로 자동 변경되지 않으며 태블릿의 기본 방향을 세로로 가정할 수 없다. [센서 좌표 안내](https://developer.android.com/develop/sensors-and-location/sensors/sensors_overview).

양쪽 가로 방향만 플레이하도록 `userLandscape`를 기본 후보로 둔다. OS 자동 회전 잠금도 존중한다. Android 16의 큰 화면에서는 방향 제한이 무시될 수 있으므로 manifest만으로 보장하지 않는다. portrait/multi-window/fold 상태가 강제되면 진행을 정지하고 가로 게임 영역과 안내를 유지하는 별도 처리를 검사한다. 접이식 아래 조작부는 첫 APK 이후 별도 UI 과제다. [Activity 방향·큰 화면 동작](https://developer.android.com/guide/topics/manifest/activity-element).

## 음향·저장·중단과 복귀

현재 vendor OpenAL Soft에는 Android OpenSL·Oboe backend 조건이 있다. 첫 동등성 검사에는 기존 1.22.2 소스를 NDK로 빌드하고 OpenSL backend를 우선 확인한다. 출력 지연·기기 문제로 Oboe가 필요하면 backend만 비교한다. 31개 WAV의 buffer/source를 준비해 재사용하고 source 위치→stop→play 순서를 지킨다. 무기 전환음·동시 효과음·볼륨·출력장치 전환은 실물 스피커로 확인한다.

`HvM.dat`의 3,624바이트 형식과 정상 저장 이벤트를 보존하고 앱 내부 filesDir에 원자 저장한다. 사격·피격·EXP 변화·매 프레임·background 진입에 파일 쓰기를 추가하지 않는다. 프로세스가 종료되면 마지막 정상 저장에서 이어간다. OS 복원용 임시 상태를 영속 저장 규칙과 혼동하지 않는다. iPhone 저장 가져오기는 별도 요청이 있을 때 백업·해시 대조 후 처리한다.

onPause·focus 상실·Surface 파괴·오디오 focus 상실에 대한 상태 전이를 정한다. 게임·터치·센서·음향을 멈추고 복귀하면 정지 메뉴에서 명시 재개한다. background에서 GL 호출을 하지 않고 과거 터치·소리를 재생하지 않는다. foreground 120초 무입력 정책도 유지한다. Activity 재생성·프로세스 종료·GL context 복구를 서로 다른 검사로 둔다.

## 빌드 도구와 이번 실행 결과

로컬에는 JDK 17.0.14, SDK 34/35/36/36.1, NDK 26.3.11579264, CMake 3.22.1, adb·Emulator와 API 36 AVD들이 있다. `/Applications/Android Studio.app`는 없었다. 다른 설치 위치까지 전수 조사한 것은 아니다. CLI 빌드는 가능하며 조사 시 adb에 연결된 기기는 없었다.

실제 실행한 명령은 다음과 같다. 성공 산출물은 ARM64 정적 라이브러리이며 APK가 아니다.

```sh
cmake -S . -B build/research-20261026/android-core-probe \
  -DCMAKE_TOOLCHAIN_FILE=/Users/sokol/Library/Android/sdk/ndk/26.3.11579264/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-26 \
  -DCMAKE_BUILD_TYPE=Release -DHM_BUILD_VALIDATION_TESTS=OFF
cmake --build build/research-20261026/android-core-probe --target HeavyMachCore -j 4
```

20개 소스가 컴파일됐고 `GlobalInfo.cpp.o`는 AArch64 ELF였다. GameSession의 필드 초기화 경고 9개와 NDK CMake 최소 버전 폐기 예고는 남았다. 조사 때문에 소스를 변경하지 않았다. Android에서 코어 검사를 실행하거나 OpenAL·EGL을 빌드한 것은 아니다. [입력 커밋·도구·산출물 해시](https://github.com/gelb2/HeavyMach/blob/main/docs/research/android-and-titles-evidence-20261026.json).

APK용 도구는 조사용 NDK와 구분한다. 첫 구성안은 AGP 8.13.2·Gradle 8.13·JDK 17·compile/targetSdk 36, NDK r28 이상 중 설치한 한 버전 고정이다. AGP 공식 호환표가 API 36.1·Gradle 8.13·JDK 17을 지원한다. 새 도구 설치와 Gradle dependency resolution은 아직 실행하지 않았다. [AGP 호환표](https://developer.android.com/build/releases/agp-8-13-0-release-notes).

16KB 페이지 환경을 초기 APK 기준에 포함한다. NDK r28 이상과 AGP 8.5.1 이상을 쓰고 OpenAL·C++ runtime 등 모든 native library의 ELF/ZIP 정렬을 확인한다. 이번 NDK r26 정적 라이브러리 성공은 그 증거가 아니다. [16KB 빌드·검사 안내](https://developer.android.google.cn/guide/practices/page-sizes?hl=en).

## 구현 순서와 완료 조건

| 단계 | 만들 것 | 다음 단계로 넘어갈 근거 |
|---|---|---|
| A0 조사 | 원본/현재 코드 대조, 코어 NDK 컴파일, 개발 계획 | 이번에 완료. Android 실행 검증은 제외 |
| A1 앱 뼈대 | Android Gradle·JNI·Surface·EGL·일반/치트 variant, assets 준비 | 두 debug APK 설치·콜드 실행, GL vendor/config·자원 해시 기록 |
| A2 공통 화면 | 메뉴·HUD·게임 프레임 조립 추출, GLES renderer·미디어 연결 | 타이틀→슬롯→모드→미션→전투. iOS 동일 장면 회귀 통과 |
| A3 입력·실행 | 틸트·점프·사격·전체 무기·음향·저장·pause/resume | 치트 전 무기 검사, 양방향 터치/실제 틸트, 저장 횟수·바이트 비교 |
| A4 회귀·성능 | 전체 미션·Survival, 화면비·context 소실·120Hz·16KB | 차이 목록 해결, 에뮬레이터와 실기기 결과 분리, 장시간 trace |
| A5 APK 인계 | 일반 release·치트 검증 APK, 재현 가능한 빌드·서명·해시 | 일반판 치트 코드 제외, 서명 확인, 업데이트 저장 유지, 설치·플레이 기록 |

공통 부분 추출은 A2에서 작은 단위로 진행한다. 기존 iOS 일반/치트 빌드와 관련 core/UI 검사를 함께 확인하고, 차이가 나면 API 교체·좌표·tick 순서 중 어느 변경에서 생겼는지 좁힌다. 전체 구현 기간은 아직 산정하지 않는다. A1의 EGL/OpenAL 실기기 결과가 있어야 A2 이후의 비용을 판단할 수 있다.

개발 브랜치는 `feat/android-bootstrap`, `feat/android-renderer`처럼 develop에서 나눈다. 작업 브랜치→develop→main을 지키고 v0.9를 이동하지 않는다. 새 버전 태그는 Android 완료 상태와 확인 범위가 정해진 뒤 부여한다.

## APK 생성·설치 절차

아래는 **Android 프로젝트를 구현한 뒤 사용할 예정 명령**이다. 현재 저장소에 gradlew나 이 task가 존재한다는 뜻은 아니다.

```sh
cd Android
./gradlew :app:assembleNormalDebug :app:assembleCheatDebug
./gradlew :app:connectedCheatDebugAndroidTest
./gradlew :app:assembleNormalRelease
apksigner verify --verbose --print-certs app/build/outputs/apk/normal/release/app-normal-release.apk
zipalign -c -P 16 -v 4 app/build/outputs/apk/normal/release/app-normal-release.apk
adb -s DEVICE_SERIAL install -r app/build/outputs/apk/cheat/debug/app-cheat-debug.apk
```

출력명은 실제 variant 구성 뒤 task·산출물과 맞춘다. Debug와 release 서명 키를 구분하고 release 키를 Git에 넣지 않는다. 키는 새 앱 업데이트에도 같은 키를 쓸 수 있도록 보존한다. 패키지명·versionCode·commit·ABI·에셋 해시·APK SHA-256·서명 인증서 지문을 인계 기록에 남긴다. APK 빌드·서명·설치·정상 플레이를 각각 판정한다. [Gradle APK 생성](https://developer.android.com/build/building-cmdline), [apksigner 검사](https://developer.android.com/tools/apksigner).

## 검증 계획

치트판에서 모든 플레이어 무기와 세 필살기를 실제 데이터로 열거한다. 기본 이동·점프·사격·전환·연사·탄약·피격·아이템·EXP·업그레이드·보스·미션 전환·결과 일러스트·저장 재실행을 검사한다. 화면 양 끝·탱크 후방·미션 3-5 항공기·원본480/확장폭을 회귀 조건에 포함한다. 동일 seed와 입력·프레임 수로 iOS/Android 상태·이벤트·저장 바이트를 비교하고 부동소수 차이는 발생 지점과 영향부터 조사한다.

에뮬레이터는 자동 조건 재현·경계 터치·회전·Activity 재생성·저장·메모리 오류를 검사한다. 실기기는 센서 방향·스피커·GPU driver·60/90/120Hz·발열·입력 지연을 확인한다. 에뮬레이터 GPU 옵션이나 CPU 제한을 특정 휴대폰 성능으로 환산하지 않는다.

프레임 계측에는 callback, simulation tick, CPU draw, EGL swap, 실제 표시 간격을 따로 남긴다. 초기 열 상태를 맞춰 고정 장면·연사·보스·메뉴 왕복을 20~30분 반복하는 검사를 제안한다. 평균·p95·p99·최대·16.67ms/33.3ms 초과 프레임 수와 메모리·파일 쓰기·이미지 디코딩 횟수를 기록한다. 짧은 Perfetto trace 구간을 함께 확보한다. Native Surface의 GPU 비용을 일반 View의 `gfxinfo`만으로 판정하지 않는다. [Android system tracing](https://developer.android.com/topic/performance/tracing), [Perfetto 기록](https://perfetto.dev/docs/getting-started/system-tracing).

## 사용할 스킬과 첫 작업

이 프로젝트의 `port-heavymach-android` 스킬을 추가했다. 계획·기준·완료 조건을 먼저 읽고 해당 단계만 실행하도록 한다. 오류 재현은 engineering-debug, 의미 있는 회귀 검사는 engineering-test, 원본 동작이 불명확할 때만 reverse-engineer-anything을 사용한다. 중요한 공통 코드 변경 검토에는 engineering-review, 보고에는 write-project-text와 present-game-artifacts/Sites를 적용한다.

다음 개발 작업은 A1이다. 도구 버전을 고정하고 원본 에셋을 검증한 뒤 EGL·OpenAL을 연결한 치트 debug APK부터 실행한다. 게임 동작을 이식하기 전에 논리 버퍼·최종 표시·입력·음향의 기준을 확보한다. 이 문서 작성만으로 Android 개발 완료나 APK 생성 완료를 표시하지 않는다.
