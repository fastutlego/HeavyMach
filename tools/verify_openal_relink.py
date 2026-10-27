#!/usr/bin/env python3
"""Rebuild an unpacked source release with a visibly modified OpenAL version."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source', type=Path, required=True, help='Unpacked, disposable source release (not a Git checkout)')
args = parser.parse_args()
root = args.source.resolve()
if root == Path(__file__).resolve().parents[1] or (root / '.git').exists():
    parser.error('Use a separate unpacked source archive; the working repository is not modified.')
version = root / 'vendor/openal-soft/version.h.in'
original = version.read_bytes()
needle = b'#define ALSOFT_VERSION "${LIB_VERSION}"'
marker = b'HM-OPENAL-RELINK-CHECK'
if original.count(needle) != 1:
    parser.error('Unexpected OpenAL version template')
results = []
try:
    version.write_bytes(original.replace(needle, b'#define ALSOFT_VERSION "${LIB_VERSION}-' + marker + b'"'))
    for sdk, scheme, destination, config in [
        ('iphonesimulator', 'HeavyMachRestored', 'generic/platform=iOS Simulator', 'Debug-iphonesimulator'),
        ('macosx', 'HeavyMachMac', 'platform=macOS,arch=arm64', 'Debug'),
    ]:
        log = root.parent / f'relink-{sdk}.log'
        derived = root / 'build' / ('ci-ios' if sdk == 'iphonesimulator' else 'ci-mac')
        with log.open('w') as stream:
            subprocess.run(['bash', 'tools/build_openal.sh', sdk], cwd=root, stdout=stream, stderr=subprocess.STDOUT, check=True)
            subprocess.run(['xcodebuild', '-quiet', '-workspace', 'HeavyMach.xcworkspace', '-scheme', scheme,
                            '-configuration', 'Debug', '-sdk', sdk, '-destination', destination,
                            '-derivedDataPath', str(derived), 'ARCHS=arm64', 'ONLY_ACTIVE_ARCH=YES',
                            'CODE_SIGNING_ALLOWED=NO', 'build'], cwd=root, stdout=stream, stderr=subprocess.STDOUT, check=True)
        app = derived / 'Build/Products' / config / f'{scheme}.app'
        # Xcode can place Debug application code in <name>.debug.dylib.
        executable = app / ('Contents/MacOS/' + scheme if sdk == 'macosx' else scheme)
        candidates = [executable] + list(app.rglob('*.debug.dylib'))
        matched = [binary for binary in candidates if marker in binary.read_bytes()]
        if not matched:
            raise RuntimeError(f'Modified library version absent from {scheme}')
        results.append({'sdk':sdk, 'scheme':scheme, 'modified_library_marker_in_app':True,
                        'linked_files':[{'path':str(b.relative_to(app)), 'sha256':hashlib.sha256(b.read_bytes()).hexdigest()} for b in matched],
                        'log_sha256':hashlib.sha256(log.read_bytes()).hexdigest()})
finally:
    version.write_bytes(original)
evidence = {'source_template_restored':version.read_bytes() == original, 'results':results,
            'scope':'unsigned link verification; no device install or gameplay run'}
(root.parent / 'openal-relink.json').write_text(json.dumps(evidence,indent=2)+'\n')
print(json.dumps(evidence,indent=2))
