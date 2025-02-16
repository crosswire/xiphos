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

# building xiphos_html

# marshalling
add_custom_command (
  OUTPUT
  ${CMAKE_CURRENT_BINARY_DIR}/marshal.h
  ${CMAKE_CURRENT_BINARY_DIR}/marshal.c
  # Marshalling
  COMMAND ${GLIB_GENMARSHAL}
  ${CMAKE_CURRENT_SOURCE_DIR}/marshal.list
  --prefix=xihtml_marshal
  --header > ${CMAKE_CURRENT_BINARY_DIR}//marshal.h

  COMMAND ${GLIB_GENMARSHAL}
  ${CMAKE_CURRENT_SOURCE_DIR}/marshal.list
  --prefix=xihtml_marshal
  --body > ${CMAKE_CURRENT_BINARY_DIR}/marshal.c

  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/marshal.list
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Marshalling xiphos_html files"
  )


# build
add_library (xiphos_html
  STATIC
  xiphos_html.c
  xiphos_html.h
  ${CMAKE_CURRENT_BINARY_DIR}/marshal.c
  )

# Add alias
add_library(Xiphos::Xiphos_html ALIAS xiphos_html)

# specify include directories to use when compiling
target_include_directories(xiphos_html
  PRIVATE
  Threads::Threads
  PkgConfig::Core
  PkgConfig::Gtk
  PkgConfig::Gnome
  PkgConfig::Soup
  )

# specify libraries or flags to use when linking
target_link_libraries(xiphos_html
  PRIVATE
  Threads::Threads
  PkgConfig::Core
  PkgConfig::Gtk
  PkgConfig::Gnome
  PkgConfig::Soup
  )

if(WK_FOUND)
  target_compile_definitions(xiphos_html
    PRIVATE
    PkgConfig::WK
    )
  target_link_libraries(xiphos_html
    PRIVATE
    PkgConfig::WK
    )
endif()
