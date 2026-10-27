#!/usr/bin/env python3
"""Regenerate runtime crop/size metadata; leave authored PNGs unchanged."""
import json
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
r=json.load((ROOT / 'Art/GroundAA/20261028-runtime/manifest.json').open())
widths={'Shilka_Body':80,'Shilka_Body2':80,'Tunguska_Body':85,'Tunguska_Body2':85,'Shilka_Turret':51,'Tunguska_Turret':50,'Shilka_Gun':32,'Tunguska_Gun':37,'Tunguska_Missile':30}
baseXs={'Shilka_Radar':[224,696,1115,1556,224,688,1100,1536],'Tunguska_Radar':[233,633,969,1338,233,638,964,1345]}
lines=[]
for a in r:
 name=a['name'];src='Extensions/GAA_'+name+'.png'
 for i,b in enumerate(a.get('frames',[a['opaque_bounds']])):
  x,y,x1,y1=b;w=x1-x;h=y1-y
  if 'Radar'in name:
   H=16 if name.startswith('Shilka')else 13;W=round(w*H/h);ax=(baseXs[name][i]-(x+x1)/2)*W/w
   key='Extensions/GAA_'+name+str(i)+'.png'
  else:
   W=widths[name];H=round(h*W/w);ax=0;key=src
   if name.endswith('Body2'):H=18 if name.startswith('Shilka')else 22
  lines.append('  {"%s","%s",{%d,%d,%d,%d},{%d,%d},%.5ff},'%(key,src,x,y,w,h,W,H,ax))
(ROOT / 'Sources/Core/GroundAAArt.inc').open('w').write('// Generated from Art/GroundAA/20261028-runtime/manifest.json. Source pixels stay unchanged.\n'+'\n'.join(lines)+'\n')
