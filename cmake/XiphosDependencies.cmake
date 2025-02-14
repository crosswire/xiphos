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

# Only a few CMake built-in finders exists to find external packages.
# So we rely on pkg-config to find packages with the directive
# pkg_check_modules(<PREFIX> IMPORTED_TARGET <module>)
# The IMPORTED_TARGET argument will create an imported target named
# PkgConfig::<PREFIX>> that can be passed directly as an argument
# to target_include_directories (), target_link_libraries()..

# TODO: as pkg-config is typically found on Unix-like systems,
#       it should be avoided, and replaced by Find*.cmake commands
#       (although a bunch has to be written from scratch.)



# cross-compiling for Windows
if (CMAKE_CROSSCOMPILING AND WIN32)
  message(STATUS "Cross-Compiling for Windows")
  # Configure pkg-config for cross-compilation
  set(ENV{PKG_CONFIG_LIBDIR} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)
endif(CMAKE_CROSSCOMPILING AND WIN32)


# find the pkg-config executable in order to find modules
find_package(PkgConfig QUIET REQUIRED)


# Biblesync dependencies
if(WIN32)
  pkg_check_modules(Biblesync REQUIRED IMPORTED_TARGET
    "biblesync>=2.0.1"
    )
else()
  find_package(Biblesync REQUIRED IMPORTED_TARGET "biblesync>=1.2.0")
endif()

# Sword dependencies
pkg_check_modules(Sword REQUIRED IMPORTED_TARGET
  "sword>=1.8.1"
  )


# core dependencies
if(WIN32)
  # libssh2 depends on libws2_32.a, windows© needs libws2-32a for sockets
  # MinGW has libuuid.a inside lib out of the box
  pkg_check_modules(Core REQUIRED IMPORTED_TARGET
    "glib-2.0"
    "gmodule-2.0"
    "icu-i18n"
    "libssh2"
    )
else()
  pkg_check_modules(Core REQUIRED IMPORTED_TARGET
    "glib-2.0"
    "gmodule-2.0"
    "icu-i18n"
    "uuid"
    )
endif()

# Gnome dependencies
pkg_check_modules(Gnome REQUIRED IMPORTED_TARGET
  "atk"
  "cairo"
  "gdk-3.0"
  "gdk-pixbuf-2.0"
  "gio-2.0"
  "gobject-2.0"
  "libsoup-3.0"
  "pango"
  "minizip"
  "zlib"
  "libxml-2.0>=2.7.8"
  )

# Gtk dependencies
if (GTK2)
  if (GTKHTML)
    # Gtk+-2.0 + GtkHtml editor
    pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET
      "gtk+-2.0>=2.14"
      "libglade-2.0"
      "webkit-1.0"
      "gtkhtml-editor-3.14"
      "libgtkhtml-3.14>=3.23"
      )
  else (GTKHTML)
    # Gtk+-2.0 + Webkit-editor
    pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET
      "gtk+-2.0>=2.24"
      "libglade-2.0"
      "webkit-1.0"
      )
  endif (GTKHTML)
  # Gtk2+-2.0 optional module
  pkg_check_modules(Unix-print IMPORTED_TARGET
    "gtk+-unix-print-2.0"
    )
else (GTK2)
  if (WEBKIT1 AND GTKHTML)
    # Gtk+-3.0 + Webkit1 + WebKit-editor
    pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET
      "gtk+-3.0"
      "webkitgtk-3.0"
      "gtkhtml-editor-4.0"
      "libgtkhtml-4.0"
      )
  endif()
  if (WEBKIT1 AND NOT GTKHTML)
    # Gtk+-3.0 + Webkit1 + GtkHtml-editor
    pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET
      "gtk+-3.0"
      "webkit2gtk-4.1"
      )
  endif()
  if (NOT WEBKIT1 AND GTKHTML)
    # Gtk+-3.0 + Webkit2 + WebKit-editor
    pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET
      "gtk+-3.0"
      "webkit2gtk-4.1"
      "gtkhtml-editor-4.0"
      "libgtkhtml-4.0"
      )
  endif()
  if (NOT WEBKIT1 AND NOT GTKHTML)
    # Gtk+-3.0 + Webkit2 + GtkHtml-editor
     pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET
      "gtk+-3.0"
      "webkit2gtk-4.1"
      )
  endif()
endif (GTK2)


# D-Bus dependencies
IF (DBUS)
  pkg_check_modules(DBus REQUIRED IMPORTED_TARGET
    "dbus-glib-1"
    )
ENDIF()

# find threads
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# find gettext
find_package(Gettext 0.19.7)
if (NOT GETTEXT_FOUND)
  message (STATUS "Native Language Support disabled.")
endif ()


if(WIN32)
  # Ensure that the default include system directories are added to the list
  # of CMake implicit includes.
  # This workarounds an issue that happens when using GCC 6 and using system
  # includes (-isystem).
  # For more details check: https://bugs.webkit.org/show_bug.cgi?id=161697
  macro(DETERMINE_GCC_SYSTEM_INCLUDE_DIRS _lang _compiler _flags _result)
    file(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/dummy" "\n")
    separate_arguments(_buildFlags UNIX_COMMAND "${_flags}")
    execute_process(COMMAND ${_compiler} ${_buildFlags} -v -E -x ${_lang} -dD dummy
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles OUTPUT_QUIET
      ERROR_VARIABLE _gccOutput)
    file(REMOVE "${CMAKE_BINARY_DIR}/CMakeFiles/dummy")
    if ("${_gccOutput}" MATCHES "> search starts here[^\n]+\n *(.+) *\n *End of (search) list")
      set(${_result} ${CMAKE_MATCH_1})
      string(REPLACE "\n" " " ${_result} "${${_result}}")
      separate_arguments(${_result})
    endif ()
  endmacro()

  if (CMAKE_COMPILER_IS_GNUCC)
    DETERMINE_GCC_SYSTEM_INCLUDE_DIRS("c" "${CMAKE_C_COMPILER}" "${CMAKE_C_FLAGS}" SYSTEM_INCLUDE_DIRS)
    set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES} ${SYSTEM_INCLUDE_DIRS})
  endif ()

  if (CMAKE_COMPILER_IS_GNUCXX)
    DETERMINE_GCC_SYSTEM_INCLUDE_DIRS("c++" "${CMAKE_CXX_COMPILER}" "${CMAKE_CXX_FLAGS}" SYSTEM_INCLUDE_DIRS)
    set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES} ${SYSTEM_INCLUDE_DIRS})
  endif ()
endif(WIN32)
