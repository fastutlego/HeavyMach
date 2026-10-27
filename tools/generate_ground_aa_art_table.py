#!/usr/bin/env python3
"""Regenerate runtime crop/size metadata; leave authored PNGs unchanged."""
import json
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
r=json.load((ROOT / 'Art/GroundAA/20261028-runtime/manifest.json').open())
widths={'Shilka_Body':104,'Shilka_Body2':104,'Tunguska_Body':112,'Tunguska_Body2':112,'Shilka_Turret':66,'Tunguska_Turret':66,'Shilka_Gun':55,'Tunguska_Gun':58,'Tunguska_Missile':40}
baseXs={'Shilka_Radar':[207,575,958,1363,211,596,966,1328],'Tunguska_Radar':[228,609,961,1333,219,613,961,1330]}
lines=[]
for a in r:
 name=a['name'];src='Extensions/GAA_'+name+'.png'
 for i,b in enumerate(a.get('frames',[a['opaque_bounds']])):
  x,y,x1,y1=b;w=x1-x;h=y1-y
  if 'Radar'in name:
   H=32 if name.startswith('Shilka')else 36;W=round(w*H/h);ax=(baseXs[name][i]-(x+x1)/2)*W/w
   key='Extensions/GAA_'+name+str(i)+'.png'
  else:
   W=widths[name];H=round(h*W/w);ax=0;key=src
   if name.endswith('Body2'):H=24 if name.startswith('Shilka')else 29
  lines.append('  {"%s","%s",{%d,%d,%d,%d},{%d,%d},%.5ff},'%(key,src,x,y,w,h,W,H,ax))
(ROOT / 'Sources/Core/GroundAAArt.inc').open('w').write('// Generated from Art/GroundAA/20261028-runtime/manifest.json. Source pixels stay unchanged.\n'+'\n'.join(lines)+'\n')
