#! /usr/bin/env python

import urllib2
import zipfile
import os

prefix = "/opt/mingw/test"
cache = prefix + "/cache"

def main():

    f = open("gtkList")

    for line in f:
        url = line.split(' ')[0]
        filename = line.split(' ')[1].strip()
        packagename = filename.rsplit('.', 1)[0]
        in_cache, package_file = is_upgrade(packagename)
        if (os.path.isfile(cache + "/" + filename)):
            print "found in cache; skipped downloading " + filename
        elif (in_cache):
            print "upgrading package " + packagename
            uninstall_package(package_file)
            download_package(url, filename)
            install_package(filename)
        else:
            download_package(url, filename)
            install_package(filename)

def uninstall_package(package_file):
    '''takes a cache/*.zip; it removes the zip file, then reads through the
    associated manifest file and removes every file listed. It leaves the
    directories alone'''

    os.remove(cache + "/" + package_file)
    filename = prefix + "/manifest/" + package_file.rsplit('.',1)[0] + ".mft"
    manifest = open(filename)
    for line in manifest:
        to_remove = line.strip()
        path = prefix + "/" + to_remove
        if (os.path.isfile(path)):
            os.remove(path)
    print "removed package " + package_file

def is_upgrade(package):
    ''' takes a package name like atk_1.26.0-1
    and looks in the cache directory to see if there is a
    match even if it has a different version number. If it
    does, return true, and the package name plus version.
    If it doesn't, return false and empty string'''

    package_name = package.split('_', 1)[0]
    files = os.listdir(cache)
    packages = [f.split('_') for f in files]
    for p in packages:
        if package_name == p[0]:
            return True, '_'.join(p)
    return False, ''

def download_package(url, filename):
    '''downloads the url and writes to the cache
    + filename'''
    print "downloading " + url
    output = open(cache + "/" + filename, 'wb')
    output.write(urllib2.urlopen(url).read())
    output.close()

def install_package(filename):
    '''unzips the package to the prefix; handles the manifest
    files specially to make them match the zip file name
    exactly. Some older package names are in a different format.
    Skips already created directories'''
    
    z = zipfile.ZipFile (cache + "/" + filename)
    for f in z.namelist():
        if not os.path.isdir(prefix + "/" + f):
            z.extract(f, prefix)
            if f.startswith('manifest/'):
                os.rename(prefix + "/" + f, prefix + "/manifest/" + filename.rsplit('.', 1)[0] + '.mft')
    print "installed " + filename

if __name__ == "__main__":
    main()
