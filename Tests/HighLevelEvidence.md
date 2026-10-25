# GlobalInfo 고레벨 정수 변환 근거

2026-10-24. 읽기 전용 분석 결과다. GlobalInfo 구현은 이 작업에서 수정하지 않았다.

## 결론

`thresholds()`의 양수 double 범위 초과는 예외나 int32 wrap이 아니라 **INT32_MAX 포화**가 원본 helper의 결과다. 내부 level66에서 처음 포화하고, level67..99의 previous/next는 모두2147483647이다. 다음 레벨 구간이0이 되는 원본의 이상 동작도 존재한다. 이를 임의의 새 성장곡선으로 교체하면 원본 규칙을 바꾼다.

## 근거 연결

1. 로컬 `Analysis/functions/0x105f8.pseudo.c`와 `.asm`: GlobalInfo calcLevelUpExp는 누적값+300×pow 계산 결과에 `___fixdfsivfp`를 호출한다. 일반 `__fixdfsi`를 가정하면 안 된다. 수학 연산은 double, 성장 base의 감소는 float다.
2. [LLVM compiler-rt Darwin helper](https://raw.githubusercontent.com/llvm/llvm-project/main/compiler-rt/lib/builtins/arm/fixdfsivfp.S)는 GPR double를 VFP에 옮기고 `vcvt.s32.f64` 후 결과를 r0로 돌려준다. 이 소스의 함수 설명과 명령이 round-toward-zero 변환임을 확인한다.
3. 로컬 호환 실행에 실제 사용한 `.../touchHLE/touchHLE_dylibs/libgcc_s.1.dylib`의 armv6 export `___fixdfsivfp`는0x2ba0이다. `xcrun otool -arch armv6 -tvV` 결과4개 명령 word는 `ec410b17 eefd7bc7 ee170a90 e12fff1e`다. 각각 double 이동, signed double→integer 변환, r0 이동, bx lr 경로다. 이 파일은 사용자가 2009년 당시 iPhone에서 쓴 OS dylib의 사본이라는 주장은 하지 않는다.
4. [ARM VFP11 Technical Reference Manual DDI0274H, 2007, §5.6/Table5-5, p.5-17](https://documentation-service.arm.com/static/5e8e227c88295d1e18d377ac): FTOSID/FTOSIS에서 양수 범위 초과/+Inf는0x7fffffff, 음수 범위 초과/−Inf는0x80000000, NaN은0이다. 따라서 ARMv6 시대 VFP에 직접 해당하는 결과 근거가 있다. 기본 예외 처리 상태를 전제로 한다. FPSCR IOC는 이식 코어가 관찰하지 않으므로 값만 재현한다.
5. 기존 호환 엔진 `src/cpu/interpreter/vfp.rs::fp_to_i32`도 NaN0/양수최대/음수최소/절삭을 명시한다. 이는 보조 일치 근거이며 하드웨어 매뉴얼 대신으로 사용하지 않았다.

## 제안 변환 함수

```cpp
int32_t originalFixDfsivfp(double value) {
    if (std::isnan(value)) return 0;
    if (value >= 2147483647.0) return INT32_MAX;
    if (value <= -2147483648.0) return INT32_MIN;
    return static_cast<int32_t>(value); // Range-safe, truncates toward zero.
}
```

2147483647.9도 절삭 후 INT32_MAX이므로 위 경계는 반환값을 정확히 보존한다. 캐스트 후 범위를 검사하면 C++ UB가 이미 발생하므로 반드시 먼저 검사한다.

## 실행한 계산 probe

`/tmp/hm-highlevel.cpp`에서 위 함수로 원본 루프를 계산했다. `clang++ -std=c++20 -ffp-contract=off -fno-fast-math` 빌드 후 `/tmp/hm-highlevel` exit0. 이것은 **복원식의 native 실행**이며 원본 ARM 명령을 직접 실행한 검사라고 주장하지 않는다. 현재 macOS `pow`를 소비하므로 libm의 마지막 비트까지 원본 iPhone과 검증한 것은 아니다. 다만 레벨66 초과량은 약1.24억으로 포화 경계와 충분히 떨어져 있다.

| 내부 level | previous | next | next−previous |
|---:|---:|---:|---:|
|63|960837471|1191349127|230511656|
|64|1191349127|1477183637|285834510|
|65|1477183637|1831618501|354434864|
|66|1831618501|2147483647|315865146|
|67|2147483647|2147483647|0|
|68|2147483647|2147483647|0|
|99|2147483647|2147483647|0|

level66 포화 전 raw double=2271117820.6131124. base는 반복 float 감소 때문에1.2400002479553223이며, 수학적1.25로 고정하면 앞선 레벨도 바뀐다.

변환 독립 test vectors:

| 입력 | int32 결과 |
|---|---:|
|2147483646.9|2147483646|
|2147483647.0|2147483647|
|2147483647.9|2147483647|
|2147483648.0|2147483647|
|−2147483649.0|−2147483648|
|+Inf|2147483647|
|−Inf|−2147483648|
|NaN|0|

## 함께 고쳐야 할 경계

`GlobalInfo addExp 0x106e0`에서 경험치 합산은 ARM32 정수 add이고 helper 변환이 아니다. 따라서 해당 합산에는 포화함수를 쓰면 안 된다. uint32 add 후 bit_cast<int32_t>로 wrap을 보존한다. 예: INT32_MAX+1→INT32_MIN. 현재 checkedInt로 막으면 원본과 다른 예외가 발생한다.

레벨66에서 EXP=INT32_MAX, addExp(0)이면 한 번만 레벨67로 증가하고 포인트 바이트도1 증가한다. 새 threshold 구간0, numerator0이므로 gauge=NaN이다. 원본은 gauge>1일 때만1로 바꾸며 NaN을0으로 바꾸지 않는다. 레벨67에서 EXP=INT32_MAX−1, addExp(0)이면 레벨업하지 않고 numerator−1/denominator0이므로 −Inf다. EXP 합산이 넘어가는 경우 numerator 계산도 원본32bit subtraction을 별도로 보존해야 한다. float gauge의 NaN/Inf를 UI에서 처리할지는 표시 계층의 명시적 방어로 구분한다.

Player의 EXP 아이템은 level67..99에서 필요구간0을 지급한다. 이는 원본 포화의 결과이며 자동으로 다음 레벨의 큰 경험치를 생성해서는 안 된다. level99는 원본 상한으로 레벨업 조건이 false다. `maximumHP` 등 다른 checkedInt 소비자에 일괄 변경을 적용하지 말고, 각 원본 helper와 정수 명령을 대조해야 한다.

## 후속 구현 보고

2026-10-24 부모 통합 담당자의 실행 보고: GlobalInfo thresholds에 포화 변환, addExperience 합산과 gauge numerator에32bit wrap을 적용했다. SaveData는 level≥67의 비유한 gauge를 원본 가능값으로 허용한다. HMMedia는 비유한 transform의 draw만 건너뛰며 저장값을 바꾸지 않는다. CoreTests의 level65/66/67/99, INT32_MAX에서 level67/NaN, +1 wrap 사례가 통과했다. 이는 통합 담당자가 실행한 결과이며, 이 문서 작성자가 별도 재실행한 결과로 표기하지 않는다.
