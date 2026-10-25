#!/usr/bin/env python3
"""Observe named enemy construction in the original ARM guest, not native code.
--run temporarily patches guest instructions. Default runs host-side tests only.
No app launch, save edits, guest calls, or original binary changes.
"""
import argparse
import io
import json
import socket
import struct
import unittest
from pathlib import Path
from observe_nil_parts import RSP, FakeStream, packet, words, TRAP

ENTRY = 0x14918
DECODED = 0x14992
RETURNED = 0x14AE6
CODES = {ENTRY: bytes.fromhex('f0b5'), DECODED: bytes.fromhex('301c'), RETURNED: bytes.fromhex('05b0')}


def read_name(rsp, pointer):
    # Names are C strings, not Objective-C objects. Bound reads to original
    # authored machine-name field size. Byte reads avoid crossing its NUL.
    raw = bytearray()
    for i in range(24):
        b = rsp.memory(pointer+i, 1)
        if b == b'\0':
            return {'pointer': hex(pointer), 'hex': raw.hex(), 'text': raw.decode('utf-8')}
        raw += b
    raise ValueError('Machine name lacks NUL within24 bytes')


def observe(rsp, output, calls):
    stop = rsp.command('?')
    if not stop.startswith(('S', 'T')):
        raise ValueError('Unexpected initial stop')
    for addr, code in CODES.items():
        if rsp.memory(addr, 2) != code:
            raise ValueError(f'Guest instruction mismatch at {addr:#x}')
    patched = set()
    halted = True

    def restore():
        for addr in list(patched):
            rsp.write(addr, CODES[addr])
            patched.remove(addr)

    def advance(addresses):
        nonlocal halted
        for addr in addresses:
            rsp.write(addr, TRAP)
            patched.add(addr)
        halted = False
        stop = rsp.command('c')
        halted = True
        regs = rsp.registers()
        # Restore every owned alternative trap before validating this stop.
        restore()
        if not stop.startswith('T05') or regs[15] not in addresses:
            raise ValueError(f'Unexpected stop {stop}, pc={regs[15]:#x}')
        if not words(bytes.fromhex(rsp.command('p19')))[0] & 32:
            raise ValueError('Expected Thumb state')
        return regs, rsp.command('qC')

    def emit(sample, phase, regs, tid, owner, name):
        output.write(json.dumps({'sample': sample, 'phase': phase, 'pc': hex(regs[15]),
            'thread': tid, 'sp': hex(regs[13]), 'self': hex(owner), 'name': name,
            'registers_u32': regs, 'bounds_hex': rsp.memory(owner+0x34, 16).hex(),
            'success': regs[0] if phase == 'returned' else None})+'\n')
        output.flush()

    try:
        for sample in range(calls):
            regs, tid = advance([ENTRY])
            owner, pointer, entry_sp = regs[0], regs[2], regs[13]
            name = read_name(rsp, pointer)
            emit(sample, 'entry', regs, tid, owner, name)
            # Missing resources may bypass DECODED. The return sentinel catches
            # that path instead of waiting forever for an unreachable address.
            regs, thread = advance([DECODED, RETURNED])
            frame = (entry_sp - 0x34) & 0xffffffff
            while True:
                if thread != tid or regs[8] != owner or regs[10] != pointer or regs[13] != frame:
                    raise ValueError('Construction thread/owner/name/frame mismatch')
                if read_name(rsp, pointer) != name:
                    raise ValueError('Machine name memory changed during construction')
                phase = 'decoded' if regs[15] == DECODED else 'returned'
                emit(sample, phase, regs, thread, owner, name)
                if phase == 'returned':
                    break
                regs, thread = advance([RETURNED])
    finally:
        if halted:
            restore()
            rsp.stream.sendall(packet('c'))
            if rsp.stream.recv(1) != b'+':
                raise ValueError('Missing final continue acknowledgment')
        elif patched:
            print('Connection lost while guest running; traps may remain. Terminate/relaunch observation app before reuse.')


