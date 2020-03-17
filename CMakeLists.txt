# Xiphos build script
#
# Copyright (C) 2018 Xiphos Development Team
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
#

# the oldest stable cmake version we support
cmake_minimum_required (VERSION 3.11 FATAL_ERROR)
cmake_policy(VERSION 3.11)


### set project name and version HERE ###
project (xiphos
  VERSION "4.1.0"
  LANGUAGES C CXX)


# set commonly used Xiphos variables:
# Xiphos website
set (XIPHOS_WEBSITE "http://xiphos.org")

# Xiphos issues report website
set (XIPHOS_BUG_REPORT "https://github.com/crosswire/xiphos/issues")

# Xiphos release date, (read from 'ChangeLog')
# read last entry in ChangeLog
file(STRINGS ${PROJECT_SOURCE_DIR}/ChangeLog
  ChangeLog_lastentry REGEX "^20[0-9][0-9]-[0-9][0-9]" LIMIT_COUNT 1)
# get year
string(SUBSTRING ${ChangeLog_lastentry} 0 4 ChangeLog_year)
# get month's number
string(SUBSTRING ${ChangeLog_lastentry} 5 2 ChangeLog_month)
# transform month's number into month's name
list(APPEND month_names "XXX;Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec")
list(GET month_names ${ChangeLog_month} ChangeLog_month)
# generate release date string
string(CONFIGURE "@ChangeLog_month@ @ChangeLog_year@" XIPHOS_RELEASE_DATE)


# make sure it's an out-of-stream build
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
  message(FATAL_ERROR "You cannot build in the source directory (or any directory with a CMakeLists.txt file). Please make an out-of-stream build subdirectory.")
endif()


# tell cmake where its modules can be found in our project directory
list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# set install directory variables as defined by the GNU Coding Standards.
# e.g. CMAKE_INSTALL_DATAROOTDIR; CMAKE_INSTALL_FULL_BINDIR;
# CMAKE_INSTALL_FULL_DOCDIR...
include (GNUInstallDirs)

# set CMAKE options that the user can optionally select ON or OFF.
include (XiphosOptions)

# find needed tools for building Xiphos
include (XiphosBuildTools)

# find dependencies
include (XiphosDependencies)

# set flags
include (XiphosFlags)

# create config.h
include (XiphosConfig_h)

# build xiphos
add_subdirectory(win32)
add_subdirectory(src/backend)
add_subdirectory(src/editor)
add_subdirectory(src/main)
add_subdirectory(src/webkit)
add_subdirectory(src/xiphos_html)
add_subdirectory(src/gtk)

# build xiphos-nav
add_subdirectory(src/examples)

# build other stuff
add_subdirectory(pixmaps)
add_subdirectory(ui)
add_subdirectory(po)
add_subdirectory(desktop)
add_subdirectory(doc)
add_subdirectory(help)

# create packages
add_subdirectory(cpack)

# report
include (XiphosReport)
# End
