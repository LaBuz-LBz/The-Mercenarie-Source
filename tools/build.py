"""Build the plugin without accessing a Kenshi installation or deploying anything."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--vc', required=True, type=Path, help='Visual Studio 2010 VC directory')
    parser.add_argument('--sdk', required=True, type=Path, help='Windows SDK 7.1 directory')
    parser.add_argument('--allow-modified', action='store_true', help='Build deliberately modified source/dependencies')
    args = parser.parse_args()
    for manifest in ('source-sha256.json', 'dependency-sha256.json'):
        expected = json.loads((ROOT / 'provenance' / manifest).read_text())
        for name, digest in expected.items():
            p = ROOT / ('source' if manifest.startswith('source-') else '') / name
            if not p.is_file():
                raise SystemExit('Missing required input: ' + name)
            if not args.allow_modified and hashlib.sha256(p.read_bytes()).hexdigest() != digest:
                raise SystemExit('Input differs from build 109: ' + name)
    vc, sdk = args.vc.resolve(), args.sdk.resolve()
    compiler = vc / 'bin/amd64/cl.exe'
    if not compiler.is_file() or not (sdk / 'Include/Windows.h').is_file():
        raise SystemExit('Visual C++ 2010 x64 compiler or Windows SDK headers not found')
    out = ROOT / '_build'
    out.mkdir(exist_ok=True)
    env = os.environ.copy()
    env['INCLUDE'] = str(vc/'include') + ';' + str(sdk/'Include')
    env['LIB'] = str(vc/'lib/amd64') + ';' + str(sdk/'Lib/x64')
    env['PATH'] = str(vc/'bin/amd64') + ';' + env.get('PATH', '')
    # Ambient CL/LINK options must not silently alter the recorded build.
    for key in ('CL', '_CL_', 'LINK', '_LINK_'):
        env.pop(key, None)
    command = [str(compiler), '/nologo', '/O2', '/GL', '/MD', '/EHsc', '/LD',
               '/DUNICODE', '/D_UNICODE', '/showIncludes']
    command += ['/I' + str(ROOT/p) for p in ('third_party/KenshiLib/Include',
                'third_party/KenshiLib/Include/ogre', 'third_party/KenshiLib/Include/kenshi',
                'third_party/boost_1_60_0', 'compat', 'source/src')]
    command += [str(ROOT/'source/GuildEscort.cpp'), '/Fo'+str(out/'GuildEscort.obj'),
                '/link', '/LTCG', '/OPT:REF', '/OPT:ICF',
                '/MAP:'+str(out/'GuildEscortContracts.map'),
                '/LIBPATH:'+str(ROOT/'third_party/KenshiLib/Libraries'),
                '/LIBPATH:'+str(ROOT/'third_party/boost_1_60_0/stage/lib'),
                'KenshiLib.lib', 'OgreMain_x64.lib', 'MyGUIEngine_x64.lib',
                '/OUT:'+str(out/'GuildEscortContracts.dll')]
    result = subprocess.run(command, cwd=out, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (out/'build.log').write_bytes(result.stdout)
    (out/'command.json').write_text(json.dumps(command, indent=2))
    if result.returncode:
        print(result.stdout.decode(errors='replace')[-8000:])
        raise SystemExit(result.returncode)
    dll = out/'GuildEscortContracts.dll'
    report = {'success': True, 'sha256': hashlib.sha256(dll.read_bytes()).hexdigest(),
              'bytes': dll.stat().st_size, 'deployed': False, 'native_tested': False}
    (out/'result.json').write_text(json.dumps(report, indent=2))
    print(json.dumps(report))

if __name__ == '__main__':
    main()
