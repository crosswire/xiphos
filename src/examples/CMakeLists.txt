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

# building xiphos-nav

if (HAVE_DBUS)
  message (STATUS "Configuring xiphos-nav target")

  # we first need to generate ipc-interface.h
  add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ipc-interface.h
    # process ipc-interface.xml to generate ipc-interface.h
    COMMAND ${DBUS_BINDING_TOOL}
    --mode=glib-client
    --output=${CMAKE_CURRENT_BINARY_DIR}/ipc-interface.h
    --prefix=ipc_object ${CMAKE_CURRENT_SOURCE_DIR}/../gtk/ipc-interface.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating src/example/ipc-interface.h"
    )

  add_executable (
    xiphos-nav
    xiphos-nav.c
    ${CMAKE_CURRENT_BINARY_DIR}/ipc-interface.h
    )


  # specify include directories to use when compiling
  target_include_directories (xiphos-nav
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR} # for ipc-interface.h
    Threads::Threads
    PkgConfig::Core
    PkgConfig::DBus
    )

  # specify libraries or flags to use when linking
  target_link_libraries(xiphos-nav
    PRIVATE
    Threads::Threads
    PkgConfig::Core
    PkgConfig::DBus
    )


  # install xiphos-nav
  install (TARGETS ${PROJECT_NAME}-nav
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT binaries)

endif (HAVE_DBUS)
