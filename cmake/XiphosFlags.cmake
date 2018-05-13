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

# set flags

message (STATUS "Setting up flags")

# include config.h
add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_BINARY_DIR})

# include source dir
include_directories(${PROJECT_SOURCE_DIR}/src)


# set a default build type if none was specified
set(default_build_type "Release")
if(EXISTS "${PROJECT_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message (STATUS "Setting build type to '${default_build_type}' as none was specified.")
  message(STATUS " To specify another build type, run cmake -DCMAKE_BUILD_TYPE=<value>")
  message(STATUS " Possible values are 'Debug', 'Release', 'RelWithDebInfo' and 'MinSizeRel'.")
  set(CMAKE_BUILD_TYPE "${default_build_type}")
endif()

# set debug and release options
set(debug_flags -DDEBUG)
set(release_flags -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2)
add_compile_options(
  "$<$<CONFIG:Debug>:${debug_flags}>"
  "$<$<CONFIG:Release>:${release_flags}>"
  "$<$<CONFIG:RelWithDebInfo>:${debug_flags}>"
  "$<$<CONFIG:MinSizeRel>:${release_flags}>"
  )

# set 'strip' option
if (STRIP)
  link_libraries( -s)
endif()


# stop the insanity. NULLs mean something. shocking, i know, but seriously...
add_compile_options(-fno-delete-null-pointer-checks)


# incompatible Windows© options
if (NOT WIN32)
  # tells the linker to add all symbols, not only used ones
  link_libraries( -rdynamic)
  # linker option to stop very recent dbus linkage nightmare.
  link_libraries( -Wl,-z,muldefs)
endif(NOT WIN32)


# set 'delint' options
if (DELINT)
set(c_flags -Werror -Wall -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wno-sign-compare)
set(cxx_flags -Werror -Wall)
add_compile_options(
  "$<$<COMPILE_LANGUAGE:C>:${c_flags}>"
  "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>"
)
endif (DELINT)


# set windows specific options
IF (WIN32)
  # it seems we don't need this anymore
#  add_compile_options( -I/usr/i686-w64-mingw32/sys-root/mingw/include -g)
  if (NOT CONSOLE)
    # disable console window in win32
    link_libraries( -mwindows)
  endif()
endif()
