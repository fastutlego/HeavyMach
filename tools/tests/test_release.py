"""Release fixtures use temporary repositories and an in-memory GitHub server."""
import copy
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tarfile
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import release


class FakeGitHub:
    def __init__(self, manifest):
        self.manifest = manifest
        self.remote = manifest['commit']
        self.release = None
        self.calls = []
        self.fail_upload = False
        self.bad_digest = False
        self.change_tag = False

    def remote_commit(self, version):
        return self.remote

    def find(self, version):
        return copy.deepcopy(self.release)

    def create(self, version, notes):
        self.calls.append('create')
        self.release = dict(id=42, name='HeavyMach ' + version, tag_name=version, draft=True, prerelease=False,
                            body=notes.read_text(), assets=[], html_url='https://example.test/release')

    def upload(self, version, paths):
        self.calls.append('upload')
        for path in paths:
            self.release['assets'].append(dict(name=path.name, size=path.stat().st_size,
                digest='sha256:' + ('0' * 64 if self.bad_digest else release.sha(path)), state='uploaded'))
            if self.fail_upload:
                raise release.ReleaseError('simulated connection failure after first upload')
        if self.change_tag:
            self.remote = 'f' * 40

    def publish(self, version):
        self.calls.append('publish')
        self.release['draft'] = False


class ReleaseTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / 'repo'
        self.root.mkdir()
        self.out = Path(self.temp.name) / 'output'
        self.git('init', '-b', 'main')
        self.git('config', 'user.name', 'Release Test')
        self.git('config', 'user.email', 'release@example.test')
        self.git('config', 'commit.gpgsign', 'false')
        self.git('config', 'tag.gpgsign', 'false')
        self.git('remote', 'add', 'origin', 'https://github.com/example/HeavyMach.git')
        for name in release.NOTICES:
            (self.root / name).write_text('fixture notice\n')
        (self.root / 'docs/public').mkdir(parents=True)
        (self.root / 'docs/public/build-and-install.md').write_text('fixture installation')
        (self.root / 'CHANGELOG.md').write_text('# Changes\n## v0.17 — 2026-10-29\n\nNew game. [Install](docs/public/build-and-install.md#build)\n\n<a id="v016"></a>\n## v0.16 — old\nOld game.\n')
        (self.root / 'Resources').mkdir()
        content = b'actual asset bytes\x00'
        self.digest = hashlib.sha256(content).hexdigest()
        pointer = release.POINTER + f'oid sha256:{self.digest}\nsize {len(content)}\n'.encode()
        (self.root / 'Resources/asset.bin').write_bytes(pointer)
        self.object = self.root / '.git/lfs/objects' / self.digest[:2] / self.digest[2:4] / self.digest
        self.object.parent.mkdir(parents=True)
        self.object.write_bytes(content)
        (self.root / 'docs/build-resources.json').write_text(json.dumps({'roots': ['Resources'], 'files': [
            {'path': 'Resources/asset.bin', 'bytes': len(content), 'sha256': self.digest}]}))
        script = self.root / 'build.sh'
        script.write_text('echo build\n')
        script.chmod(0o755)
        (self.root / 'install-link').symlink_to('docs/public/build-and-install.md')
        self.git('add', '.')
        self.git('commit', '-m', 'initial')
        self.git('switch', '-c', 'develop')
        (self.root / 'feature.txt').write_text('tagged feature')
        self.git('add', '.')
        self.git('commit', '-m', 'feature')
        self.git('switch', 'main')
        self.git('merge', '--no-ff', 'develop', '-m', 'release merge')
        self.git('update-ref', 'refs/remotes/origin/main', 'HEAD')
        self.git('tag', '-a', 'v0.17', '-m', 'release')

    def git(self, *args):
        result = subprocess.run(['git', *args], cwd=self.root, capture_output=True, check=True)
        return result.stdout.decode().strip()

    def prepare(self):
        return release.prepare(self.root, 'v0.17', self.out)

    def test_exact_tag_excludes_dirty_and_untracked(self):
        (self.root / 'feature.txt').write_text('uncommitted edit')
        (self.root / '.env').write_text('private fixture')
        manifest = self.prepare()
        with tarfile.open(self.out / 'HeavyMach-v0.17-source-with-assets.tar.gz') as tar:
            self.assertEqual(tar.extractfile('HeavyMach-v0.17/feature.txt').read(), b'tagged feature')
            self.assertNotIn('HeavyMach-v0.17/.env', tar.getnames())
            self.assertEqual(tar.getmember('HeavyMach-v0.17/build.sh').mode, 0o755)
            self.assertEqual(tar.extractfile('HeavyMach-v0.17/Resources/asset.bin').read(), self.object.read_bytes())
        self.assertEqual(manifest['checks']['build_assets'], 1)
        notes = (self.out / 'release-notes.md').read_text()
        self.assertNotIn('Old game', notes)
        self.assertNotIn('<a id=', notes)
        self.assertIn('/blob/v0.17/docs/public/build-and-install.md#build', notes)

    def test_missing_lfs_does_not_leave_output(self):
        self.object.unlink()
        with self.assertRaisesRegex(release.ReleaseError, 'Missing LFS'):
            self.prepare()
        self.assertFalse(self.out.exists())

    def test_corrupt_lfs_rejected(self):
        self.object.write_bytes(b'wrong bytes')
        with self.assertRaisesRegex(release.ReleaseError, 'LFS content mismatch'):
            self.prepare()

    def test_existing_output_preserved(self):
        self.out.mkdir()
        (self.out / 'keep').write_text('keep')
        with self.assertRaisesRegex(release.ReleaseError, 'already exists'):
            self.prepare()
        self.assertEqual((self.out / 'keep').read_text(), 'keep')

    def test_bad_version_and_non_release_tag(self):
        with self.assertRaises(release.ReleaseError):
            release.tag_info(self.root, '../v0.17')
        self.git('tag', 'v0.18')
        with self.assertRaisesRegex(release.ReleaseError, 'annotated'):
            release.tag_info(self.root, 'v0.18')
        self.git('tag', '-a', 'v0.19', '-m', 'not merged', 'HEAD^')
        with self.assertRaisesRegex(release.ReleaseError, 'merge commit'):
            release.tag_info(self.root, 'v0.19')

    def test_notes_missing_target_and_reference_links(self):
        with self.assertRaisesRegex(release.ReleaseError, 'link missing'):
            release.extract_notes('## v0.17\n[bad](missing.md)', 'v0.17', 'x/y', set())
        body, links = release.extract_notes('## v0.17\n[doc][d]\n[d]: docs/a.md\n[web](https://example.test/x)',
                                           'v0.17', 'x/y', {'docs/a.md'})
        self.assertIn('[d]: https://github.com/x/y/blob/v0.17/docs/a.md', body)
        self.assertIn('[web](https://example.test/x)', body)
        self.assertEqual(links, ['docs/a.md'])

    def test_publish_success(self):
        api = FakeGitHub(self.prepare())
        receipt = release.publish(self.root, 'v0.17', self.out, github=api)
        self.assertEqual(api.calls, ['create', 'upload', 'publish'])
        self.assertTrue(receipt['published'])

    def test_check_only_has_no_remote_writes(self):
        api = FakeGitHub(self.prepare())
        release.publish(self.root, 'v0.17', self.out, check_only=True, github=api)
        self.assertEqual(api.calls, [])
        self.assertFalse((self.out / 'publish-result.json').exists())

    def test_existing_public_release_untouched(self):
        api = FakeGitHub(self.prepare())
        api.release = {'draft': False}
        with self.assertRaisesRegex(release.ReleaseError, 'already has a published'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        self.assertEqual(api.calls, [])

    def test_tampered_archive_and_notes_stop_before_network(self):
        api = FakeGitHub(self.prepare())
        notes = self.out / 'release-notes.md'
        original = notes.read_bytes()
        notes.write_text('changed')
        with self.assertRaisesRegex(release.ReleaseError, 'notes changed'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        notes.write_bytes(original)
        (self.out / 'SHA256SUMS.txt').write_text('changed')
        with self.assertRaisesRegex(release.ReleaseError, 'attachment changed'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        self.assertEqual(api.calls, [])

    def test_remote_tag_mismatch(self):
        api = FakeGitHub(self.prepare())
        api.remote = 'a' * 40
        with self.assertRaisesRegex(release.ReleaseError, 'Remote tag differs'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        self.assertEqual(api.calls, [])

    def test_tag_changed_during_upload_keeps_draft(self):
        api = FakeGitHub(self.prepare())
        api.change_tag = True
        with self.assertRaisesRegex(release.ReleaseError, 'changed during upload'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        self.assertTrue(api.release['draft'])
        self.assertNotIn('publish', api.calls)

    def test_corrupt_remote_asset_keeps_draft(self):
        api = FakeGitHub(self.prepare())
        api.bad_digest = True
        with self.assertRaisesRegex(release.ReleaseError, 'checksum/state mismatch'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        self.assertTrue(api.release['draft'])
        self.assertNotIn('publish', api.calls)

    def test_upload_failure_resumes_without_duplicate_assets(self):
        api = FakeGitHub(self.prepare())
        api.fail_upload = True
        with self.assertRaisesRegex(release.ReleaseError, 'simulated connection'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        receipt = json.loads((self.out / 'publish-result.json').read_text())
        self.assertEqual(receipt['stage'], 'upload')
        self.assertFalse(receipt['published'])
        self.assertTrue(api.release['draft'])
        api.fail_upload = False
        release.publish(self.root, 'v0.17', self.out, resume=True, github=api)
        self.assertEqual(len(api.release['assets']), 3)
        self.assertEqual(api.calls.count('create'), 1)
        self.assertEqual(api.calls.count('publish'), 1)

    def test_unowned_draft_is_untouched(self):
        api = FakeGitHub(self.prepare())
        api.create('v0.17', self.out / 'release-notes.md')
        api.calls.clear()
        with self.assertRaisesRegex(release.ReleaseError, 'Existing draft'):
            release.publish(self.root, 'v0.17', self.out, resume=True, github=api)
        self.assertEqual(api.calls, [])

    def test_publication_response_failure_records_unknown_state(self):
        api = FakeGitHub(self.prepare())
        original = api.publish
        def fail_after_publish(version):
            original(version)
            raise release.ReleaseError('connection lost after publication')
        api.publish = fail_after_publish
        with self.assertRaisesRegex(release.ReleaseError, 'connection lost'):
            release.publish(self.root, 'v0.17', self.out, github=api)
        receipt = json.loads((self.out / 'publish-result.json').read_text())
        self.assertIsNone(receipt['published'])
        self.assertEqual(receipt['stage'], 'publish')
        self.assertFalse(api.release['draft'])

    def test_api_failure_is_not_treated_as_missing_release(self):
        api = release.GitHub(self.root, 'x/y')
        with patch.object(api, 'cli', side_effect=release.ReleaseError('authentication failed')):
            with self.assertRaisesRegex(release.ReleaseError, 'authentication'):
                api.find('v0.17')


if __name__ == '__main__':
    unittest.main()
