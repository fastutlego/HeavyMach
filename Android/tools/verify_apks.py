#!/usr/bin/env python3
"""Verify package separation, signatures and 16 KiB ELF/APK alignment."""
from pathlib import Path
import hashlib,json,os,struct,subprocess,zipfile
root=Path(__file__).resolve().parents[1];sdk=Path(os.environ.get('ANDROID_SDK_ROOT',Path.home()/'Library/Android/sdk'));bt=sdk/'build-tools/36.0.0';rows=[]
for flavor,package in [('normal','com.gelb2.heavymach.restored'),('cheat','com.gelb2.heavymach.validation')]:
 apk=root/f'app/build/outputs/apk/{flavor}/debug/app-{flavor}-debug.apk'
 appid=subprocess.check_output([str(sdk/'cmdline-tools/latest/bin/apkanalyzer'),'manifest','application-id',str(apk)],text=True).strip();assert appid==package
 subprocess.run([str(bt/'apksigner'),'verify',str(apk)],check=True)
 subprocess.run([str(bt/'zipalign'),'-c','-P','16','4',str(apk)],check=True)
 with zipfile.ZipFile(apk) as z:
  so=z.read('lib/arm64-v8a/libheavymach.so');assert so[:6]==b'\x7fELF\x02\x01'
  off=struct.unpack_from('<Q',so,32)[0];size,count=struct.unpack_from('<HH',so,54);align=[]
  for i in range(count):
   record=off+i*size
   if struct.unpack_from('<I',so,record)[0]==1:
    a=struct.unpack_from('<Q',so,record+48)[0];assert a>=16384;align.append(a)
  assert align
  assets=[n for n in z.namelist() if n.startswith('assets/')]
  manifest=subprocess.check_output([str(sdk/'cmdline-tools/latest/bin/apkanalyzer'),'manifest','print',str(apk)],text=True)
  assert ('com.gelb2.heavymach.TestReceiver' in manifest)==(flavor=='cheat')
 rows.append({'edition':flavor,'package':package,'sha256':hashlib.sha256(apk.read_bytes()).hexdigest(),'bytes':apk.stat().st_size,'asset_files':len(assets),'elf_load_alignment':align,'signature_verified':True,'apk_16kb_alignment':True,'cheat_receiver':flavor=='cheat','signing':'Android debug certificate'})
print(json.dumps(rows,indent=2))
