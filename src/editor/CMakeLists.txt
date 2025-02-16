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

# build editor
add_library (editor STATIC link_dialog.c)

# Add alias
add_library(Xiphos::Editor ALIAS editor)

if (USE_WEBKIT_EDITOR)
  target_sources(editor
    PRIVATE
    editor.c
    webkit_editor.c
    )
else ()
  target_sources(editor
    PRIVATE
    slib-editor.c
    link_dialog.c
    )
endif ()


# specify include directories to use when compiling
target_include_directories(editor
  PRIVATE
  Threads::Threads
  PkgConfig::Core
  PkgConfig::Gtk
  PkgConfig::Gnome
  PkgConfig::Soup
  PkgConfig::Sword
  PkgConfig::WK
  )

# specify libraries or flags to use when linking
target_link_libraries(editor
  PRIVATE
  Threads::Threads
  PkgConfig::Core
  PkgConfig::Gtk
  PkgConfig::Gnome
  PkgConfig::Soup
  PkgConfig::Sword
  PkgConfig::WK
  )

if (WIN32)
  # add X11/keysymdefs.h
  target_include_directories(editor
    PRIVATE
    ${PROJECT_SOURCE_DIR}/win32/include
    )
endif ()
