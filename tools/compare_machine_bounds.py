#!/usr/bin/env python3
"""Compare named original guest construction captures with native GameSession output.
Empty, incomplete, failed or mismatched observations never count as agreement.
No arguments: host-side tests. --original/--native/--output: compare captures.
"""
import argparse
import copy
import hashlib
import json
import struct
import unittest
from pathlib import Path


def compare(original, native):
    if not original:
        raise ValueError('No original runtime observations')
    if native['constructionErrors'] or native['unsupportedEvents']:
        raise ValueError('Native construction has errors or unsupported events')
    rows = native['syntheticConstruction']
    lookup = {r['name']: r for r in rows}
    if len(lookup) != len(rows) or len(rows) != native['machineCount']:
        raise ValueError('Native construction inventory is incomplete or duplicate')
    samples = {}
    for row in original:
        samples.setdefault(row['sample'], []).append(row)
    comparisons = []
    for sample, group in samples.items():
        if [r['phase'] for r in group] != ['entry', 'decoded', 'returned']:
            raise ValueError(f'Sample {sample}: incomplete or failed construction sequence')
        entry, decoded, returned = group
        key = (entry['self'], entry['thread'], entry['name'])
        if any((r['self'], r['thread'], r['name']) != key for r in group):
            raise ValueError(f'Sample {sample}: construction identity mismatch')
        if [int(r['pc'], 16) for r in group] != [0x14918, 0x14992, 0x14ae6]:
            raise ValueError(f'Sample {sample}: wrong observation boundary')
        frame = (int(entry['sp'], 16)-0x34) & 0xffffffff
        if any(int(r['sp'], 16) != frame for r in group[1:]):
            raise ValueError(f'Sample {sample}: call frame mismatch')
        if returned['success'] != 1:
            raise ValueError(f'Sample {sample}: original constructor did not succeed')
        name = entry['name']['text']
        if bytes.fromhex(entry['name']['hex']).decode('utf-8') != name:
            raise ValueError('Name raw bytes differ from text')
        item = lookup[name]
        if item['boundsOrder'] != ['left', 'bottom', 'right', 'top']:
            raise ValueError('Unexpected native bounds order')
        expected = struct.pack('<4I', *(int(v, 16) for v in item['boundsBinary32']))
        actual = bytes.fromhex(returned['bounds_hex'])
        after_decode = bytes.fromhex(decoded['bounds_hex'])
        if len(actual) != 16 or len(after_decode) != 16:
            raise ValueError('Bounds are not four binary32 words')
        comparisons.append({'sample': sample, 'name': name,
            'original_decoded_hex': after_decode.hex(), 'original_returned_hex': actual.hex(),
            'native_hex': expected.hex(), 'returned_match': actual == expected,
            'decoded_match': after_decode == expected,
            'bounds_changed_after_decode': actual != after_decode})
    return {'scope': 'Only named successful original runtime calls in the supplied capture; no whole-game equivalence claim',
            'original_calls': len(comparisons), 'unique_names': len({r['name'] for r in comparisons}),
            'native_inventory_count': len(rows),
            'all_observed_returned_match': all(r['returned_match'] for r in comparisons),
            'comparisons': comparisons}


class Tests(unittest.TestCase):
    def setUp(self):
        name={'text':'ArmoredCar015','hex':b'ArmoredCar015'.hex(),'pointer':'0x3000'}
        self.original=[dict(sample=0,phase=phase,pc=hex(pc),sp=hex(sp),self='0x2000',thread='QC1',name=copy.deepcopy(name),success=success,bounds_hex='0000a0c1000000000000a04100002642') for phase,pc,sp,success in [('entry',0x14918,0x4000,None),('decoded',0x14992,0x3fcc,None),('returned',0x14ae6,0x3fcc,1)]]
        self.native={'constructionErrors':0,'unsupportedEvents':[],'machineCount':1,'syntheticConstruction':[{'name':'ArmoredCar015','boundsOrder':['left','bottom','right','top'],'boundsBinary32':['c1a00000','00000000','41a00000','42260000']}]}
    def test_match(self):
        self.assertTrue(compare(self.original,self.native)['all_observed_returned_match'])
    def test_one_bit_difference_is_not_hidden(self):
        self.original[-1]['bounds_hex']='0000a0c1000000000000a04101002642'
        self.assertFalse(compare(self.original,self.native)['all_observed_returned_match'])
    def test_empty_and_partial_rejected(self):
        for rows in [[],self.original[:2],self.original[::2]]:
            with self.assertRaises(ValueError):compare(rows,self.native)
    def test_frame_and_identity_rejected(self):
        self.original[-1]['sp']='0x3fd8'
        with self.assertRaises(ValueError):compare(self.original,self.native)
        self.original[-1]['sp']='0x3fcc';self.original[-1]['self']='0x2100'
        with self.assertRaises(ValueError):compare(self.original,self.native)
    def test_failed_and_duplicate_rejected(self):
        self.original[-1]['success']=0
        with self.assertRaises(ValueError):compare(self.original,self.native)
        self.original[-1]['success']=1;self.original.append(copy.deepcopy(self.original[-1]))
        with self.assertRaises(ValueError):compare(self.original,self.native)


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original',type=Path);p.add_argument('--native',type=Path);p.add_argument('--output',type=Path)
    a=p.parse_args()
    if not any([a.original,a.native,a.output]):
        r=unittest.TextTestRunner().run(unittest.defaultTestLoader.loadTestsFromTestCase(Tests))
        return 0 if r.wasSuccessful() else 1
    if not all([a.original,a.native,a.output]):p.error('All three paths are required')
    original_bytes=a.original.read_bytes();native_bytes=a.native.read_bytes()
    result=compare([json.loads(s) for s in original_bytes.splitlines()],json.loads(native_bytes))
    result['inputs']={str(a.original):hashlib.sha256(original_bytes).hexdigest(),str(a.native):hashlib.sha256(native_bytes).hexdigest()}
    with a.output.open('x') as f:json.dump(result,f,indent=2);f.write('\n')
    print(json.dumps({k:v for k,v in result.items() if k not in ('comparisons','inputs')}))
    return 0 if result['all_observed_returned_match'] else 1

if __name__=='__main__':raise SystemExit(main())
