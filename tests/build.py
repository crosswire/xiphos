#!/usr/bin/python

import os
import subprocess
import itertools
import unittest

class BuildTests(unittest.TestCase):
    def setUp(self):
        self.fnull = open(os.devnull,'w')

    def tearDown(self):
        self._run(["./waf","distclean"])
        self.fnull.close()

    def _run(self, args):
        sub = subprocess.Popen(args, stderr=self.fnull, stdout=self.fnull)
        sub.communicate()
        return sub
        
    def _configure(self, gtk, backend):
        sub = self._run(["./waf","configure","--gtk", gtk, "--backend", backend, "--disable-dbus"])
        self.assertEqual(0,sub.returncode)

    def _build(self):
        sub = self._run(["./waf","build"])
        self.assertEqual(0,sub.returncode)

    def _configure_and_build(self, gtk, backend):
        self._configure(gtk, backend)
        self._build()

gtks = ("2","3")
backs= ("webkit","gtkhtml","xulrunner")

for gtk, backend in itertools.product(gtks,backs):
    if gtk == "3" and backend == "xulrunner": continue
    def ch(gtk, backend):
        return lambda self: self._configure_and_build(gtk, backend)
    setattr(BuildTests, "test_%s_%s" % (gtk, backend), ch(gtk, backend))

suite = unittest.TestLoader().loadTestsFromTestCase(BuildTests)
unittest.TextTestRunner(verbosity=2).run(suite)
