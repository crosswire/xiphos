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
        
    def _configure(self, gtk, backend, delint):
        arglist = ["./waf","configure","--gtk", gtk, "--backend", backend,
                   "--disable-dbus"]
        if delint:
            arglist.append("--enable-delint")
        sub = self._run(arglist)
        self.assertEqual(0,sub.returncode)

    def _build(self):
        sub = self._run(["./waf","build"])
        self.assertEqual(0,sub.returncode)

    def _configure_and_build(self, gtk, backend, delint):
        self._configure(gtk, backend, delint)
        self._build()

gtks = ("2","3")
backs= ("webkit","gtkhtml","xulrunner")
delit= ("delint_", '')

for gtk, backend, delint in itertools.product(gtks,backs,delit):
    if gtk == "3" and backend == "xulrunner": continue
    def ch(gtk, backend, delint):
        return lambda self: self._configure_and_build(gtk, backend, delint)
    setattr(BuildTests, "test_%s%s_%s" % (delint, gtk, backend), ch(gtk, backend, delint))

suite = unittest.TestLoader().loadTestsFromTestCase(BuildTests)
unittest.TextTestRunner(verbosity=2).run(suite)
