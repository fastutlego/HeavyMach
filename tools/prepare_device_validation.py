#!/usr/bin/env python3
"""Check/install the isolated validation app on the designated physical iPhone."""
import argparse
import datetime as dt
import hashlib
import json
from pathlib import Path
import plistlib
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
DEVICE = "00008150-000808EC36D8401C"
BUNDLE = "com.gelb2.heavymach.validation"
DEFAULT_APP = ROOT / "build/direction-controls-20261024/iphoneos/HeavyMachValidation.app"


def code_hashes(app):
    """Hash Mach-O code, including Debug.dylib and embedded frameworks."""
    magic = {bytes.fromhex(value) for value in
             ("feedface", "cefaedfe", "feedfacf", "cffaedfe",
              "cafebabe", "bebafeca", "cafebabf", "bfbafeca")}
    hashes = {}
    for path in sorted(app.rglob("*")):
        if not path.is_file():
            continue
        with path.open("rb") as file:
            header = file.read(4)
        if header in magic or path.suffix == ".dylib":
            hashes[str(path.relative_to(app))] = hashlib.sha256(path.read_bytes()).hexdigest()
    return hashes


def run(argv, timeout=60):
    return subprocess.run(argv, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                          timeout=timeout, check=False)


class Session:
    def __init__(self, action):
        stamp = dt.datetime.now(dt.timezone.utc).strftime("%Y%m%dT%H%M%S.%fZ")
        self.folder = ROOT / "build/device-validation-runs" / stamp
        self.folder.mkdir(parents=True)
        self.report = {"action": action, "device": DEVICE, "bundle": BUNDLE,
                       "installed": False, "launched": False}

    def cli(self, label, args, timeout=45):
        path = self.folder / (label + ".json")
        result = run(["xcrun", "devicectl", *args, "--timeout", str(timeout),
                      "--json-output", str(path)], timeout=timeout + 10)
        (self.folder / (label + ".log")).write_bytes(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(f"{label} 실패: {label}.log 확인. 후속 작업을 중단했습니다.")
        payload = json.loads(path.read_text())
        if payload.get("info", {}).get("outcome") != "success":
            raise RuntimeError(f"{label}: devicectl 성공 응답이 없습니다.")
        return payload["result"]

    def artifact(self, app):
        if not (app / "Info.plist").is_file():
            raise RuntimeError(f"앱 산출물이 아직 없습니다: {app}")
        info = plistlib.loads((app / "Info.plist").read_bytes())
        if info.get("CFBundleIdentifier") != BUNDLE:
            raise RuntimeError("bundle ID가 검증 앱과 다릅니다. 설치하지 않습니다.")
        if info.get("DTPlatformName") != "iphoneos":
            raise RuntimeError("실제 iPhone용 산출물이 아닙니다.")
        executable = app / info["CFBundleExecutable"]
        self.report["artifact"] = {"path": str(app), "executable_sha256":
            hashlib.sha256(executable.read_bytes()).hexdigest(),
            "signature_verified": False, "platform": "iphoneos"}
        hashes = code_hashes(app)
        self.report["artifact"].update(code_sha256=hashes, code_manifest_sha256=
            hashlib.sha256(json.dumps(hashes, sort_keys=True).encode()).hexdigest())
        result = run(["codesign", "--verify", "--deep", "--strict", str(app)])
        if result.returncode:
            raise RuntimeError("앱 서명 검증 실패. 설치하지 않습니다.")
        result = run(["lipo", "-archs", str(executable)])
        if result.returncode or "arm64" not in result.stdout.decode().split():
            raise RuntimeError("arm64 실행 파일이 없습니다.")
        # Parse provisioning in memory only: never emit certificate/profile contents.
        result = run(["security", "cms", "-D", "-i", str(app / "embedded.mobileprovision")])
        if result.returncode:
            raise RuntimeError("개발 기기 프로파일을 확인하지 못했습니다.")
        profile = plistlib.loads(result.stdout)
        expiry = profile["ExpirationDate"].replace(tzinfo=dt.timezone.utc)
        if expiry <= dt.datetime.now(dt.timezone.utc):
            raise RuntimeError("서명 프로파일이 만료됐습니다. 재빌드가 필요합니다.")
        if DEVICE not in profile.get("ProvisionedDevices", []):
            raise RuntimeError("지정한 Air가 서명 프로파일에 없습니다.")
        if not profile.get("Entitlements", {}).get("application-identifier", "").endswith("." + BUNDLE):
            raise RuntimeError("서명 프로파일의 앱 식별자가 다릅니다.")
        result = run(["codesign", "-d", "--entitlements", ":-", str(app)])
        entitlements = plistlib.loads(result.stdout) if result.returncode == 0 and result.stdout else {}
        if entitlements.get("application-identifier") != profile["Entitlements"]["application-identifier"]:
            raise RuntimeError("실제 코드 서명의 앱 식별자가 프로파일과 다릅니다.")
        if not entitlements.get("get-task-allow"):
            raise RuntimeError("Instruments 연결용 개발 서명이 아닙니다.")
        self.report["artifact"].update(signature_verified=True, profile_expires=expiry.isoformat())

    def build(self, destination):
        derived = Path("/tmp/heavymach-direction-device")
        built = derived / "Build/Products/Debug-iphoneos/HeavyMachValidation.app"
        if destination.name != "HeavyMachValidation.app" or destination == built:
            raise RuntimeError("build 보존 경로는 빌드 출력과 다른 HeavyMachValidation.app이어야 합니다.")
        command = ["xcodebuild", "build", "-project", str(ROOT / "iOS/HeavyMachRestored.xcodeproj"),
                   "-scheme", "HeavyMachValidation", "-configuration", "Debug",
                   "-destination", "generic/platform=iOS", "-derivedDataPath", str(derived),
                   "CODE_SIGNING_ALLOWED=YES", "CODE_SIGNING_REQUIRED=YES"]
        # Never prompt for credentials or change keychain settings here.
        with (self.folder / "signed-build.log").open("wb") as log:
            result = subprocess.run(command, stdout=log, stderr=subprocess.STDOUT,
                                    timeout=600, check=False)
        if result.returncode:
            raise RuntimeError("서명 빌드 실패: signed-build.log 확인. 키체인이 잠겼다면 사용자가 잠금을 해제한 뒤 build를 다시 실행하세요.")
        self.artifact(built)
        destination.parent.mkdir(parents=True, exist_ok=True)
        if destination.exists():
            shutil.move(str(destination), str(self.folder / "previous-HeavyMachValidation.app"))
        shutil.copytree(built, destination, symlinks=True)
        self.artifact(destination)

    def connected(self):
        devices = self.cli("devices", ["list", "devices"], 15)["devices"]
        for device in devices:
            properties = device.get("properties", {})
            hardware = properties.get("hardware", device.get("hardwareProperties", {}))
            if hardware.get("udid") != DEVICE:
                continue
            if hardware.get("reality") != "physical" or hardware.get("productType") != "iPhone18,4":
                raise RuntimeError("대상 UDID의 기기 종류가 지정한 실제 Air와 다릅니다.")
            connection = properties.get("connection", {})
            state = connection.get("state", device.get("connectionProperties", {}).get("tunnelState"))
            self.report["connection"] = state
            return state == "connected"
        self.report["connection"] = "not-listed"
        return False

    def installed_app(self):
        result = self.cli("apps", ["device", "info", "apps", "--device", DEVICE,
                                   "--include-all-apps", "--bundle-id", BUNDLE])
        apps = result["apps"]
        # Do not infer absence from a failed query or an unrecognised response.
        if not isinstance(apps, list):
            raise RuntimeError("설치 앱 응답 형식을 확인하지 못했습니다.")
        matches = [app for app in apps if app.get("bundleIdentifier") == BUNDLE]
        self.report["existing_validation_app"] = bool(matches)
        return bool(matches)

    def export(self, label):
        destination = self.folder / label
        self.cli(label, ["device", "copy", "from", "--device", DEVICE,
                        "--domain-type", "appDataContainer", "--domain-identifier", BUNDLE,
                        "--source", "Documents", "--destination", str(destination)])
        self.report[label] = str(destination)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", nargs="?", default="check", choices=["check", "build", "install", "launch", "export"])
    parser.add_argument("--app", type=Path, default=DEFAULT_APP)
    parser.add_argument("--dry-run", action="store_true", help="검사만 수행하며 설치/실행/백업하지 않음")
    args = parser.parse_args()
    action = "check" if args.dry_run else args.action
    session = Session(action)
    status = 0
    try:
        if action == "build":
            session.build(args.app.resolve())
            print("서명 빌드와 산출물 검사를 통과했습니다. 설치/실행하지 않았습니다.")
            return 0
        # Inspect connectivity even when the build has not arrived yet.
        connected = session.connected()
        session.artifact(args.app.resolve())
        if not connected:
            print("산출물 검사는 통과했습니다. 지정한 실제 Air가 연결되지 않아 설치/실행하지 않았습니다.")
            status = 2
        else:
            existing = session.installed_app()
            if action == "check":
                print("실제 Air 연결 및 산출물 검사를 통과했습니다. 설치/실행하지 않았습니다.")
            elif action == "install":
                if existing:
                    session.export("validationDocuments-before-install")
                else:
                    session.report["installation_kind"] = "new-validation-install"
                session.cli("install", ["device", "install", "app", "--device", DEVICE, str(args.app.resolve())])
                session.report["installed"] = True
                session.cli("launch", ["device", "process", "launch", "--device", DEVICE, BUNDLE])
                session.report["launched"] = True
                print("검증 앱 설치와 실행에 성공했습니다. 실제 플레이 검사는 아직 수행하지 않았습니다.")
            elif not existing:
                raise RuntimeError("검증 앱이 설치돼 있지 않습니다. install 명령을 먼저 실행하세요.")
            elif action == "launch":
                session.cli("launch", ["device", "process", "launch", "--device", DEVICE, BUNDLE])
                session.report["launched"] = True
                print("기존 검증 앱을 실행했습니다. 재설치하지 않았습니다.")
            elif action == "export":
                session.export("validationDocuments-after-test")
                print("검증 앱 Documents를 Mac에 복사했습니다.")
    except (RuntimeError, OSError, ValueError, KeyError, subprocess.TimeoutExpired) as error:
        session.report["error"] = str(error)
        print(str(error), file=sys.stderr)
        status = 1
    finally:
        session.report["exit_code"] = status
        (session.folder / "summary.json").write_text(json.dumps(session.report, ensure_ascii=False, indent=2) + "\n")
        print(f"기록: {session.folder}")
    return status


if __name__ == "__main__":
    sys.exit(main())
