#!/usr/bin/env python3
"""Observe existing touchHLE guest via RSP. --run changes guest code in memory.
No app launch, options edit, or original binary write. Default action is self-test.
"""
import argparse
import json
import io
import math
import socket
import struct
import sys
import unittest
from pathlib import Path

POINTS = ((0x42DC, 'rot_before', bytes.fromhex('1cf0b6ea')),
          (0x42E0, 'rot_after', bytes.fromhex('b149')),
          (0x42EE, 'bounds_before', bytes.fromhex('1cf0aeea')),
          (0x42F2, 'bounds_after', bytes.fromhex('321c')))
FINAL_POINT = (0x477A, 'root_accumulated', bytes.fromhex('1098'))
TRAP = bytes.fromhex('00be')  # Thumb BKPT #0; runtime memory only.


def packet(body):
    data = body.encode('ascii')
    return b'$' + data + b'#' + f'{sum(data) & 255:02x}'.encode()


def words(data):
    if len(data) % 4:
        raise ValueError('ARM32 data must contain complete 4-byte words')
    return list(struct.unpack('<' + 'I' * (len(data)//4), data))


def decode_buffer(data):
    if len(data) != 24:
        raise ValueError('Expected CGRect16 + CGPoint8, exactly24 bytes')
    values = struct.unpack('<6f', data)
    return {'hex': data.hex(), 'u32': words(data),
            'floats': [v if math.isfinite(v) else str(v) for v in values],
            'order': ['rect_left', 'rect_bottom', 'rect_right', 'rect_top', 'rot_x', 'rot_y']}


class RSP:
    def __init__(self, stream):
        self.stream = stream

    def greet(self):
        # touchHLE GdbServer::new requires a one-byte ACK before any packet.
        self.stream.sendall(b'+')
        if self.read_exact(1) != b'+':
            raise ValueError('Invalid debugger greeting')

    def read_exact(self, n):
        out = b''
        while len(out) < n:
            chunk = self.stream.recv(n-len(out))
            if not chunk:
                raise EOFError('Debugger connection closed')
            out += chunk
        return out

    def reply(self):
        while True:
            lead = self.read_exact(1)
            if lead == b'+':
                continue
            if lead != b'$':
                raise ValueError(f'Unexpected RSP byte {lead!r}')
            body = bytearray()
            while True:
                c = self.read_exact(1)
                if c == b'#':
                    break
                body += c
            checksum = int(self.read_exact(2), 16)
            if sum(body) & 255 != checksum:
                raise ValueError('RSP checksum mismatch')
            self.stream.sendall(b'+')
            return body.decode('ascii')

    def command(self, body):
        self.stream.sendall(packet(body))
        return self.reply()

    def memory(self, addr, size):
        reply = self.command(f'm{addr:x},{size:x}')
        if reply.startswith('E'):
            raise ValueError(f'Memory read {addr:#x}: {reply}')
        data = bytes.fromhex(reply)
        if len(data) != size:
            raise ValueError('Short memory read')
        return data

    def write(self, addr, data):
        if self.command(f'M{addr:x},{len(data):x}:{data.hex()}') != 'OK':
            raise ValueError(f'Memory write failed at {addr:#x}')

    def registers(self):
        data = bytes.fromhex(self.command('g'))
        if len(data) != 64:
            raise ValueError('Expected16 ARM32 general registers')
        return words(data)


def observe(rsp, output, calls, final=False):
    """One temporary breakpoint at a time; no remote single-step is required."""
    points = POINTS + (FINAL_POINT,) if final else POINTS
    stop = rsp.command('?')
    if not stop.startswith(('S', 'T')):
        raise ValueError(f'Unexpected initial stop: {stop}')
    for addr, _, expected in points:
        if rsp.memory(addr, len(expected)) != expected:
            raise ValueError(f'Guest instruction mismatch at {addr:#x}; no breakpoint installed')
    halted, patched, completed, sampled, frame = True, None, 0, False, None
    try:
        while completed < calls:
            for addr, phase, expected in points:
                rsp.write(addr, TRAP)
                patched = (addr, expected[:2])
                halted = False
                stop = rsp.command('c')
                halted = True
                regs = rsp.registers()
                # debug_cpu_error rewinds Thumb PC by2 before reporting the stop.
                if not stop.startswith('T05') or regs[15] != addr:
                    raise ValueError(f'Unexpected stop {stop}, pc={regs[15]:#x}')
                cpsr = words(bytes.fromhex(rsp.command('p19')))[0]
                if not cpsr & 32:
                    raise ValueError('Expected Thumb state')
                rsp.write(addr, expected[:2])
                patched = None
                tid = rsp.command('qC')
                if phase == 'rot_before':
                    sampled = regs[10] == 0
                    frame = (regs[13], tid)
                elif frame != (regs[13], tid):
                    raise ValueError('Thread/stack changed during getter observation')
                if sampled:
                    sp = regs[13]
                    owner = words(rsp.memory(sp+0x30, 4))[0]
                    record = {'sample': completed, 'phase': phase, 'pc': hex(addr),
                              'thread': tid, 'sp': hex(sp), 'r10': hex(regs[10]),
                              'self': hex(owner), 'registers_u32': regs,
                              'buffer': decode_buffer(rsp.memory(sp+0x50, 24)),
                              'machine_bounds_hex': rsp.memory(owner+52, 16).hex()}
                    output.write(json.dumps(record, allow_nan=False)+'\n')
                    output.flush()
            if sampled:
                completed += 1
    finally:
        if halted:
            if patched:
                rsp.write(*patched)
            # c has no immediate ACK result beyond '+': do not wait for a stop
            # after removing the last breakpoint. Resume before closing socket.
            rsp.stream.sendall(packet('c'))
            if rsp.stream.recv(1) != b'+':
                raise ValueError('Missing final continue acknowledgment')
        elif patched:
            print('Guest may still contain a breakpoint: connection lost while running; '
                  'terminate/relaunch the comparison app before reuse.', file=sys.stderr)


class FakeStream:
    def __init__(self, data): self.data, self.sent = data, b''
    def recv(self, n):
        result, self.data = self.data[:min(n, 1)], self.data[min(n, 1):]
        return result
    def sendall(self, data): self.sent += data


class ParserTests(unittest.TestCase):
    def test_touchhle_greeting(self):
        stream = FakeStream(b'+')
        RSP(stream).greet()
        self.assertEqual(stream.sent, b'+')
        with self.assertRaises(ValueError):
            RSP(FakeStream(b'-')).greet()

    def test_frame_and_partial_transport(self):
        stream = FakeStream(b'+'+packet('0100000000000080'))
        self.assertEqual(RSP(stream).command('g'), '0100000000000080')
        self.assertEqual(stream.sent, packet('g')+b'+')
    def test_checksum(self):
        with self.assertRaises(ValueError): RSP(FakeStream(b'$OK#00')).reply()
    def test_words_and_buffer(self):
        data = struct.pack('<6f', -20, 0, 20, 40, 1.25, -.5)
        decoded = decode_buffer(data)
        self.assertEqual(decoded['floats'], [-20, 0, 20, 40, 1.25, -.5])
        self.assertEqual(bytes.fromhex(decoded['hex']), data)
        self.assertEqual(words(bytes.fromhex('78563412')), [0x12345678])
        with self.assertRaises(ValueError): decode_buffer(data[:-1])
    def test_nonfinite_lossless_json(self):
        data = struct.pack('<6I', 0x7fc12345, 0x80000000, 0x7f800000, 0, 0, 0)
        decoded = decode_buffer(data)
        self.assertEqual(decoded['u32'][0], 0x7fc12345)
        self.assertEqual(decoded['u32'][1], 0x80000000)
        json.dumps(decoded, allow_nan=False)
    def test_remote_read_error(self):
        with self.assertRaises(ValueError): RSP(FakeStream(packet('E00'))).memory(0, 4)

    def test_collector_sequence_and_restore(self):
        self.check_collector(False)

    def test_collector_with_root_accumulated(self):
        self.check_collector(True)

    def check_collector(self, final):
        points = POINTS + (FINAL_POINT,) if final else POINTS
        class Model:
            def __init__(self):
                self.stream = FakeStream(b'+'); self.mem = {}; self.pc = 0; self.stops = 0
                for addr, _, code in points:
                    for n, byte in enumerate(code): self.mem[addr+n] = byte
            def command(self, cmd):
                if cmd == '?': return 'S00'
                if cmd == 'qC': return 'QC1'
                if cmd == 'p19': return '20000000'
                if cmd == 'c':
                    self.pc = next(addr for addr, _, _ in points if self.memory(addr, 2) == TRAP)
                    self.stops += 1
                    return 'T05thread:1;'
                raise AssertionError(cmd)
            def memory(self, addr, size):
                if addr == 0x1030: return struct.pack('<I', 0x2000)
                if addr == 0x1050: return struct.pack('<6f', 1, 2, 3, 4, 5, 6)
                if addr == 0x2034: return bytes(16)
                return bytes(self.mem[addr+n] for n in range(size))
            def write(self, addr, data):
                for n, byte in enumerate(data): self.mem[addr+n] = byte
            def registers(self):
                regs = [0]*16; regs[13] = 0x1000; regs[15] = self.pc
                regs[10] = 1 if self.stops <= len(points) else 0  # Skip an actual non-nil call.
                return regs
        remote = Model(); output = io.StringIO(); observe(remote, output, 1, final)
        records = [json.loads(line) for line in output.getvalue().splitlines()]
        self.assertEqual([r['phase'] for r in records], [p[1] for p in points])
        self.assertTrue(all(r['r10'] == '0x0' for r in records))
        self.assertEqual(remote.stops, len(points)*2)
        for addr, _, code in points: self.assertEqual(remote.memory(addr, len(code)), code)
        self.assertEqual(remote.stream.sent, packet('c'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--run', action='store_true', help='Connect and patch guest memory; root-operated only')
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=24681)
    parser.add_argument('--calls', type=int, default=3)
    parser.add_argument('--final', action='store_true', help='Also observe root addParts accumulated bounds before child parts')
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    if not args.run:
        result = unittest.TextTestRunner().run(unittest.defaultTestLoader.loadTestsFromTestCase(ParserTests))
        return 0 if result.wasSuccessful() else 1
    if args.calls < 1 or args.output is None:
        parser.error('--run requires --calls >=1 and --output PATH')
    # Exclusive creation protects an existing capture from accidental overwrite.
    with args.output.open('x') as output, socket.create_connection((args.host, args.port), timeout=10) as stream:
        stream.settimeout(None)  # Waiting for the operator to enter gameplay is intentional.
        rsp = RSP(stream)
        rsp.greet()
        observe(rsp, output, args.calls, args.final)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
