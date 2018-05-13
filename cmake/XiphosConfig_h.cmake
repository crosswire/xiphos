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

# Create config.h

message (STATUS "Generating config.h")

# pixmaps dir
set (PACKAGE_PIXMAPS_DIR "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")

# locale dir
set (PACKAGE_LOCALE_DIR "${CMAKE_INSTALL_FULL_LOCALEDIR}")

# share dir
set (SHARE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")

# textdomain
set (GETTEXT_PACKAGE "${PROJECT_NAME}")

# webkit editor
if (NOT GTKHTML)
  set (USE_WEBKIT_EDITOR ON)
endif ()

# Gtk
if (NOT GTK2)
  set (USE_GTK_3 ON)
  set (USE_GTKBUILDER ON)
endif ()

# select WebkitGtk
if (NOT WEBKIT1)
  set (USE_WEBKIT2 ON)
endif ()

# enable dBus
if (DBUS)
  set (HAVE_DBUS ON)
endif()


# i18n
include (CheckIncludeFiles)
check_include_files ("locale.h;libintl.h" ENABLE_NLS)

# strcasestr
include (CheckFunctionExists)
check_function_exists (strcasestr HAVE_STRCASESTR)


# generate config.h
set (CONFIG_H ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
  )
