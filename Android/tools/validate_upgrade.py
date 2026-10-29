#!/usr/bin/env python3
"""Reproduce pending-EXP mission entry in the cheat app; restore its saved files."""
from pathlib import Path
import argparse,json,os,struct,subprocess,time
p=argparse.ArgumentParser();p.add_argument('--serial',required=True);p.add_argument('--save-file',type=Path,required=True);p.add_argument('--slot',type=int,default=1);a=p.parse_args()
pkg='com.gelb2.heavymach.validation';root=Path(__file__).resolve().parents[2];out=root/'Android/build/evidence'/time.strftime('upgrade-%Y%m%d-%H%M%S');out.mkdir(parents=True)
b=[str(Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'))/'platform-tools/adb'),'-s',a.serial];results=[]
def raw(*v):return subprocess.check_output(b+list(map(str,v)))
def shell(*v):return raw('shell',*v).decode()
def cmd(n,*v):
 args=['am','broadcast','-n',pkg+'/com.gelb2.heavymach.TestReceiver','-a','com.gelb2.heavymach.TEST','--es','command',n]
 for k,x in zip('abcd',v):args+=['--ei',k,str(x)]
 shell(*args)
def dump():shell('run-as',pkg,'rm','-f','files/diagnostics.json');cmd('dump');return json.loads(raw('exec-out','run-as',pkg,'cat','files/diagnostics.json'))
def check(n,ok,**e):
 results.append({'check':n,'passed':bool(ok),**e});(out/'results.json').write_text(json.dumps(results,indent=2));print(n+': '+('PASS' if ok else 'FAIL'),flush=True)
def capture(n):(out/(n+'.png')).write_bytes(raw('exec-out','screencap','-p'))
def tap(x,y):
 state=dump();cw,ch=state['canvas'];png=raw('exec-out','screencap','-p');w,h=struct.unpack('>II',png[16:24]);shell('input','tap',round((x+(cw-480)/2)*w/cw),round((ch-y-(ch-320)/2)*h/ch))
def frozen(n):
 x=dump();time.sleep(.7);y=dump();check(n,y['paused'] and x['worldTime']==y['worldTime'] and x['hp']==y['hp'] and x['x']==y['x'],before=x,after=y);return y
shell('am','force-stop',pkg);backup=raw('exec-out','run-as',pkg,'tar','-cf','-','files');(out/'cheat-files-before.tar').write_bytes(backup)
data=bytearray(a.save_file.read_bytes());assert len(data)==3*0x4b8 and 0<=a.slot<3;data[:0x4b8]=data[a.slot*0x4b8:(a.slot+1)*0x4b8]
try:
 subprocess.run(b+['shell','run-as',pkg,'sh','-c','"cat > files/HvM.dat"'],input=data,check=True)
 shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');time.sleep(1);cmd('saved');time.sleep(1);state=frozen('pending EXP entry freezes combat');capture('entry')
 check('pending EXP levels once and keeps total',state['upgradeStep']==1 and state['level']==4 and state['experience']==2415 and not state['gameMenu'],state=state)
 cmd('key',7,1);cmd('fire',1);cmd('jump');time.sleep(.3);cmd('key',7,0);cmd('fire',0);cmd('resume');frozen('combat input and Resume cannot bypass upgrade')
 shell('input','keyevent','KEYCODE_HOME');time.sleep(.3);shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');time.sleep(.6);state=frozen('background return preserves upgrade without menu overlap');check('upgrade owns modal',not state['gameMenu'] and state['upgradeStep']==1,state=state)
 tap(139,172);tap(294,95);time.sleep(.8);state=dump();check('touch selection and OK apply one movement upgrade',state['grades']==[2,1,1] and state['upgradeStep']==0 and not state['paused'],state=state);capture('confirmed');before=state;time.sleep(.5);after=dump();check('combat resumes after panel closes',after['worldTime']>before['worldTime'],before=before,after=after)
 # A fresh earned level uses the same modal, including interruption while closing.
 cmd('exp',2000);time.sleep(.6);state=frozen('earned EXP upgrade freezes combat');tap(240,172);tap(294,95);cmd('focus',0);time.sleep(.8);cmd('focus',1);state=frozen('focus loss during close stays paused');check('closed upgrade hands interruption to Resume menu',state['upgradeStep']==0 and state['gameMenu'] and state['grades']==[2,2,1],state=state)
 cmd('resume');time.sleep(.4);state=dump();check('Resume after interrupted close advances combat',not state['paused'] and not state['gameMenu'],state=state)
finally:
 shell('am','force-stop',pkg)
 subprocess.run(b+['shell','run-as',pkg,'tar','-xf','-'],input=backup,check=True)
 restored=raw('exec-out','run-as',pkg,'cat','files/HvM.dat')
 import tarfile,io
 original=tarfile.open(fileobj=io.BytesIO(backup)).extractfile('files/HvM.dat').read()
 check('original cheat save restored',restored==original)
 shell('input','keyevent','KEYCODE_HOME')
print('EVIDENCE '+str(out));raise SystemExit(0 if all(r['passed'] for r in results) else 1)
