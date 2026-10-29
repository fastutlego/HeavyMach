#!/usr/bin/env python3
"""Exercise actual Android Clock audio focus on the dedicated emulator."""
from pathlib import Path
import argparse,subprocess,json,time,os
p=argparse.ArgumentParser();p.add_argument('--serial',required=True);a=p.parse_args()
if not a.serial.startswith('emulator-'):raise SystemExit('This creates a timer; use the dedicated emulator')
base=[str(Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'))/'platform-tools/adb'),'-s',a.serial];pkg='com.gelb2.heavymach.validation';out=Path(__file__).resolve().parents[1]/'build/evidence'/time.strftime('audio-%Y%m%d-%H%M%S');out.mkdir(parents=True)
def shell(*v):return subprocess.check_output(base+['shell',*map(str,v)],text=True)
def cmd(n,*v):
 args=['am','broadcast','-n',pkg+'/com.gelb2.heavymach.TestReceiver','-a','com.gelb2.heavymach.TEST','--es','command',n]
 for k,x in zip('abcd',v):args+=['--ei',k,str(x)]
 shell(*args)
def dump():shell('run-as',pkg,'rm','-f','files/diagnostics.json');cmd('dump');return json.loads(shell('run-as',pkg,'cat','files/diagnostics.json'))
shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');cmd('cheats',1);cmd('stage',0,0,0,0);time.sleep(.3)
shell('am','start','-W','-a','android.intent.action.SET_TIMER','--ei','android.intent.extra.alarm.LENGTH',6,'--ez','android.intent.extra.alarm.SKIP_UI','true','--es','android.intent.extra.alarm.MESSAGE','HeavyMachAudioTest')
shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');cmd('resume');before=dump();loss={}
try:
 for _ in range(16):
  time.sleep(.5);loss=dump()
  if not loss['audioActive']:break
 (out/'focus-loss.txt').write_text(shell('dumpsys','audio'));(out/'alarm.png').write_bytes(subprocess.check_output(base+['exec-out','screencap','-p']))
finally:
 shell('am','start','-W','-a','android.intent.action.DISMISS_TIMER');shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity')
time.sleep(.5);gain=dump();cmd('resume');cmd('fire',1);time.sleep(2.2);playing=dump();(out/'output-playing.txt').write_text(shell('dumpsys','media.audio_flinger'));cmd('fire',0)
rows=[{'check':'Clock alarm stops audio and pauses game','passed':not loss.get('audioActive',True) and loss.get('paused',False),'before':before,'loss':loss},{'check':'Alarm dismissal restores audio with game paused','passed':gain.get('audioActive',False) and gain.get('paused',False),'gain':gain},{'check':'Resume fires and plays after alarm','passed':playing.get('audioStarts',0)>gain.get('audioStarts',0) and playing.get('shots',0)>gain.get('shots',0),'playing':playing}]
(out/'results.json').write_text(json.dumps(rows,indent=2));print(json.dumps(rows,indent=2));print('EVIDENCE '+str(out));raise SystemExit(0 if all(r['passed'] for r in rows) else 1)
