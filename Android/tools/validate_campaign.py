#!/usr/bin/env python3
"""Load every campaign stage, then exercise boss auto-aim using the real core."""
from pathlib import Path
import argparse,subprocess,json,time,os
p=argparse.ArgumentParser();p.add_argument('--serial',required=True);a=p.parse_args();base=[str(Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'))/'platform-tools/adb'),'-s',a.serial];pkg='com.gelb2.heavymach.validation';out=Path(__file__).resolve().parents[1]/'build/evidence'/time.strftime('campaign-%Y%m%d-%H%M%S');out.mkdir(parents=True);rows=[]
def shell(*v):return subprocess.check_output(base+['shell',*map(str,v)],text=True)
def cmd(n,*v):
 args=['am','broadcast','-n',pkg+'/com.gelb2.heavymach.TestReceiver','-a','com.gelb2.heavymach.TEST','--es','command',n]
 for k,x in zip('abcd',v):args+=['--ei',k,str(x)]
 shell(*args)
def dump():shell('run-as',pkg,'rm','-f','files/diagnostics.json');cmd('dump');return json.loads(shell('run-as',pkg,'cat','files/diagnostics.json'))
def check(n,ok,**data):
 r={'check':n,'passed':bool(ok),**data};rows.append(r);print(json.dumps(r),flush=True);(out/'results.json').write_text(json.dumps(rows,indent=2))
def capture(n):(out/(n+'.png')).write_bytes(subprocess.check_output(base+['exec-out','screencap','-p']))
shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');time.sleep(.5);cmd('cancel');cmd('fixtureSettings',0,0,0,0);cmd('cheats',1)
for m,count in enumerate([4,5,5,5,5,5,6]):
 for stage in range(count):
  cmd('stage',m,stage,20,0);time.sleep(.3);state=dump();check(f'campaign {m+1}-{stage+1} starts',state.get('mission')==m and state.get('stage')==stage and state.get('worldTime',0)>0 and not state.get('error'),state=state)
  if stage==0:capture(f'mission-{m+1}')
cmd('fixtureSettings',6,6,6,2);cmd('stage',2,4,99,4);cmd('aim',1);time.sleep(.5);b=dump();check('automatic aim acquires boss-stage enemy',b.get('aim')==2 and b.get('aimTarget',0)!=0,state=b);cmd('fire',1);time.sleep(5);d=dump();cmd('fire',0);check('auto-aim fires original weapon',d.get('shots',0)>b.get('shots',0) and not d.get('error'),before=b,after=d);capture('boss-auto-aim')
# Go through Android's actual KeyEvent adapter, not the diagnostic key command.
cmd('aim',0);cmd('fixtureSettings',0,0,0,0);cmd('stage',0,0,0,0);time.sleep(.3);b=dump();shell('input','keyevent','--longpress','KEYCODE_F');time.sleep(.2);d=dump();check('Android F key fires',d.get('shots',0)>b.get('shots',0),before=b,after=d)
b=dump();shell('input','keyevent','KEYCODE_E');d=dump();check('Android E key changes weapon',d.get('weapon')!=b.get('weapon'),before=b,after=d)
cmd('pause');(out/'memory.txt').write_text(shell('dumpsys','meminfo',pkg));print('EVIDENCE '+str(out),flush=True);raise SystemExit(0 if all(r['passed'] for r in rows) else 1)
