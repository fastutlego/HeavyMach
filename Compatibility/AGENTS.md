# 호환 실행판 작업

상위 AGENTS.md와 이 폴더의 README.md·upstream.json을 읽는다. 원본 ARMv6 게임과 ARM64 호스트를 구별한다. 원본 IPA·저장과 사용자 변경을 보존한다. 호환 실행기 변경은 patches/heavymach.patch, SDL 바인딩 변경은 patches/rust-sdl2.patch에 기록한다. 고정 upstream과 패치를 대조하며 기존 checkout을 reset하지 않는다. MoleWorld 전용 패치·치트를 활성화하지 않는다. 서명 키·프로파일·계정·IPA·DerivedData·Rust target은 커밋하지 않는다. 빌드·서명·설치·실행·플레이 결과를 각각 기록한다.

분리 전 상위 지침 원문은 ../docs/migration/legacy-ios/에 보존했다. 현행 경로와 브랜치는 상위 저장소 관리 문서를 따른다.
