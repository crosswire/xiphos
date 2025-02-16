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

# build backend

add_library (backend STATIC
  gs_stringmgr.cpp
  module_manager.cc
  sword_main.cc
  )
# Add alias
add_library(Xiphos::Backend ALIAS backend)


# specify include directories to use when compiling
target_include_directories(backend
  PRIVATE
  Threads::Threads
  PkgConfig::Core
  PkgConfig::Gtk
  PkgConfig::Gnome
  PkgConfig::Soup
  PkgConfig::Sword
  )

# specify libraries or flags to use when linking
target_link_libraries(backend
  PRIVATE
  Threads::Threads
  PkgConfig::Core
  PkgConfig::Gtk
  PkgConfig::Gnome
  PkgConfig::Soup
  PkgConfig::Sword
  )

if(WK_FOUND)
  target_compile_definitions(backend
    PRIVATE
    PkgConfig::WK
    )
  target_link_libraries(backend
    PRIVATE
    PkgConfig::WK
    )
endif()
