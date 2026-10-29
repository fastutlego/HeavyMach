#!/usr/bin/env python3
"""Emulator accelerometer → Android rotation mapping → tank movement."""
from pathlib import Path
import argparse,subprocess,json,time,os
p=argparse.ArgumentParser();p.add_argument('--serial',required=True);a=p.parse_args()
if not a.serial.startswith('emulator-'):raise SystemExit('Requires emulator sensor controls')
b=[str(Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'))/'platform-tools/adb'),'-s',a.serial];pkg='com.gelb2.heavymach.validation';out=Path(__file__).resolve().parents[1]/'build/evidence'/time.strftime('tilt-%Y%m%d-%H%M%S');out.mkdir(parents=True);rows=[]
def shell(*v):return subprocess.check_output(b+['shell',*map(str,v)],text=True)
def sensor(y):subprocess.run(b+['emu','sensor','set','acceleration',f'0:{y}:9.3'],check=True,stdout=subprocess.DEVNULL)
def cmd(n,*v):
 args=['am','broadcast','-n',pkg+'/com.gelb2.heavymach.TestReceiver','-a','com.gelb2.heavymach.TEST','--es','command',n]
 for k,x in zip('abcd',v):args+=['--ei',k,str(x)]
 shell(*args)
def dump():shell('run-as',pkg,'rm','-f','files/diagnostics.json');cmd('dump');return json.loads(shell('run-as',pkg,'cat','files/diagnostics.json'))
rot=shell('settings','get','system','user_rotation').strip();auto=shell('settings','get','system','accelerometer_rotation').strip()
try:
 shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');shell('settings','put','system','accelerometer_rotation',0);cmd('fixtureSettings',0,0,0,0);cmd('cheats',1)
 for rotation,sign in [(1,1),(3,-1)]:
  sensor(0);shell('settings','put','system','user_rotation',rotation);time.sleep(.6);cmd('stage',0,0,0,0);time.sleep(.3)
  for direction in [1,-1]:
   before=dump();sensor(sign*direction*3);time.sleep(1.5);after=dump();sensor(0);delta=after['x']-before['x'];rows.append({'rotation':rotation,'direction':direction,'delta_x':delta,'passed':delta*direction>5,'before':before,'after':after});time.sleep(.3)
finally:
 sensor(0);shell('settings','put','system','user_rotation',rot);shell('settings','put','system','accelerometer_rotation',auto);cmd('pause')
(out/'results.json').write_text(json.dumps(rows,indent=2));print(json.dumps(rows,indent=2));print('EVIDENCE '+str(out));raise SystemExit(0 if all(r['passed'] for r in rows) else 1)
