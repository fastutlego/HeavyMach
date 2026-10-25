#!/usr/bin/env python3
"""Create analysis-only copies to expose Ghidra's omitted Thumb switch blocks.

Never execute these copies as the game; only one effect branch is reachable.
The original source is read-only. All replacements are four bytes at 0x9b86.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

parser=argparse.ArgumentParser()
parser.add_argument('--source',type=Path,required=True)
parser.add_argument('--output',type=Path,default=Path('/tmp'))
args=parser.parse_args()
source=args.source.resolve();directory=args.output.resolve()
original=source.read_bytes()
assert hashlib.sha256(original).hexdigest()=='97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb'
assert original[0x9b86:0x9b8a]==bytes.fromhex('15f0a0ea')
directory.mkdir(parents=True,exist_ok=True)
for case,target in [(2,0xac00),(3,0xac82),(4,0xafc4)]:
    path=directory/f'HeavyMach-bullet-case{case}'
    assert path.resolve()!=source
    derived=bytearray(original)
    immediate=(target-(0xab86+4))//2
    patch=struct.pack('<HH',0xe000|(immediate&0x7ff),0x46c0)
    derived[0x9b86:0x9b8a]=patch
    path.write_bytes(derived)
    print(json.dumps({'case':case,'path':str(path),'sha256':hashlib.sha256(derived).hexdigest(),'replacement_hex':patch.hex()}))
assert source.read_bytes()==original
