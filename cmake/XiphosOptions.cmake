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
#
# manage CMAKE options

# set CMake options that the user can optionally select ON or OFF
option (GTK2     "Force UI build with Gtk2 instead of Gtk3."      OFF)
option (GTKHTML  "Force gtkhtml editor instead of webkit editor." OFF)
option (WEBKIT1  "Force webkit1 instead of webkit2. (Gtk3 only)"  OFF)
option (DBUS     "Use the Xiphos dbus API."                       ON)
option (CHATTY   "Enable lots of tracing."                        OFF)
option (HELP     "Creating help files."                           ON)
option (EPUB     "Creating ePub help files"                       ON)
option (HTMLHELP "Creating HTML Help Files"                       OFF)
option (MSHELP   "Creating Windows© Help file"                    OFF)
option (PDFHELP  "Creating PDF Help"                              OFF)
option (I18N     "Native Language Support."                       ON)
option (STRIP    "Strip resulting binary (Release only)."         OFF)
option (DEBUG    "Enable Xiphos g_str debugging functions."       OFF)
option (DELINT   "Build with: -Wall -Werror."                     OFF)
option (CONSOLE  "Enable console window in win32."                ON)
option (POSTINST "Enable post-install tasks."                     ON)
option (PACKAGE_RPM "Configure CPack to make RPM."                ON)
option (PACKAGE_DEB "Configure CPack to make DEB."                ON)
set (HELPDIR ${CMAKE_INSTALL_DATADIR}/help
  CACHE STRING "user documentation [default: ${CMAKE_INSTALL_DATADIR}/help]")

# Gtk2 implies WebKit1 only
if(GTK2)
  set(WEBKIT1 ON)
endif(GTK2)


# do not strip if not in Release mode
if (NOT (CMAKE_BUILD_TYPE MATCHES Release) AND STRIP)
  message(STATUS "NOT Mode=Release: Strip has been disabled")
  set (STRIP OFF)
endif ()


# set win32 options:
if (WIN32)
  # not sure about this: disable D-Bus
  set (DBUS OFF)
  # build MS-Help
  if (HELP)
    set (MSHELP ON)
  endif (HELP)
  # no console in Release
  if (CMAKE_BUILD_TYPE MATCHES Release)
    set (CONSOLE OFF)
  endif ()
endif (WIN32)
