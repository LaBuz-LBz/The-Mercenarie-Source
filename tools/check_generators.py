"""Regenerate review inputs in a temporary directory; never edit source or game files."""
from pathlib import Path
import json
import re
import runpy
import tempfile
import hashlib

ROOT = Path(__file__).resolve().parents[1]
INPUT = ROOT / 'generators/inputs'

def load(name):
    return json.loads((INPUT/name).read_text(encoding='utf-8'))

def regenerate():
    result = {}
    with tempfile.TemporaryDirectory(prefix='mercenarie-generators-') as name:
        tmp = Path(name)
        data = load('localization-editable.json')
        def row(match):
            item = data['rows'][int(match.group(1))]
            value = ('"' + ''.join('\\x%02X' % b for b in item['value'].encode('utf-8')) + '"'
                     if item['encoding'] == 'hex' else json.dumps(item['value']))
            return '{' + json.dumps(item['key']) + ',' + value + '},'
        text = re.sub(r'@@ROW_(\d+)@@', row, data['template'])
        (tmp/'LocalizationDefaults.generated.h').write_bytes(text.encode('ascii'))
        ids = load('cleanup-owned-ids.json')
        text = ('// Exact additions audited read-only against base/Newwworld/rebirth.\n'
                '// Includes historical IDs shipped inside Guild Escort Contracts.mod.\n'
                + ''.join('"' + i + '",\n' for i in ids))
        original = (ROOT/'source/CleanupOwnedDefinitions.generated.h').read_bytes()
        if b'\r\n' in original:
            text = text.replace('\n', '\r\n')
        (tmp/'CleanupOwnedDefinitions.generated.h').write_bytes(text.encode('utf-8'))
        (tmp/'docs').mkdir()
        (tmp/'tools').mkdir()
        (tmp/'docs/real-estate-v9-fcs-audit.json').write_text(
            json.dumps(load('estate-audit-projection.json')), encoding='utf-8')
        script = tmp/'tools/generate.py'
        script.write_bytes((ROOT/'generators/historical/estate-generator-excerpt.py').read_bytes())
        runpy.run_path(str(script), run_name='__main__')
        for filename in ['LocalizationDefaults.generated.h', 'CleanupOwnedDefinitions.generated.h',
                         'RealEstateBaseline.generated.h']:
            actual = (tmp/filename).read_bytes()
            expected = (ROOT/'source'/filename).read_bytes()
            result[filename] = {'exact': actual == expected,
                                'sha256': hashlib.sha256(actual).hexdigest()}
    print(json.dumps(result, indent=2))
    if not all(item['exact'] for item in result.values()):
        raise SystemExit(1)

if __name__ == '__main__':
    regenerate()
