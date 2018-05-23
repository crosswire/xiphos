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

# check for utilities needed for building Xiphos

# generic function
function (xiphos_find_program cmd_name cmd_exec)
  message (STATUS "Looking for ${cmd_exec}")
  find_program(${cmd_name} ${cmd_exec})
  if(${cmd_name})
    message (STATUS "Looking for ${cmd_exec} - found")
  else()
    message(FATAL_ERROR "Cannot find ${cmd_exec}, which is required to build ${PROJECT_NAME}")
  endif()
endfunction ()


# mandatory tools
xiphos_find_program (GLIB_GENMARSHAL glib-genmarshal)
xiphos_find_program (XMLLINT xmllint)
xiphos_find_program (XSLTPROC xsltproc)

# dbus tools
if (DBUS)
  xiphos_find_program (DBUS_BINDING_TOOL dbus-binding-tool)
endif (DBUS)

# i18n tools
if (I18N)
  # we need itstool for translating Help
  xiphos_find_program (ITSTOOL itstool)
endif (I18N)


# Linux tools
if (UNIX)
  # checking xiphos.desktop
  xiphos_find_program (DESKTOP_FILE_VALIDATE desktop-file-validate)
  # checking xiphos.appdata.xml
  xiphos_find_program (APPSTREAM_UTIL appstream-util)
endif (UNIX)


# Windows© tools
if (WIN32)
# CPackNSIS needs makensis
  xiphos_find_program (MAKENSIS makensis)
endif (WIN32)


# help tools
if (EPUB OR MSHELP OR PDFHELP)
  # we need yelp-build for converting Help to Html or ePub
  xiphos_find_program (YELP_BUILD yelp-build)
endif (EPUB OR MSHELP OR PDFHELP)

# Windows© help compiler: chmcmd/Linux hhc.exe/Windows©
if (MSHELP)
  if (WIN32)
    if (CMAKE_CROSSCOMPILING)
      xiphos_find_program (CHMCMD chmcmd)
    else (CMAKE_CROSSCOMPILING)
      xiphos_find_program (HHC_EXE hhc.exe)
    endif (CMAKE_CROSSCOMPILING)
  else (WIN32)
    xiphos_find_program (CHMCMD chmcmd)
  endif (WIN32)
endif (MSHELP)

# ePub help: yelp-build needs uuidgen
if (EPUB)
  xiphos_find_program (UUIDGEN uuidgen)
endif (EPUB)

# Html to Pdf Help converter
if (PDFHELP)
  xiphos_find_program (EBOOK_CONVERT ebook-convert)
endif (PDFHELP)
