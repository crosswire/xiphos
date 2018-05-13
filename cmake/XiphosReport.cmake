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

# Information Summary

message("")
message(STATUS "Summary")

# project info
message(STATUS "  Xiphos version               ${PROJECT_VERSION}")
message(STATUS "  Release date                 ${XIPHOS_RELEASE_DATE}")
message(STATUS "  Build type                   ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix               ${CMAKE_INSTALL_PREFIX}")

# Options
if (GTK2)
  message(STATUS "  Gtk version                  Gtk+-2.0")
else ()
  message(STATUS "  Gtk version                  Gtk+-3.0")
endif ()

if (WEBKIT1)
  message(STATUS "  WebKitGTK                    WebKit1")
else ()
  message(STATUS "  WebKitGTK                    WebKit2")
endif ()

if (GTKHTML)
  message(STATUS "  Editor                       Gtkhtml")
else ()
  message(STATUS "  Editor                       WebKit-editor")
endif ()
message(STATUS "  D-Bus                        ${DBUS}")

if (CMAKE_BUILD_TYPE MATCHES Debug)

  message("")
  message(STATUS "Debug summary")

  # System info
  message(STATUS "  System:                      ${CMAKE_SYSTEM_NAME}")
  message(STATUS "  CMake version:               ${CMAKE_VERSION}")
  message(STATUS "  System processor:            ${CMAKE_SYSTEM_PROCESSOR}")
  message(STATUS "  C Compiler:                  ${CMAKE_C_COMPILER}")
  message(STATUS "  CXX Compiler:                ${CMAKE_CXX_COMPILER}")
  file(TIMESTAMP ${PROJECT_SOURCE_DIR}/ChangeLog XIPHOS_REPORT_DATE "%Y-%m-%d %H:%M")
  message(STATUS "  Last Changelog               ${XIPHOS_REPORT_DATE}")
  # Options
  message(STATUS "  Chatty                       ${CHATTY}")
  message(STATUS "  Delint                       ${DELINT}")
  message(STATUS "  Debug                        ${DEBUG}")
  message(STATUS "  Strip                        ${STRIP}")

#  get_directory_property(COMPOPTIONS COMPILE_OPTIONS)
#  message(STATUS "  COMPILE_OPTIONS:             ${COMPOPTIONS}")
#  message(STATUS "  CMAKE_C_FLAGS:               ${CMAKE_C_FLAGS}")
#  message(STATUS "  CMAKE_CXX_FLAGS:             ${CMAKE_CXX_FLAGS}")
#  message(STATUS "  CMAKE_EXE_LINKER_FLAGS:      ${CMAKE_EXE_LINKER_FLAGS}")
endif()

if(WIN32)
  message("")
  message(STATUS "Windows© summary")
  message(STATUS "  Crosscompiling:              ${CMAKE_CROSSCOMPILING}")
  message(STATUS "  Toolchain:                   ${CMAKE_TOOLCHAIN_FILE}")
  message(STATUS "  root path:                   ${CMAKE_FIND_ROOT_PATH}")
  message(STATUS "  Console:                     ${CONSOLE}")
  message(STATUS "  Library prefixes:            ${CMAKE_FIND_LIBRARY_PREFIXES}")
  message(STATUS "  Library suffixes:            ${CMAKE_FIND_LIBRARY_SUFFIXES}")
endif()


# warnings

# Warn about unknown platform
if( NOT WIN32 AND NOT (CMAKE_SYSTEM_NAME STREQUAL Linux))
  message(AUTHOR_WARNING "Unknown plateform, assuming ${CMAKE_SYSTEM_NAME} is UNIX like platform")
endif()

# warn about Webkit2 editor not yet implemented
if (NOT WEBKIT1 AND NOT GTKHTML)
  message (AUTHOR_WARNING "Webkit2 and the WebKit2-editor were both selected ON, but integration of the Webkit2 editor is a work in progress. This can lead to fatal errors and build failures.")
endif ()
