#! /usr/bin/env python

from shutil import copy, copytree, rmtree, copyfile
import os
from subprocess import call

f = open('files.txt', 'r')
lines = f.readlines()
f.close()

triplet='i686-w64-mingw32'
prefix='/usr/' + triplet + '/sys-root/mingw/'

dest = os.path.abspath (os.path.dirname(__file__) + '/binaries/Xiphos/') + '/'
rmtree(dest)
os.mkdir(dest)


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

programs = [
	'addld.exe',
	'diatheke.exe',
 	'imp2gbs.exe',
	'imp2ld.exe',
	'imp2vs.exe',
	'installmgr.exe',
	'mkfastmod.exe',
	'mod2imp.exe',
	'mod2osis.exe',
	'mod2vpl.exe',
	'mod2zmod.exe',
	'osis2mod.exe',
	'tei2mod.exe',
	'vpl2mod.exe',
	'vs2osisref.exe',
	]

for p in programs:
	copyfile(prefix + 'bin/' + triplet + '-' + p, dest + 'bin/' + p)
	call('strip ' + dest + 'bin/' + p, shell=True)

libraries = [
	'icudata46.dll',
	'icui18n46.dll',
	'icuio46.dll',
	'icule46.dll',
	'iculx46.dll',
	'icutu46.dll',
	'icuuc46.dll',
	'libclucene-0.dll',
	'libcurl-4.dll',
	'libgcc_s_sjlj-1.dll',
	'libgcrypt-11.dll',
	'libgnurx-0.dll',
	'libgpg-error-0.dll',
	'libidn-11.dll',
	'libnspr4.dll',
	'libplc4.dll',
	'libplds4.dll',
	'libssh2-1.dll',
	'libstdc++-6.dll',
	'libsword-1-6-2.dll',
	'nss3.dll',
	'nssutil3.dll',
	'ssl3.dll',
	'zlib1.dll',
	'libglib-2.0-0.dll',
	'libgio-2.0-0.dll',
	]

for l in libraries:
	copyfile(prefix + 'bin/' + l, dest + 'bin/' + l)
	call('strip ' + dest + 'bin/' + p, shell=True)

try:
	os.remove('xiphos-dist.zip')
except:
	pass
import urllib
urllib.urlretrieve("http://ftp.xiphos.org/sword/xiphos-dist/xiphos-dist.zip", "xiphos-dist.zip")

from zipfile import ZipFile
z = ZipFile('xiphos-dist.zip', 'r')
z.extractall(dest)
z.close()

os.chdir('nsis')
call('wine /home/matthew/NSIS/Unicode/makensis.exe ' + 'installer.nsi', shell=True)
os.chdir('..')



