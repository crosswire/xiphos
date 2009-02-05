#!/usr/bin/env python
"""
Xiphos Bible Study Tool
svn2utf16.py

Copyright (C) 2008 Xiphos Developer Team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Library General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
"""
 
# Unicode NSIS (Nullsoft Scriptable Install System) expext its input text files
# to be in UTF-16LE.
# But SVN (Subversion) handles UTF-16 files as binary files.

# This script converts all input files for NSIS from UTF-8 to UTF-16.

# Usage:
#                $ python svn2utf16.py
#
# delete files in UTF-16
#
#                $ python svn2utf16.py clean
 
import os
import sys
from os.path import join, dirname, split, abspath

# charsets
ch_utf8 = "utf8"
ch_utf16 = "utf16"

# absolute path to folder, where this script is
base = abspath(__file__)
base = dirname(base)


# join file name with base folder
def b_join(name):
    return abspath(join(base, name))


# folders, files - use absolute paths
fprefix = "utf16_" # prefix for new text files in UTF-16LE
folders = map(b_join, ["i18n", "include"])
nsis_script = b_join("installer.nsi")
release_notes = b_join("../../RELEASE-NOTES")
release_notes_utf16 = b_join("utf16_RELEASE-NOTES")


# add prefix 'utf16_' to file name
def add_fpref(name):
    (head, tail) = split(name)
    return join(head, fprefix + tail)


# UTF-8 to UTF-16
def to_utf16(file, fileout=None):
    file_in = open(file, 'r')

    if fileout == None:
        file_out = open(add_fpref(file), 'w')
    else:
        file_out = open(fileout, 'w')
    
    # conversion
    content = file_in.read()
    content = unicode(content,  ch_utf8)
    content = content.encode(ch_utf16)
    
    file_out.write(content)
    file_out.close()
    file_in.close()


# convert all files to UTF-16
def convert():

    print "converting..."
    print nsis_script
    to_utf16(nsis_script)
    
    print release_notes
    to_utf16(release_notes, release_notes_utf16)

    for fold in folders:
        print "dir:", fold
        files = os.listdir(fold)
        for f in files:
            # ignore hidden '.svn' folder
            if f == ".svn":
                continue
            f = join(fold, f)
            print f
            to_utf16(f)
    

# delete all UTF-16 files
def clean():

    print "deleting..."

    script = add_fpref(nsis_script)
    if os.path.exists(script):
        print script
        os.remove(script)

    if os.path.exists(release_notes_utf16):
        print release_notes_utf16
        os.remove(release_notes_utf16)

    for fold in folders:
        files = os.listdir(fold)
        for f in files:
            # file name contains string 'utf16_'
            if f.find(fprefix) >= 0:
                f = join(fold, f)
                print f
                os.remove(f)
    

def main():
    clean()
    if len(sys.argv) <= 1:
        convert()
    

if __name__ == '__main__':
    main()

