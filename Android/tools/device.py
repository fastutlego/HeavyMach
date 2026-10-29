#!/usr/bin/env python3
"""HeavyMach device deployment. Always select a serial; never clear app data."""
from pathlib import Path
import argparse,datetime,getpass,hashlib,json,os,re,subprocess,sys,time
ROOT=Path(__file__).resolve().parents[2]
SDK=Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'))
ADB=str(SDK/'platform-tools/adb')
def run(args,**kw):return subprocess.run(args,check=True,**kw)
def output(args):return subprocess.check_output(args,text=True).strip()
p=argparse.ArgumentParser();p.add_argument('--serial');sub=p.add_subparsers(dest='action',required=True)
for a in ('devices','diagnose','finish','dump','wireless-usb','logs','capture'):sub.add_parser(a)
i=sub.add_parser('install');i.add_argument('apk',type=Path)
for a in ('launch','finish','dump','logs','capture'):
    q=next(x for k,x in sub.choices.items() if k==a) if a in sub.choices else sub.add_parser(a)
    q.add_argument('--edition',choices=('normal','cheat'),default='cheat')
for a in ('pair','connect'):
    q=sub.add_parser(a);q.add_argument('endpoint')
a=p.parse_args();base=[ADB]
if a.action=='devices':
    print(output(base+['devices','-l']));print(output(base+['mdns','services']));sys.exit()
if a.action in ('pair','connect'):
    if not re.fullmatch(r'[A-Za-z0-9.:-]+:\d+',a.endpoint):p.error('Use the phone IP:port')
    if a.action=='pair':
        code=getpass.getpass('Pairing code: ')
        if not re.fullmatch(r'\d{6}',code):p.error('Expected six digits')
        # Send via stdin, keeping the pairing code out of the process arguments.
        run(base+['pair',a.endpoint],input=code+'\n',text=True)
    else:run(base+['connect',a.endpoint])
    sys.exit()
if not a.serial:p.error('--serial is required; use devices first')
base+=['-s',a.serial]
if output(base+['get-state'])!='device':p.error('Device is not authorized/online')
package='com.gelb2.heavymach.'+('validation' if getattr(a,'edition','cheat')=='cheat' else 'restored')
if a.action=='diagnose':
    facts={k:output(base+['shell','getprop',k]) for k in ('ro.product.model','ro.product.device','ro.build.version.release','ro.build.version.sdk','ro.product.cpu.abilist','ro.serialno')}
    facts['transport']=a.serial;facts['page_size']=output(base+['shell','getconf','PAGE_SIZE']);facts['battery']='\n'.join(line for line in output(base+['shell','dumpsys','battery']).splitlines() if re.match(r'^  (level|temperature|status|USB powered|AC powered|voltage):',line));facts['lock']='\n'.join(line.strip() for line in output(base+['shell','dumpsys','window','policy']).splitlines() if re.match(r'^\s+(showing|mAwake|mScreenOnFully)=',line));print(json.dumps(facts,indent=2));sys.exit()
if a.action=='wireless-usb':
    listing=output([ADB,'devices','-l']);line=next((line for line in listing.splitlines() if line.split() and line.split()[0]==a.serial),'')
    if 'usb:' not in line:p.error('Select the USB serial for initial TCP/IP setup')
    route=output(base+['shell','ip','-f','inet','addr','show','wlan0']);m=re.search(r'inet (\d+\.\d+\.\d+\.\d+)/',route)
    if not m:p.error('Phone has no Wi-Fi address')
    identity=output(base+['shell','getprop','ro.serialno']);endpoint=m[1]+':5555';run(base+['tcpip','5555'])
    for attempt in range(6):
        time.sleep(1);result=output([ADB,'connect',endpoint])
        if 'connected to' in result:break
    else:raise SystemExit('Wi-Fi ADB connection failed: '+result)
    other=output([ADB,'-s',endpoint,'shell','getprop','ro.serialno'])
    if identity!=other:raise SystemExit('Wireless identity differs from USB device')
    print('Wireless serial: '+endpoint);sys.exit()
if a.action=='install':
    apk=a.apk.resolve()
    if not apk.is_file():p.error('APK not found')
    analyzer=SDK/'cmdline-tools/latest/bin/apkanalyzer'
    package=output([str(analyzer),'manifest','application-id',str(apk)])
    if package not in ('com.gelb2.heavymach.restored','com.gelb2.heavymach.validation'):p.error('Unexpected package '+package)
    print(json.dumps({'serial':a.serial,'package':package,'apk':str(apk),'sha256':hashlib.sha256(apk.read_bytes()).hexdigest()}))
    run(base+['install','-r',str(apk)])
elif a.action=='launch':run(base+['shell','am','start','-W','-n',package+'/com.gelb2.heavymach.GameActivity'])
elif a.action=='finish':
    run(base+['shell','am','force-stop',package]);run(base+['shell','input','keyevent','KEYCODE_HOME']);print('Stopped '+package+'; device returned to Home')
else:
    dest=ROOT/'Android/build/evidence'/datetime.datetime.now().strftime('%Y%m%d-%H%M%S');dest.mkdir(parents=True,exist_ok=True)
    if a.action=='capture':
        with (dest/'screen.png').open('wb') as f:run(base+['exec-out','screencap','-p'],stdout=f)
    elif a.action=='logs':
        with (dest/'logcat.txt').open('w') as f:run(base+['logcat','-d','-v','threadtime'],stdout=f)
    elif a.action=='dump':
        if a.edition!='cheat':p.error('Native diagnostics are available in the cheat edition')
        run(base+['shell','am','broadcast','-n',package+'/com.gelb2.heavymach.TestReceiver','-a','com.gelb2.heavymach.TEST','--es','command','dump'])
        with (dest/'diagnostics.json').open('w') as f:run(base+['exec-out','run-as',package,'cat','files/diagnostics.json'],stdout=f)
    print(dest)
