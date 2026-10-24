#!/usr/bin/env python3
import os,sys,shutil,plistlib,zipfile,hashlib,json
from pathlib import Path
root=Path(__file__).resolve().parent
source=root/'compat-source/fresh-port/20-touchHLE-src/touchHLE'
xcode='--xcode' in sys.argv
sim=(os.environ.get('PLATFORM_NAME')=='iphonesimulator') if xcode else ('--device' not in sys.argv)
target='aarch64-apple-ios-sim' if sim else 'aarch64-apple-ios'
app=(Path(os.environ['TARGET_BUILD_DIR'])/os.environ['WRAPPER_NAME']) if xcode else root/'build'/target/'HeavyMachRecovery.app'
exe=source/'target'/target/'release/touchHLE'
if not exe.is_file(): raise SystemExit(f'Build missing: {exe}')
app.mkdir(parents=True,exist_ok=True)
shutil.copy2(exe,app/'HeavyMachRecovery')
# Xcode must sign the newly copied binary even when Cargo reused its cache.
os.utime(app/'HeavyMachRecovery', None)
shutil.copy2(root/'HeavyMach_options.txt',app/'HeavyMach_options.txt')
shutil.copytree(source/'touchHLE_fonts',app/'touchHLE_fonts',dirs_exist_ok=True)
# Guest libraries are data consumed by HLE, never loaded by the host dyld.
with zipfile.ZipFile(app/'touchHLE_dylibs.zip','w',zipfile.ZIP_DEFLATED) as z:
 for p in (source/'touchHLE_dylibs').iterdir():
  if p.is_file(): z.write(p,p.name)
shutil.copy2(source/'touchHLE_default_options.txt',app/'touchHLE_default_options.txt')
ipa=root/'originals/Heavy_Mach_1.13_ios_2.1.ipa'
expected=json.loads((root/'upstream.json').read_text())['ipa_sha256']
if hashlib.sha256(ipa.read_bytes()).hexdigest()!=expected:
 raise SystemExit('Original IPA SHA-256 mismatch; refusing to package')
shutil.copy2(ipa,app/'HeavyMach.ipa')
if not xcode:
 d=plistlib.load(open(root/'ios/Info.plist','rb'));d['CFBundleSupportedPlatforms']=['iPhoneSimulator' if sim else 'iPhoneOS'];plistlib.dump(d,open(app/'Info.plist','wb'))
print(app)
