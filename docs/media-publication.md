# 플레이 영상·README 이미지 작업

사용자가 게임 플레이 영상을 제공하면 이 절차로 스크린샷·클립과 README 설명을 만든다. 현재는 영상 수신 전이다. FFmpeg와 ffprobe가 작업 Mac에 설치돼 있음을 확인했다.

## 원본과 장면 기록

원본 영상은 Git 제외 `build/media/<작업명>/original/`에 보존하고 SHA-256을 기록한다. 편집은 `derived/`에 새 파일로 출력한다. ffprobe로 길이·해상도·회전 메타데이터·프레임률·음향 트랙을 확인한다. 본문에 쓰는 앱 종류·기기·버전은 제공 정보와 실제 화면에서 확인하며 모르는 값은 만들어 넣지 않는다.

[장면 명세 예시](media-scenes.example.json)에 원본 해시, 촬영 조건, 시작·종료 시각, 대표 프레임 시각, 관찰한 동작, 캡션, 파생 파일을 기록한다. 예시는 빈 양식이며 실제 영상의 시각이나 결과가 아니다.

영상을 직접 보고 이동·점프·사격·무기 전환·미션 클리어가 드러나는 장면을 고른다. 치트 패널이 있는 장면은 치트 앱이라고 표시한다. 원본·복원 비교 장면은 실제 두 판본의 영상이 있을 때만 만든다. 한 영상으로 수정 전후를 꾸미지 않는다.

## 추출

필요한 도구는 `ffmpeg`, `ffprobe`다. 아래 경로·타임코드는 작업자가 실제 파일과 장면 명세로 바꾼다. `-n`은 기존 출력을 덮어쓰지 않는다.

```sh
ffprobe -v error -show_format -show_streams -of json "$HM_VIDEO" > "$HM_WORK/probe.json"
ffmpeg -n -i "$HM_VIDEO" -ss "$HM_FRAME_TIME" -frames:v 1 "$HM_WORK/derived/gameplay.png"
ffmpeg -n -i "$HM_VIDEO" -ss "$HM_CLIP_START" -t "$HM_CLIP_DURATION" \
  -c:v libx264 -crf 20 -pix_fmt yuv420p -c:a aac -movflags +faststart \
  "$HM_WORK/derived/gameplay.mp4"
```

타임코드는 원본의 재생 시간 기준이다. 원본 비율·게임 진행 속도와 음향을 유지한다. HUD·탄착점·적 이동을 설명할 때 필요한 화면을 자르지 않는다. 회전·HDR·색 변화가 있으면 원본과 추출 프레임을 비교해 수정한다. 긴 영상 전체를 GIF로 바꾸지 않는다. 반복 움직임이 꼭 필요한 짧은 구간만 GIF 후보를 만들고, 용량·글자 가독성을 본 뒤 채택한다.

## README 배치

| 위치 | 자료 | 설명 |
|---|---|---|
| 제목·짧은 소개 아래 | 대표 전투 화면 한 장 또는 짧은 동작 GIF | 어떤 게임인지 바로 보이는 장면 |
| 플레이 소개 | 이동·점프·사격·전환 중 서로 다른 동작의 화면 2~4장 | 이미지마다 한 문장. 실제로 보이는 동작을 쓴다. |
| 일반·치트 앱 설명 | 일반 화면과 검증 패널 화면 | 두 앱의 차이를 실제 화면과 연결 |
| 복원 과정 링크 근처 | 실제 수정 전후가 있는 경우에만 비교 이미지 | 비교 조건·수정 대상을 캡션에 적음 |
| 영상 링크 | 대표 프레임을 누르면 클립으로 이동 | 재생 가능한 링크를 직접 확인 |

이미지는 원본 비율로 표시하고 상세 화면은 원본 크기로 열 수 있게 연결한다. 넓은 화면에서는 필요할 때 2열 표를 쓰며, 좁은 화면에서 HUD를 읽기 어렵다면 한 열로 바꾼다. 배지·장식·문구를 게임 화면 위에 합성하지 않는다. 아직 없는 영상·이미지의 빈 틀을 README에 게시하지 않는다.

GitHub의 [상대 이미지 경로와 Markdown 안내](https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)를 사용한다. 저장소 이미지에는 상대 경로를 쓰고, 영상은 우선 대표 이미지와 클립 링크로 연결한다. 인라인 재생을 적용하면 실제 GitHub 화면에서 재생을 확인한다. GitHub의 [첨부 형식·용량 안내](https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/attaching-files)는 PNG·GIF·MP4 등을 지원하며 H.264를 권장한다. 게시 시점의 제한을 다시 확인한다.

## 파일과 문서 연결

채택된 이미지와 명세는 `docs/public/media/`에 추가한다. PNG·JPEG는 저장소의 Git LFS 설정을 따른다. 클립은 크기와 게시 경로를 확인해 저장하고, 원본 영상 전체를 일반 Git에 추가하지 않는다. 임시 파일 경로나 만료되는 URL을 README의 링크로 쓰지 않는다.

같은 이미지·클립 ID를 README, 이미지 색인, SaaS 집필 안내에서 사용한다. SaaS 담당 세션은 원본 해시·타임코드·촬영 조건과 함께 가져간다. 원본이 개인 로컬 경로에만 있으면 출처 메타데이터와 채택된 파생 파일만으로도 어느 장면인지 확인할 수 있게 한다.

마감 때 원본 해시 보존, 파생 파일 재생·비율·색·오디오, 이미지 링크·대체 텍스트, GitHub의 넓은/좁은 화면, 사이트·다운로드와 캡션 일치를 확인한다. 잘라낸 하이라이트를 전체 미션·성능 검사의 근거로 바꾸어 설명하지 않는다. 작업 브랜치→develop→main 순서로 반영하며 v0.9 태그를 이동하지 않는다.
