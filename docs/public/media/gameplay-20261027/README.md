# 플레이 이미지·GIF·영상

README에 채택한 이미지 8장, GIF 2개, 소리 있는 MP4 3개다. Codex가 내가 제공한 두 녹화본에서 추출했다. PNG는 원본 회전을 반영한 2736×1260이며 화면 전체를 보존했다.

[프로젝트 소개](../../../../README.md) · [장면 명세](media-scenes.json)

| 장면 | 이미지 | GIF | 영상·원본 구간 |
|---|---|---|---|
| 자동조준 | [01](01-auto-aim.png) | [5초](01-auto-aim.gif), 원본 V2 9.5~14.5초 | [7초 MP4](01-auto-aim.mp4), V2 9~16초 |
| 점프·사격 | [02](02-jump.png) | — | V1 56.003333초 프레임 |
| 무기 목록 | [03](03-weapon.png) | — | V1 44.015초 프레임 |
| 공중 지원 | [04](04-special.png) | — | V1 164.075초 프레임 |
| 강화 | [05](05-upgrade.png) | — | V1 112.04초 프레임 |
| 결과 그림 | [06](06-stage-clear.png) | — | V1 172.061667초 프레임 |
| 추가 전투기 | [07](07-fighters.png) | — | [13초 MP4](07-fighters.mp4), V2 90~103초 |
| 점프·착지 | [08](08-jump-clip.png) | [6초](08-jump-clip.gif), 원본 V2 37~43초 | [7초 MP4](08-jump-clip.mp4), V2 37~44초 |

GIF는 768×354, 12fps, 원래 재생 속도다. 해상도·색 수를 줄인 미리보기이며 소리는 MP4에 있다. PNG·MP4·GIF의 SHA-256과 캡션·대체 텍스트는 명세에 있다.

원본 V1은 10월 25일, V2는 10월 27일 파일이다. 촬영 기종·앱 종류·빌드 번호는 미확인이다. 원본은 Git에서 제외한 `build/media/user-gameplay-20261027/original/`에 보관했고 Downloads 파일도 유지했다.

## 다시 추출하기

```sh
python3 tools/extract_gameplay_media.py
python3 tools/make_readme_gifs.py
```

첫 명령은 원본 두 파일과 FFmpeg·ffprobe로 PNG·MP4를 추출한다. 두 번째는 저장소의 MP4로 GIF를 만든다. 기존 출력은 덮어쓰지 않고 해시·크기·디코딩을 확인한다. 영상 원본은 앱 빌드에 사용하지 않는다.
