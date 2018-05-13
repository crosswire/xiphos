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
cmake_minimum_required (VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)


### set project name and version HERE ###
project (xiphos
  VERSION "4.1.0"
  LANGUAGES C CXX)

# set Xiphos website
set (XIPHOS_WEBSITE "http://xiphos.org")
# set Xiphos issues report website
set (XIPHOS_BUG_REPORT "https://github.com/crosswire/xiphos/issues")
# set Xiphos release date, (we use 'ChangeLog' time value)
file(TIMESTAMP ${PROJECT_SOURCE_DIR}/ChangeLog
  XIPHOS_RELEASE_DATE "%b %Y" UTC)


# tell cmake where its modules can be found in our project directory
list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list (APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# make sure it's an out-of-stream build
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
  message(FATAL_ERROR "You cannot build in the source directory (or any directory with a CMakeLists.txt file). Please make an out-of-stream build subdirectory.")
endif()

# set install directory variables as defined by the GNU Coding Standards.
# e.g. CMAKE_INSTALL_DATAROOTDIR; CMAKE_INSTALL_FULL_BINDIR;
# CMAKE_INSTALL_FULL_DOCDIR...
include (GNUInstallDirs)

# cross-compiling for Windows
if (CMAKE_CROSSCOMPILING AND WIN32)
  message(STATUS "Cross-Compiling for Windows")
  # Configure pkg-config for cross-compilation
  set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)
endif(CMAKE_CROSSCOMPILING AND WIN32)


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
add_subdirectory(src/examples)
add_subdirectory(pixmaps)
add_subdirectory(ui)
add_subdirectory(po)
add_subdirectory(desktop)
add_subdirectory(mhelp)
add_subdirectory(doc)

# create packages
add_subdirectory(cpack)

# report
include (XiphosReport)
# End