class Model:
    def __init__(self, failure=False, wrong_owner=False, wrong_frame=False):
        self.mem={a+i:b for a,c in CODES.items() for i,b in enumerate(c)}
        self.stream=FakeStream(b'+');self.failure=failure;self.wrong_owner=wrong_owner
        self.pc=0;self.stops=0;self.wrong_frame=wrong_frame
    def command(self,c):
        if c=='?':return 'S00'
        if c=='qC':return 'QC1'
        if c=='p19':return '20000000'
        if c=='c':
            traps=[a for a in CODES if self.memory(a,2)==TRAP]
            self.pc=RETURNED if self.failure and RETURNED in traps else traps[0]
            self.stops+=1;return 'T05thread:1;'
        raise AssertionError(c)
    def write(self,a,data):
        for i,b in enumerate(data):self.mem[a+i]=b
    def memory(self,a,n):
        if 0x3000<=a<0x3018:return b'ArmoredCar015\0'[a-0x3000:a-0x3000+n]
        if a==0x2034:return struct.pack('<4f',-20,0,20,40)
        return bytes(self.mem[a+i] for i in range(n))
    def registers(self):
        r=[0]*16;r[15]=self.pc;r[13]=0x4000 if self.pc==ENTRY else (0x3fd8 if self.wrong_frame else 0x3fcc)
        if self.pc==ENTRY:r[0]=0x2000;r[2]=0x3000
        else:r[8]=0x2100 if self.wrong_owner else 0x2000;r[10]=0x3000;r[0]=0 if self.failure else 1
        return r


class Tests(unittest.TestCase):
    def assert_restored(self,m):
        for a,c in CODES.items():self.assertEqual(m.memory(a,2),c)
        self.assertEqual(m.stream.sent,packet('c'))
    def test_success_identity_and_final_bounds(self):
        m=Model();o=io.StringIO();observe(m,o,1)
        records=[json.loads(s) for s in o.getvalue().splitlines()]
        self.assertEqual([r['phase'] for r in records],['entry','decoded','returned'])
        self.assertEqual(records[-1]['success'],1)
        self.assertEqual(records[-1]['name']['text'],'ArmoredCar015')
        self.assertEqual(records[-1]['bounds_hex'],'0000a0c1000000000000a04100002042')
        self.assert_restored(m)
    def test_missing_resource_early_return(self):
        m=Model(failure=True);o=io.StringIO();observe(m,o,1)
        records=[json.loads(s) for s in o.getvalue().splitlines()]
        self.assertEqual([r['phase'] for r in records],['entry','returned'])
        self.assertEqual(records[-1]['success'],0);self.assert_restored(m)
    def test_owner_mismatch_restores_both_traps(self):
        m=Model(wrong_owner=True)
        with self.assertRaises(ValueError):observe(m,io.StringIO(),1)
        self.assert_restored(m)
    def test_entry_frame_mismatch_restores_both_traps(self):
        m=Model(wrong_frame=True)
        with self.assertRaises(ValueError):observe(m,io.StringIO(),1)
        self.assert_restored(m)
    def test_invalid_instruction_never_patches(self):
        m=Model();m.mem[ENTRY]=0
        with self.assertRaises(ValueError):observe(m,io.StringIO(),1)
        self.assertFalse(any(m.memory(a,2)==TRAP for a in CODES))
        self.assertEqual(m.stream.sent,b'')
    def test_unterminated_name_rejected(self):
        class Remote:
            def memory(self,a,n):return b'x'*n
        with self.assertRaises(ValueError):read_name(Remote(),0)


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--run',action='store_true');p.add_argument('--host',default='127.0.0.1')
    p.add_argument('--port',type=int,default=24681);p.add_argument('--calls',type=int,default=3)
    p.add_argument('--output',type=Path)
    a=p.parse_args()
    if not a.run:
        r=unittest.TextTestRunner().run(unittest.defaultTestLoader.loadTestsFromTestCase(Tests))
        return 0 if r.wasSuccessful() else 1
    if a.calls<1 or a.output is None:p.error('--run requires positive --calls and --output')
    with a.output.open('x') as output,socket.create_connection((a.host,a.port),timeout=10) as stream:
        stream.settimeout(None)
        rsp=RSP(stream);rsp.greet();observe(rsp,output,a.calls)
    return 0

if __name__=='__main__':raise SystemExit(main())
