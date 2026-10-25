#!/usr/bin/env python3
"""Exercise pre-push policy against isolated real Git histories."""
import os, subprocess, tempfile, unittest
from pathlib import Path
GUARD=Path(__file__).resolve().parents[1]/'.githooks/check-branch-flow.py'
ZERO='0'*40
class FlowTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory();self.root=Path(self.tmp.name)
        self.git('init','-q','-b','main');self.git('config','user.name','Test');self.git('config','user.email','test@example.invalid')
        self.git('commit','-q','--allow-empty','-m','bootstrap');self.initial=self.git('rev-parse','HEAD')
        self.git('branch','develop')
    def tearDown(self):self.tmp.cleanup()
    def git(self,*args):return subprocess.check_output(['git',*args],cwd=self.root,stderr=subprocess.DEVNULL,text=True).strip()
    def run_guard(self,ref,new,old=ZERO):
        return subprocess.run(['python3',str(GUARD)],cwd=self.root,input=f'{ref} {new} {ref} {old}\n',text=True,capture_output=True).returncode
    def release(self):
        self.git('switch','-q','-c','feat/test','develop');(self.root/'file').write_text('feature')
        self.git('add','file');self.git('commit','-q','-m','feature')
        self.git('switch','-q','develop');self.git('merge','-q','--no-ff','feat/test','-m','develop merge')
        self.git('switch','-q','main');self.git('merge','-q','--no-ff','develop','-m','main merge')
        return self.git('rev-parse','HEAD')
    def test_empty_bootstrap(self):self.assertEqual(self.run_guard('refs/heads/main',self.initial),0)
    def test_direct_commit_rejected(self):
        self.git('commit','-q','--allow-empty','-m','direct')
        self.assertNotEqual(self.run_guard('refs/heads/main',self.git('rev-parse','HEAD'),self.initial),0)
    def test_feature_merge_rejected(self):
        self.git('switch','-q','-c','feat/test');self.git('commit','-q','--allow-empty','-m','feature');self.git('switch','-q','main');self.git('merge','-q','--no-ff','feat/test','-m','wrong merge')
        self.assertNotEqual(self.run_guard('refs/heads/main',self.git('rev-parse','HEAD'),self.initial),0)
    def test_develop_merge_allowed(self):self.assertEqual(self.run_guard('refs/heads/main',self.release(),self.initial),0)
    def test_protected_branch_deletion(self):self.assertNotEqual(self.run_guard('refs/heads/develop',ZERO,self.initial),0)
    def test_tag_main_merge_allowed(self):
        self.release();self.git('tag','-a','v0.9','-m','release')
        self.assertEqual(self.run_guard('refs/tags/v0.9',self.git('rev-parse','v0.9')),0)
    def test_tag_replacement_rejected(self):
        self.release();self.git('tag','-a','v0.9','-m','release')
        self.assertNotEqual(self.run_guard('refs/tags/v0.9',self.git('rev-parse','v0.9'),self.initial),0)
    def test_tag_feature_rejected(self):
        self.release();self.git('tag','-a','v0.9','-m','wrong','feat/test')
        self.assertNotEqual(self.run_guard('refs/tags/v0.9',self.git('rev-parse','v0.9')),0)
if __name__=='__main__':unittest.main()
