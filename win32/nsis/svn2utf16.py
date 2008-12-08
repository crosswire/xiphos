#!/usr/bin/env python
"""
GnomeSword Bible Study Tool
svn2utf16.py

Copyright (C) 2008 GnomeSword Developer Team

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

# charsets
ch_utf8 = "utf8"
ch_utf16 = "utf16"

fprefix = "utf16_" # prefix for new text files in UTF-16LE
folders = ["i18n", "include"]
nsis_script = "installer.nsi"


# UTF-8 to UTF-16
def to_utf16(file):
    file_in = open(file, 'r')
    file_out = open(fprefix + file, 'w')
    
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
    for fold in folders:
        files = os.listdir(fold)
        os.chdir(fold)
        for f in files:
            # ignore hidden '.svn' folder
            if f == ".svn":
                continue
            print fold + "/" + f
            to_utf16(f)
        os.chdir("..")
    

# delete all UTF-16 files
def clean():
    print "deleting..."

    script = fprefix + nsis_script
    if os.path.exists(script):
        print script
        os.remove(script)

    for fold in folders:
        files = os.listdir(fold)
        os.chdir(fold)
        for f in files:
            # file name contains string 'utf16_'
            if f.find(fprefix) >= 0:
                print fold + "/" + f
                os.remove(f)
        os.chdir("..")
    


def main():
    clean()
    if len(sys.argv) <= 1:
        convert()
    

if __name__ == '__main__':
    main()

