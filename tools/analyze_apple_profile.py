#!/usr/bin/env python3
"""Summarize exported Time Profiler stacks; retain original samples separately."""
import argparse
from collections import Counter
import json
import xml.etree.ElementTree as ET
p=argparse.ArgumentParser();p.add_argument('samples');p.add_argument('--output',required=True);a=p.parse_args()
r=ET.parse(a.samples).getroot();refs={e.attrib['id']:e for e in r.iter() if 'id' in e.attrib}
def resolve(e):
    return refs.get(e.get('ref'),e) if e is not None else None
all_counts=Counter();frame_counts=Counter();rows=0;frame_rows=0
for row in r.iter('row'):
    stack=resolve(row.find('tagged-backtrace'))
    if stack is None:continue
    names=[resolve(f).get('name','?') for f in stack.findall('frame')]
    rows+=1;all_counts.update(set(names))
    if '-[HMGameView frame:]' in names:
        frame_rows+=1;frame_counts.update(set(names[:names.index("-[HMGameView frame:]")+1]))
result={'sample_rows':rows,'game_frame_sample_rows':frame_rows,
        'inclusive_all':all_counts.most_common(25),'inclusive_game_frame':frame_counts.most_common(35),
        'scope':'CPU samples; inclusive stacks overlap. No GPU time or wall-clock FPS inference.'}
with open(a.output,'w') as f:json.dump(result,f,indent=2,ensure_ascii=False);f.write('\n')
print(json.dumps(result,ensure_ascii=False))
