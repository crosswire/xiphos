#! /usr/bin/env python

from shutil import copy, copytree
import os

f = open('files.txt', 'r')
lines = f.readlines()
f.close()

prefix = '/usr/i686-pc-mingw32/sys-root/mingw/'
dest = os.path.abspath (os.path.dirname(__file__) + '/binaries/Xiphos/') + '/'

for l in lines:
    if len(l) > 1 and (not l.startswith('d ') and not l.startswith('#')):
	filename = dest + l.strip('\n')
	dirs = filename.split('/')
	dir = '/'.join(dirs[:len(dirs)-1])
	try:
	  os.makedirs(dir)
	except:
	  pass
	try:
	  copy(prefix + l.strip('\n'), filename)
	except:
	  print 'file not found: ' + l.strip('\n')
	  
locales = dest + 'share/sword/'
try:
  os.makedirs(locales)
except:
  pass
copytree(prefix + 'share/sword/locales.d/', locales + 'locales.d')

