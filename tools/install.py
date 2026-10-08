"""Explicit, opt-in DLL installation into an existing complete mod installation."""
import argparse
import hashlib
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--mod-dir', required=True, type=Path)
parser.add_argument('--confirm-install', action='store_true')
args = parser.parse_args()
if not args.confirm_install:
    raise SystemExit('No changes: pass --confirm-install after reading BUILDING.md')
target = args.mod_dir.resolve()
if target.name != 'Guild Escort Contracts' or not (target/'Guild Escort Contracts.mod').is_file():
    raise SystemExit('Target must be an existing complete Guild Escort Contracts mod directory')
processes = subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True)
if any(line.lower().startswith('"kenshi') for line in processes.splitlines()):
    raise SystemExit('Close Kenshi before installation')
src = ROOT/'_build/GuildEscortContracts.dll'
if not src.is_file():
    raise SystemExit('Build the DLL first')
dst = target/'GuildEscortContracts.dll'
if dst.exists():
    digest = hashlib.sha256(dst.read_bytes()).hexdigest()
    backup = ROOT/'_build/backups'/digest
    backup.mkdir(parents=True, exist_ok=True)
    shutil.copy2(dst, backup/dst.name)
shutil.copy2(src, dst)
assert hashlib.sha256(src.read_bytes()).digest() == hashlib.sha256(dst.read_bytes()).digest()
print('DLL installed; other mod files and user settings were not modified.')
