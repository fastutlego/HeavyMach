#!/usr/bin/env python3
"""Emit authored sprite crops and fixed radar axle anchors; never alter pixels."""
import json
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
SOURCE = 'Art/GroundAA/20261028-integrated/manifest.json'
manifest = json.loads((ROOT / SOURCE).read_text())
lines = ['// Generated from ' + SOURCE + '. Source pixels stay unchanged.']
for image in manifest['images']:
    crop = ','.join(map(str, image['crop']))
    size = ','.join(map(str, image['size']))
    x, y = image['anchor']
    lines.append('  {"%s","%s",{%s},{%s},%.5ff,%.5ff},' %
                 (image['name'], image['source'], crop, size, x, y))
(ROOT / 'Sources/Core/GroundAAArt.inc').write_text('\n'.join(lines) + '\n')
