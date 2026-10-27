#!/usr/bin/env python3
"""Extract the documented gameplay candidates. Originals stay in ignored build/media.
Usage: python3 tools/extract_gameplay_media.py
Requires ffmpeg/ffprobe on PATH. Existing outputs are verified and kept.
"""
import hashlib,json,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
DEST=ROOT/'docs/public/media/gameplay-20261027'
WORK=ROOT/'build/media/user-gameplay-20261027'
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def run(args,log):
    with log.open('w') as f: subprocess.run(args,stdout=f,stderr=subprocess.STDOUT,check=True)
def main():
    manifest=DEST/'media-scenes.json'; data=json.loads(manifest.read_text())
    sources={s['id']:s for s in data['sources']}
    for src in sources.values():
        p=WORK/'original'/src['filename']
        assert sha(p)==src['sha256'], f'Original hash mismatch: {p.name}'
    for scene in data['scenes']:
        src=WORK/'original'/sources[scene['source_id']]['filename']
        outputs=[]
        for name in scene['outputs']:
            dest=DEST/name; log=WORK/'review'/(dest.stem+('.log' if dest.suffix=='.png' else '-clip.log'))
            if not dest.exists():
                if dest.suffix=='.png':
                    args=['ffmpeg','-hide_banner','-n','-ss',str(scene['poster_seconds']),'-copyts','-i',str(src),'-an','-vf','showinfo','-frames:v','1','-fps_mode','passthrough',str(dest)]
                else:
                    args=['ffmpeg','-hide_banner','-n','-ss',str(scene['start_seconds']),'-i',str(src),'-t',str(scene['end_seconds']-scene['start_seconds']),'-map','0:v:0','-map','0:a:0','-c:v','libx264','-preset','fast','-crf','22','-threads','4','-vf','scale=in_range=full:out_range=limited,setsar=1','-pix_fmt','yuv420p','-color_range','tv','-colorspace','bt709','-color_primaries','bt709','-color_trc','bt709','-fps_mode','vfr','-c:a','aac','-b:a','128k','-movflags','+faststart',str(dest)]
                run(args,log)
            if dest.suffix=='.png':
                matches=re.findall(r'\bn:\s*0\s+pts:\s*\d+\s+pts_time:([\d.]+)',log.read_text())
                assert matches, f'Missing source PTS: {name}'
                scene['poster_actual_pts_seconds']=float(matches[0])
                assert 0 <= scene['poster_actual_pts_seconds']-scene['poster_seconds'] < .05
            probe=json.loads(subprocess.check_output(['ffprobe','-v','error','-show_streams','-show_format','-of','json',str(dest)]))
            video=next(x for x in probe['streams'] if x['codec_type']=='video')
            assert (video['width'],video['height'])==(2736,1260)
            item=dict(path=name,sha256=sha(dest),bytes=dest.stat().st_size,width=video['width'],height=video['height'],codec=video['codec_name'])
            if dest.suffix=='.mp4':
                item['duration_seconds']=float(probe['format']['duration'])
                assert abs(item['duration_seconds']-(scene['end_seconds']-scene['start_seconds']))<.1
                assert any(x['codec_type']=='audio' for x in probe['streams'])
                run(['ffmpeg','-v','error','-i',str(dest),'-f','null','-'],WORK/'review'/(dest.stem+'-decode.log'))
                item['audio_codec']='aac';item['full_decode']='passed'
            outputs.append(item)
        scene['output_metadata']=outputs
        print(scene['id'], 'verified',flush=True)
    manifest.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n')
if __name__=='__main__': main()
