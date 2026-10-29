#!/usr/bin/env python3
"""On-device HUD, EGL and wide/tall display checks; uses only the cheat package."""
from pathlib import Path
import argparse, subprocess, json, time, os
p=argparse.ArgumentParser();p.add_argument('--serial',required=True);p.add_argument('--hud-only',action='store_true');a=p.parse_args()
ADB=str(Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'))/'platform-tools/adb');base=[ADB,'-s',a.serial];pkg='com.gelb2.heavymach.validation'
out=Path(__file__).resolve().parents[1]/'build/evidence'/time.strftime('platform-%Y%m%d-%H%M%S');out.mkdir(parents=True);results=[]
def shell(*v):return subprocess.check_output(base+['shell',*map(str,v)],text=True)
def cmd(n,*v):
 args=['am','broadcast','-n',pkg+'/com.gelb2.heavymach.TestReceiver','-a','com.gelb2.heavymach.TEST','--es','command',n]
 for k,x in zip('abcd',v):args+=['--ei',k,str(x)]
 shell(*args)
def dump():
 shell('run-as',pkg,'rm','-f','files/diagnostics.json');cmd('dump');return json.loads(shell('run-as',pkg,'cat','files/diagnostics.json'))
def check(n,ok,**data):
 row={'check':n,'passed':bool(ok),**data};results.append(row);print(json.dumps(row),flush=True);(out/'results.json').write_text(json.dumps(results,indent=2))
def capture(n):(out/(n+'.png')).write_bytes(subprocess.check_output(base+['exec-out','screencap','-p']))
def hud():
 r=subprocess.run(base+['exec-out','run-as',pkg,'cat','files/HUDLayoutV1.txt'],capture_output=True);return r.stdout if r.returncode==0 else b''
def launch():shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');time.sleep(.7)
# Run on the dedicated landscape emulator at 2340x1080. Never change a physical screen size.
if not a.hud_only and not a.serial.startswith('emulator-'):raise SystemExit('Use the dedicated emulator for display override checks')
launch()
if not a.hud_only:shell('wm','size','reset')
time.sleep(.5)
raw=subprocess.check_output(base+['exec-out','screencap','-p'])
import struct
if struct.unpack('>II',raw[16:24])!=(2340,1080):raise SystemExit('HUD coordinate test requires 2340x1080; use normalized positions for other devices')
cmd('cancel');cmd('fixtureSettings',0,0,0,0);cmd('cheats',1);cmd('stage',0,0,0,0);cmd('pause');cmd('editor');cmd('reset');cmd('apply');cmd('editor');time.sleep(.3)
b=dump();saved=hud();shell('input','swipe',475,965,150,600,700);shell('input','swipe',1194,972,1194,650,700);d=dump();capture('hud-draft')
check('HUD drag freezes world and does not save',d['editor'] and d['worldTime']==b['worldTime'] and d['hudWrites']==b['hudWrites'] and hud()==saved,before=b,after=d)
cmd('sample');time.sleep(.3);capture('hud-sample');e=dump();check('sample scene preserves live world',e['worldTime']==b['worldTime'] and e['x']==b['x'] and e['hp']==b['hp'],state=e)
cmd('sample');cmd('apply');time.sleep(.2);ap=dump();content=hud();check('Apply saves moved HUD once',not ap['editor'] and ap['hudWrites']==b['hudWrites']+1 and content!=saved,state=ap)
cmd('resume');time.sleep(.7);b=dump();shell('input','tap',150,600);time.sleep(.15);d=dump();check('moved Jump responds at new position',d['y']>b['y']+5,before=b,after=d);capture('moved-jump')
time.sleep(1);b=dump();shell('input','tap',475,965);time.sleep(.15);d=dump();check('old Jump position no longer jumps',abs(d['y']-b['y'])<2,before=b,after=d)
cmd('pause');cmd('editor');shell('input','swipe',150,600,300,400,500);cmd('cancel');check('Cancel retains applied file',hud()==content)
shell('am','force-stop',pkg);launch();cmd('saved');cmd('resume');time.sleep(.6);b=dump();shell('input','tap',150,600);time.sleep(.15);d=dump();check('HUD persists across process restart',hud()==content and d['y']>b['y']+5,before=b,after=d)
cmd('pause');cmd('editor');cmd('reset');cmd('apply');check('Reset restores default positions',all(line.startswith('0 ') for line in hud().decode().splitlines()[1:]))
# Explicitly destroy/rebuild EGL through the same recovery helper used after EGL_CONTEXT_LOST.
cmd('resume');time.sleep(.3);cmd('pause');b=dump();cmd('recreateContext');time.sleep(.5);d=dump();check('GPU rebuild retains paused world',d['contexts']==b['contexts']+1 and d['uploads']>b['uploads'] and d['x']==b['x'] and d['worldTime']==b['worldTime'] and d['paused'] and not d['error'],before=b,after=d);capture('context-restored')
if a.hud_only:
 print('EVIDENCE '+str(out),flush=True);raise SystemExit(0 if all(r['passed'] for r in results) else 1)
rotation=shell('settings','get','system','user_rotation').strip();auto=shell('settings','get','system','accelerometer_rotation').strip()
try:
 shell('settings','put','system','accelerometer_rotation',0)
 for r in (1,3):
  shell('settings','put','system','user_rotation',r);time.sleep(.8);d=dump();check('landscape rotation '+str(r),not d['error'] and d['canvas'][0]>d['canvas'][1] and d['worldTime']==b['worldTime'],state=d);capture('rotation-'+str(r))
 shell('wm','size','1536x2048');time.sleep(1);d=dump();check('tablet aspect extends canvas height',d['canvas'][1]>320 and d['canvas'][0]>=480 and not d['error'],state=d);capture('tablet-paused');cmd('editor');time.sleep(.3);capture('tablet-editor');cmd('cancel')
finally:
 shell('wm','size','reset');shell('settings','put','system','user_rotation',rotation);shell('settings','put','system','accelerometer_rotation',auto)
time.sleep(.5);cmd('resume');time.sleep(.4);d=dump();check('resume after surface changes',d['worldTime']>b['worldTime'] and not d['paused'] and not d['error'],state=d)
(out/'logcat.txt').write_text(shell('logcat','-d','-s','HeavyMach','AndroidRuntime'));print('EVIDENCE '+str(out),flush=True)
raise SystemExit(0 if all(r['passed'] for r in results) else 1)
