#!/usr/bin/env python

import urllib2
import zipfile
import os

#from os.path import abspath, dirname, join, getsize
#from os.path import exists
#from os import mkdir

PREFIX = '/opt/mingw/test'
CACHE = os.path.join(PREFIX, 'cache')
# runable from any location, not only CWD
SCRIPT_PREFIX = os.path.dirname(os.path.abspath(__file__))
PACKAGE_LIST = os.path.join(SCRIPT_PREFIX, 'gtkList')

def main():

    make_dir(PREFIX)
    make_dir(CACHE)
    f = open(PACKAGE_LIST)

    for line in f:
        url, filename = line.split()[0:2]
        packagename = filename.rsplit('.', 1)[0]

        # download
        if is_downloaded(filename):
            print "found in cache; skipped downloading " + filename
        else:
            # install
            package_to_uninstall = is_upgrade(packagename)
            if package_to_uninstall:
                print "upgrading package " + packagename
                uninstall_package(package_to_uninstall)
            download_package(url, filename)
            install_package(filename)

    f.close()

def make_dir(directory):

    if not os.path.exists(directory):
        os.mkdir(directory)
        print 'making dir', directory

def is_downloaded(package_file):
    '''tests package existence and its length;
    When a connection prolem occurs, then only a zero-length file
    with name package_file is created.'''

    cache_file = os.path.join(CACHE, package_file)
    if not os.path.isfile(cache_file): 
        return False
    elif not os.path.getsize(cache_file) > 0:
        return False

    return True

def uninstall_package(package_file):
    '''takes a cache/*.zip; it removes the zip file, then reads through the
    associated manifest file and removes every file listed. It leaves the
    directories alone'''

    os.remove(os.path.join(CACHE, package_file))
    filename = os.path.join(PREFIX, 'manifest', package_file.rsplit('.', 1)[0] + '.mft')
    manifest = open(filename)
    for line in manifest:
        to_remove = line.strip()
        path = os.path.join(PREFIX, to_remove)
        if (os.path.isfile(path)):
            os.remove(path)
    print 'removed package', package_file

def is_upgrade(package):
    ''' takes a package name like atk_1.26.0-1
    and looks in the cache directory to see if there is a
    match even if it has a different version number. If it
    does, return the package name plus version.
    If it doesn't, return empty string'''

    package_name = package.split('_', 1)[0]
    files = os.listdir(CACHE)
    packages = [f.split('_') for f in files]
    for p in packages:
        if package_name == p[0]:
            return '_'.join(p)
    return ''

def download_package(url, filename):
    '''downloads the url and writes to the cache
    + filename'''
    print 'downloading', url
    output = open(os.path.join(CACHE, filename), 'wb')
    output.write(urllib2.urlopen(url).read())
    output.close()

def install_package(filename):
    '''unzips the package to the prefix; handles the manifest
    files specially to make them match the zip file name
    exactly. Some older package names are in a different format.
    Skips already created directories'''
    
    z = zipfile.ZipFile(os.path.join(CACHE, filename))
    for f in z.namelist():
        if not os.path.isdir(os.path.join(PREFIX, f)):
            z.extract(f, PREFIX)
            if f.startswith('manifest' + os.sep):
                os.rename(os.path.join(PREFIX, f), os.path.join(PREFIX, 'manifest', filename.rsplit('.', 1)[0] + '.mft'))
    print 'installed', filename

if __name__ == '__main__':
    main()
