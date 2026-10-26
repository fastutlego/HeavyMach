#!/usr/bin/env python3
"""Run the existing scene test and attach Instruments to its new app process."""
import argparse
import json
from pathlib import Path
import subprocess
import time

p = argparse.ArgumentParser()
p.add_argument('--platform', choices=('mac', 'ipad'), required=True)
p.add_argument('--template', choices=('Time Profiler', 'Allocations', 'Metal System Trace'), required=True)
p.add_argument('--output', type=Path, required=True)
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
out = a.output.resolve()
out.mkdir(parents=True, exist_ok=False)
mac = a.platform == 'mac'
name = 'HeavyMachMacValidation' if mac else 'HeavyMachValidation'
def app_pids():
    rows = subprocess.check_output(['ps', '-axo', 'pid=,comm='], text=True).splitlines()
    return {int(row.strip().split(None, 1)[0]) for row in rows
            if len(row.strip().split(None, 1)) == 2 and Path(row.strip().split(None, 1)[1]).name == name}
old_pids = app_pids()
command = ['xcodebuild', '-workspace', 'HeavyMach.xcworkspace', '-scheme', name,
           '-configuration', 'Debug', '-destination',
           'platform=macOS,arch=arm64' if mac else 'platform=iOS Simulator,id=5177F449-414C-471D-AB92-79DF7F768DC9',
           '-derivedDataPath', 'build/workspace-mac' if mac else 'build/workspace-validation',
           '-parallel-testing-enabled', 'NO',
           '-only-testing:' + ('HeavyMachMacInputTests/HMAppleInputTests/testMacPresentedFrameCostsWithProductionTimer' if mac else 'HeavyMachRenderTests/HMFramePerformanceTests/testOnscreenOpenGLCadenceAudioAndResume'),
           'test-without-building']
with (out / 'tests.log').open('w') as test_log, (out / 'record.log').open('w') as trace_log:
    test = subprocess.Popen(command, cwd=root, stdout=test_log, stderr=subprocess.STDOUT)
    deadline = time.monotonic() + 90
    pid = None
    while time.monotonic() < deadline and test.poll() is None:
        found = app_pids() - old_pids
        if found:
            pid = min(found)
            break
        time.sleep(.2)
    if pid is None:
        test.terminate()
        raise SystemExit('Test app did not start; see tests.log')
    record = ['xcrun', 'xctrace', 'record', '--template', a.template, '--attach', str(pid),
              '--time-limit', '55s', '--output', str(out / 'recording.trace')]
    if not mac:
        record += ['--device', '5177F449-414C-471D-AB92-79DF7F768DC9']
    if a.template == 'Allocations':
        record += ['--instrument', 'Leaks']
    print(f'Attached target PID {pid}: {a.platform} / {a.template}', flush=True)
    trace = subprocess.Popen(record, cwd=root, stdout=trace_log, stderr=subprocess.STDOUT)
    if a.template == 'Allocations':
        # A command-line heap snapshot complements Instruments' GUI-only tables.
        time.sleep(18)
        with (out / 'leaks.txt').open('w') as leak_log:
            subprocess.run(['leaks', str(pid)], stdout=leak_log, stderr=subprocess.STDOUT, timeout=45)
        with (out / 'vmmap.txt').open('w') as memory_log:
            subprocess.run(['vmmap', '-summary', str(pid)], stdout=memory_log, stderr=subprocess.STDOUT, timeout=30)

    try:
        end = time.monotonic() + 120
        while test.poll() is None and time.monotonic() < end:
            content = (out / 'tests.log').read_text(errors='replace')
            if not mac and ("Test Suite 'Selected tests' failed" in content or "Test Suite 'Selected tests' passed" in content):
                time.sleep(3)
                subprocess.run(['xcrun','simctl','terminate','5177F449-414C-471D-AB92-79DF7F768DC9','com.gelb2.heavymach.validation'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                break
            time.sleep(.5)
        test_code = test.wait(timeout=15)
    except subprocess.TimeoutExpired:
        test.terminate()
        test_code = test.wait(timeout=15)
    print(f'Test process exit: {test_code}; preserving trace', flush=True)
    try:
        trace_code = trace.wait(timeout=180)
    except subprocess.TimeoutExpired:
        trace.terminate()
        trace_code = trace.wait(timeout=15)
    summary = dict(platform=a.platform, template=a.template, pid=pid, test_exit=test_code,
                   trace_exit=trace_code, command=command, trace_command=record)
    (out / 'run.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary), flush=True)
    raise SystemExit(0 if trace_code == 0 else 1)
