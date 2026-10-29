#!/usr/bin/env python3
"""Record real combat frame delivery, memory and thermal status on one device."""
from pathlib import Path
import argparse,subprocess,json,time,os,re,math
p=argparse.ArgumentParser();p.add_argument('--serial',required=True);p.add_argument('--seconds',type=int,default=45);a=p.parse_args();base=[str(Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'))/'platform-tools/adb'),'-s',a.serial];pkg='com.gelb2.heavymach.validation';out=Path(__file__).resolve().parents[1]/'build/evidence'/time.strftime('performance-%Y%m%d-%H%M%S');out.mkdir(parents=True)
def shell(*v):return subprocess.check_output(base+['shell',*map(str,v)],text=True)
def cmd(n,*v):
 args=['am','broadcast','-n',pkg+'/com.gelb2.heavymach.TestReceiver','-a','com.gelb2.heavymach.TEST','--es','command',n]
 for k,x in zip('abcd',v):args+=['--ei',k,str(x)]
 shell(*args)
def dump():shell('run-as',pkg,'rm','-f','files/diagnostics.json');cmd('dump');return json.loads(shell('run-as',pkg,'cat','files/diagnostics.json'))
def battery():return '\n'.join(x for x in shell('dumpsys','battery').splitlines() if re.match(r'^  (level|temperature|status|USB powered):',x))
shell('am','start','-W','-n',pkg+'/com.gelb2.heavymach.GameActivity');time.sleep(.5);cmd('cheats',1);cmd('fixtureSettings',6,6,6,2);cmd('stage',2,4,60,0);cmd('aim',1);cmd('fire',1);time.sleep(3)
(out/'memory-before.txt').write_text(shell('dumpsys','meminfo',pkg));(out/'battery-before.txt').write_text(battery());(out/'thermal-before.txt').write_text(shell('dumpsys','thermalservice'))
shell('dumpsys','SurfaceFlinger','--timestats','-clear','-enable');t0=time.monotonic();before=dump();start=(t0+time.monotonic())/2
trace='/data/misc/perfetto-traces/hm-'+time.strftime('%Y%m%d-%H%M%S')+'.perfetto-trace';traceLog=(out/'perfetto.log').open('w')
proc=subprocess.Popen(base+['shell','perfetto','-t','15s','sched','freq','idle','am','wm','gfx','view','-a',pkg,'-o',trace],stdout=traceLog,stderr=subprocess.STDOUT)
time.sleep(a.seconds);t1=time.monotonic();after=dump();elapsed=(t1+time.monotonic())/2-start;stats=shell('dumpsys','SurfaceFlinger','--timestats','-dump');shell('dumpsys','SurfaceFlinger','--timestats','-disable');cmd('fire',0);cmd('pause')
(out/'surfaceflinger.txt').write_text(stats);(out/'memory-after.txt').write_text(shell('dumpsys','meminfo',pkg));(out/'battery-after.txt').write_text(battery());(out/'thermal-after.txt').write_text(shell('dumpsys','thermalservice'));(out/'combat.png').write_bytes(subprocess.check_output(base+['exec-out','screencap','-p']))
proc.wait(timeout=10);traceLog.close()
if proc.returncode==0:subprocess.run(base+['pull',trace,str(out/'combat.perfetto-trace')],check=True,stdout=subprocess.DEVNULL);shell('rm',trace)
layers=[]
for block in stats.split('layerName = ')[1:]:
 if 'SurfaceView['+pkg not in block.splitlines()[0]:continue
 hist=re.search(r'present2present histogram is as below:\n([^\n]+)',block);pairs=[(int(ms),int(n)) for ms,n in re.findall(r'(\d+)ms=(\d+)',hist[1])] if hist else [];total=sum(n for _,n in pairs)
 def percentile(q):
  t=0
  for ms,n in pairs:
   t+=n
   if t>=math.ceil(total*q):return ms
 layers.append({'layer':block.splitlines()[0],'histogram_frames':total,'p90_bucket_ms':percentile(.9),'p99_bucket_ms':percentile(.99),'max_nonempty_bucket_ms':max((ms for ms,n in pairs if n),default=0),'over_33ms':sum(n for ms,n in pairs if ms>33)})
result={'elapsed_seconds':elapsed,'engine_fps':(after['frames']-before['frames'])/elapsed,'before':before,'after':after,'layers':layers,'perfetto_recorded':proc.returncode==0,'scope':'one Galaxy, warmed boss-stage combat; histogram buckets are integer milliseconds'}
(out/'result.json').write_text(json.dumps(result,indent=2));print(json.dumps(result,indent=2));print('EVIDENCE '+str(out));raise SystemExit(0 if layers and not after['error'] and after.get('shots',0)>before.get('shots',0) else 1)
