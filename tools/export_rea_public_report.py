#!/usr/bin/env python3
"""Build public metadata appendices from retained REA evidence, without raw code or local paths."""
import argparse
import collections
import csv
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def nested(value):
    if isinstance(value, dict):
        yield value
        for child in value.values():
            yield from nested(child)
    elif isinstance(value, list):
        for child in value:
            yield from nested(child)
    elif isinstance(value, str) and value.lstrip().startswith('{'):
        try:
            yield from nested(json.loads(value))
        except ValueError:
            pass


def csvfile(out, name, fields, rows):
    with (out / name).open('w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive-root', type=Path, required=True)
    parser.add_argument('--title-root', type=Path, required=True)
    args = parser.parse_args()
    out = ROOT / 'docs/public/rea'
    out.mkdir(parents=True, exist_ok=True)
    roots = {'analysis': ROOT / 'Analysis', 'archive-evidence': args.archive_root,
             'title-evidence': args.title_root, 'resource-evidence': ROOT / 'Resources/Evidence'}
    evidence = {}
    artifacts = []
    errors = []
    for label, root in roots.items():
        for path in sorted(root.rglob('*.json')):
            try:
                value = json.loads(path.read_text())
            except (ValueError, UnicodeError):
                continue
            alias = label + '/' + str(path.relative_to(root))
            found = set()
            for item in nested(value):
                eid, operation = item.get('evidence_id'), item.get('operation')
                if isinstance(eid, str) and eid.startswith('ev_') and isinstance(operation, str):
                    subject = item.get('subject') or {}
                    digest = (subject.get('digest') or {}).get('sha256', '')
                    entry = evidence.setdefault(eid, dict(operation=operation, target_sha256=digest,
                                                         locations=set(), complete_payload=False))
                    entry['locations'].add(alias)
                    entry['complete_payload'] |= 'raw_result' in item
                    found.add(eid)
            # Failed top-level requests have no Evidence ID; retain their identity too.
            if isinstance(value, dict) and (value.get('isError') or 'error' in value or
                                           value.get('code') in ('execution_failure', 'invalid_request')):
                errors.append({'artifact': alias, 'sha256': sha(path), 'evidence_count': len(found)})
            artifacts.append({'artifact': alias, 'sha256': sha(path), 'bytes': path.stat().st_size,
                              'evidence_ids': ';'.join(sorted(found))})
    rows = [{'evidence_id': key, 'operation': v['operation'], 'target_sha256': v['target_sha256'],
             'complete_payload_retained': v['complete_payload'], 'retained_at': ';'.join(sorted(v['locations']))}
            for key, v in sorted(evidence.items())]
    csvfile(out, 'evidence-index.csv', list(rows[0]), rows)
    csvfile(out, 'artifact-index.csv', ['artifact', 'sha256', 'bytes', 'evidence_ids'], artifacts)
    csvfile(out, 'failed-results.csv', ['artifact', 'sha256', 'evidence_count'], errors)
    manifest = json.loads((ROOT / 'Analysis/export-manifest.json').read_text())
    coverage = json.loads((ROOT / 'Analysis/source-coverage.json').read_text())
    families = {v['address']: v.get('family') for v in coverage['procedures']}
    rows = []
    for record in manifest['functions']:
        exports = record.get('exports', {})
        rows.append({'address': record['address'], 'symbol': record['value'],
                     'classification': record.get('classification', record.get('status', '')),
                     'family_at_audit': families.get(record['address']) or '',
                     'pseudo_status': exports.get('pseudo', {}).get('status', ''),
                     'pseudo_evidence_id': exports.get('pseudo', {}).get('evidence_id', ''),
                     'assembly_status': exports.get('assembly', {}).get('status', ''),
                     'assembly_evidence_id': exports.get('assembly', {}).get('evidence_id', '')})
    csvfile(out, 'original-procedures.csv', list(rows[0]), rows)
    # Retain the comparison build's entire inventory, not only 19 selected pairs.
    source = json.loads((args.archive_root / 'comparison/core/assault-procedures.json').read_text())
    inventory = next(v for v in nested(source) if v.get('operation') == 'list_procedures' and 'raw_result' in v)
    assault_rows = [{'address': v['address'], 'symbol': v['value'],
                    'external': v['procedure']['external'], 'thunk': v['procedure']['thunk']}
                   for v in inventory['raw_result']]
    csvfile(out, 'assault-procedures.csv', list(assault_rows[0]), assault_rows)
    family_snapshot = {'scope': 'Historical source-coverage audit; consult current report for later changes.',
                       'original_sha256': coverage['original_sha256'], 'families': coverage['families']}
    (out / 'family-audit-snapshot.json').write_text(json.dumps(family_snapshot, ensure_ascii=False, indent=2)+'\n')
    docs = []
    for path in sorted((ROOT / 'docs').rglob('*.md')):
        if 'public/rea/' in str(path):
            continue
        title = next((line.lstrip('# ') for line in path.read_text().splitlines() if line.startswith('# ')), path.stem)
        docs.append({'document': str(path.relative_to(ROOT)), 'title': title, 'sha256': sha(path)})
    csvfile(out, 'document-index.csv', ['document', 'title', 'sha256'], docs)
    summary = {'original_sha256': manifest['source_sha256'], 'original_inventory': len(rows),
               'classification_counts': manifest['classification_counts'], 'export_failures': len(manifest['failures']),
               'metadata_failures': len(manifest['metadata_failures']), 'assault_inventory': len(assault_rows),
               'families': len(coverage['families']), 'retained_json_artifacts': len(artifacts),
               'unique_evidence_ids': len(evidence), 'operations': dict(sorted(collections.Counter(v['operation'] for v in evidence.values()).items())),
               'failed_top_level_results': len(errors), 'documents_indexed': len(docs),
               'scope': 'Retained records in four declared roots. Unique IDs are not tool call counts. Indexes include summaries and failures; original source/runtime completeness is not inferred.'}
    (out / 'inventory-summary.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2)+'\n')
    print(json.dumps(summary, ensure_ascii=False))


if __name__ == '__main__':
    main()
