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

# install win32 required files

# define common dirs
if (WIN32)
  # set icon file
  set(XIPHOS_MAIN_ICON "${CMAKE_SOURCE_DIR}/pixmaps/gs2-48x48.ico")
  # generate resource file
    configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/res/xiphos-winres.rc.in
    ${CMAKE_BINARY_DIR}/src/gtk/xiphos-winres.rc
  )
  # don't install in /usr
  if (CMAKE_INSTALL_PREFIX MATCHES "^/usr")
    set (CMAKE_INSTALL_PREFIX "install")
  endif (CMAKE_INSTALL_PREFIX MATCHES "^/usr")

  if (CMAKE_CROSSCOMPILING)
    # define MinGW dirs
    set (SYS_ROOT_BIN ${CMAKE_FIND_ROOT_PATH}/bin)
    set (SYS_ROOT_LIB ${CMAKE_FIND_ROOT_PATH}/lib)
    set (SYS_ROOT_SHARE ${CMAKE_FIND_ROOT_PATH}/share)
    set (SYS_ROOT_ETC ${CMAKE_FIND_ROOT_PATH}/etc)
  else (CMAKE_CROSSCOMPILING)
    # plain Windows©
    message (FATAL_ERROR "Windows© build is not yet implemented, please define SYS_ROOT_BIN, SYS_ROOT_LIB... variables")
  endif (CMAKE_CROSSCOMPILING)

  # add required files for cross-compiling
  # X11/keysymdef.h
  file (COPY ${CMAKE_SOURCE_DIR}/win32/include/X11
    DESTINATION ${CMAKE_BINARY_DIR}/src/main)
  file (COPY ${CMAKE_SOURCE_DIR}/win32/include/X11
    DESTINATION ${CMAKE_BINARY_DIR}/src/gtk)


  # install biblesync.dll
  if (EXISTS ../biblesync/libbiblesync.dll)
    install(FILES
      ../biblesync/libbiblesync.dll
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT binaries
      )
  elseif (EXISTS ${CMAKE_FIND_ROOT_PATH}/bin/libbiblesync.dll)
    install(FILES
      ${SYS_ROOT_BIN}/libbiblesync.dll
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT binaries
      )
  endif (EXISTS ../biblesync/libbiblesync.dll)


  # install architecture dependent binaries
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    # install x86_64 binaries
    install(FILES
      ${SYS_ROOT_BIN}/gspawn-win64-helper.exe
      ${SYS_ROOT_BIN}/gspawn-win64-helper-console.exe
      ${SYS_ROOT_BIN}/libgcc_s_seh-1.dll
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT binaries
      )
  else()
    # install i686 binaries
    install(FILES
      ${SYS_ROOT_BIN}/gspawn-win32-helper.exe
      ${SYS_ROOT_BIN}/gspawn-win32-helper-console.exe
      ${SYS_ROOT_BIN}/libgcc_s_sjlj-1.dll
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT binaries
      )
  endif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")


  # install gtk stuff
  if (GTK2)
    # gtk+-2.0 binaries
    install(FILES
      ${SYS_ROOT_BIN}/libglade-2.0-0.dll
      ${SYS_ROOT_BIN}/libgailutil-18.dll
      ${SYS_ROOT_BIN}/libgtk-win32-2.0-0.dll
      ${SYS_ROOT_BIN}/libjavascriptcoregtk-1.0-0.dll
      ${SYS_ROOT_BIN}/libwebkitgtk-1.0-0.dll
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT binaries
      )
    # gtk+-2.0 libraries
    install(DIRECTORY
      ${SYS_ROOT_LIB}/gtk-2.0
      DESTINATION ${CMAKE_INSTALL_LIBDIR}
      COMPONENT binaries
      )
    # gtk+-2.0 shared files
    install(DIRECTORY
      ${SYS_ROOT_SHARE}/webkitgtk-1.0
      DESTINATION ${CMAKE_INSTALL_DATADIR}
      COMPONENT data
      )
  else (GTK2)
    # gtk+-3.0 binaries
    install(FILES
      ${SYS_ROOT_BIN}/libgailutil-3-0.dll
      ${SYS_ROOT_BIN}/libgtk-3-0.dll
      ${SYS_ROOT_BIN}/libjavascriptcoregtk-3.0-0.dll
      ${SYS_ROOT_BIN}/libwebkitgtk-3.0-0.dll
      DESTINATION ${CMAKE_INSTALL_BINDIR}
      COMPONENT binaries
      )
    # gtk+-3.0 shared files
    install(DIRECTORY
      ${SYS_ROOT_SHARE}/webkitgtk-3.0
      DESTINATION ${CMAKE_INSTALL_DATADIR}
      COMPONENT data
      )
  endif (GTK2)

  # install required deps binaries
  # bin: Evolving versions
  install(DIRECTORY ${SYS_ROOT_BIN}
    DESTINATION ${CMAKE_INSTALL_BINDIR}/..
    COMPONENT binaries
    FILES_MATCHING
    PATTERN "libcrypto-*.dll"
    PATTERN "libssl-*.dll"
    PATTERN "libidn*.dll"
    )
  # bin: Stable versions
  install(FILES
    ${SYS_ROOT_BIN}/libsword.dll
    ${SYS_ROOT_BIN}/libminizip-1.dll
    ${SYS_ROOT_BIN}/libdbus-1-3.dll
    ${SYS_ROOT_BIN}/libdbus-glib-1-2.dll
    ${SYS_ROOT_BIN}/addld.exe
    ${SYS_ROOT_BIN}/emptyvss.exe
    ${SYS_ROOT_BIN}/imp2gbs.exe
    ${SYS_ROOT_BIN}/imp2ld.exe
    ${SYS_ROOT_BIN}/imp2vs.exe
    ${SYS_ROOT_BIN}/mkfastmod.exe
    ${SYS_ROOT_BIN}/mod2imp.exe
    ${SYS_ROOT_BIN}/mod2osis.exe
    ${SYS_ROOT_BIN}/mod2vpl.exe
    ${SYS_ROOT_BIN}/mod2zmod.exe
    ${SYS_ROOT_BIN}/osis2mod.exe
    ${SYS_ROOT_BIN}/tei2mod.exe
    ${SYS_ROOT_BIN}/vpl2mod.exe
    ${SYS_ROOT_BIN}/xml2gbs.exe
    ${SYS_ROOT_BIN}/installmgr.exe
    ${SYS_ROOT_BIN}/diatheke.exe
    ${SYS_ROOT_BIN}/vs2osisreftxt.exe
    ${SYS_ROOT_BIN}/uconv.exe
    ${SYS_ROOT_BIN}/icui18n57.dll
    ${SYS_ROOT_BIN}/icuuc57.dll
    ${SYS_ROOT_BIN}/icudata57.dll
    ${SYS_ROOT_BIN}/icule57.dll
    ${SYS_ROOT_BIN}/libcairo-gobject-2.dll
    ${SYS_ROOT_BIN}/iconv.dll
    ${SYS_ROOT_BIN}/libcairo-2.dll
    ${SYS_ROOT_BIN}/libfontconfig-1.dll
    ${SYS_ROOT_BIN}/libexpat-1.dll
    ${SYS_ROOT_BIN}/libfreetype-6.dll
    ${SYS_ROOT_BIN}/zlib1.dll
    ${SYS_ROOT_BIN}/libpixman-1-0.dll
    ${SYS_ROOT_BIN}/libpng16-16.dll
    ${SYS_ROOT_BIN}/libgdk_pixbuf-2.0-0.dll
    ${SYS_ROOT_BIN}/libgdk-3-0.dll
    ${SYS_ROOT_BIN}/libgdk-win32-2.0-0.dll
    ${SYS_ROOT_BIN}/libgio-2.0-0.dll
    ${SYS_ROOT_BIN}/libglib-2.0-0.dll
    ${SYS_ROOT_BIN}/libgmodule-2.0-0.dll
    ${SYS_ROOT_BIN}/libgobject-2.0-0.dll
    ${SYS_ROOT_BIN}/libffi-6.dll
    ${SYS_ROOT_BIN}/libjasper-4.dll
    ${SYS_ROOT_BIN}/libjpeg-62.dll
    ${SYS_ROOT_BIN}/libtiff-5.dll
    ${SYS_ROOT_BIN}/libpango-1.0-0.dll
    ${SYS_ROOT_BIN}/libpangocairo-1.0-0.dll
    ${SYS_ROOT_BIN}/libpangoft2-1.0-0.dll
    ${SYS_ROOT_BIN}/libpangowin32-1.0-0.dll
    ${SYS_ROOT_BIN}/libatk-1.0-0.dll
    ${SYS_ROOT_BIN}/libxml2-2.dll
    ${SYS_ROOT_BIN}/libbz2-1.dll
    ${SYS_ROOT_BIN}/libgthread-2.0-0.dll
    ${SYS_ROOT_BIN}/libgnurx-0.dll
    ${SYS_ROOT_BIN}/libenchant.dll
    ${SYS_ROOT_BIN}/libcurl-4.dll
    ${SYS_ROOT_BIN}/libssh2-1.dll
    ${SYS_ROOT_BIN}/libclucene-core.dll
    ${SYS_ROOT_BIN}/libclucene-shared.dll
    ${SYS_ROOT_BIN}/libwinpthread-1.dll
    ${SYS_ROOT_BIN}/libsoup-2.4-1.dll
    ${SYS_ROOT_BIN}/libsqlite3-0.dll
    ${SYS_ROOT_BIN}/libxslt-1.dll
    ${SYS_ROOT_BIN}/libintl-8.dll
    ${SYS_ROOT_BIN}/libstdc++-6.dll
    ${SYS_ROOT_BIN}/gdb.exe
    ${SYS_ROOT_BIN}/libwebp-7.dll
    ${SYS_ROOT_BIN}/libgstapp-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstbase-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstreamer-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstpbutils-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstvideo-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstaudio-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstbase-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstcontroller-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstfft-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstnet-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstriff-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstrtp-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstrtsp-1.0-0.dll
    ${SYS_ROOT_BIN}/libgstsdp-1.0-0.dll
    ${SYS_ROOT_BIN}/libgsttag-1.0-0.dll
    ${SYS_ROOT_BIN}/libharfbuzz-0.dll
    ${SYS_ROOT_BIN}/libharfbuzz-icu-0.dll
    ${SYS_ROOT_BIN}/libpcre-1.dll
    ${SYS_ROOT_BIN}/liborc-0.4-0.dll
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT binaries
    )


  # install required libraries
  install(DIRECTORY
    ${SYS_ROOT_LIB}/enchant
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT binaries
    )


  # install required shared files
  install(DIRECTORY
    ${SYS_ROOT_SHARE}/sword
    ${SYS_ROOT_SHARE}/enchant
    #  ${SYS_ROOT_SHARE}/gtkhtml-3.14
    DESTINATION ${CMAKE_INSTALL_DATADIR}
    COMPONENT data
    )


  # install required system conf files
  install(DIRECTORY
    ${SYS_ROOT_ETC}/fonts
    ${SYS_ROOT_ETC}/pango
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}
    COMPONENT data
    )


  # install win32 specific files
  install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/sword.conf
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/utils.bat
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT binaries
    )
  install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/etc
    ${CMAKE_CURRENT_SOURCE_DIR}/festival
    ${CMAKE_CURRENT_SOURCE_DIR}/share
    DESTINATION ${CMAKE_INSTALL_DATADIR}/..
    COMPONENT data
    )

  # install svg icon
  install(FILES
    ${PROJECT_SOURCE_DIR}/pixmaps/xiphos.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    COMPONENT data
    )


  # update farsi gtk+ .mo file
  execute_process(
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} ${PROJECT_SOURCE_DIR}/win32/po/fa.po -o ${CMAKE_CURRENT_BINARY_DIR}/gtk20.mo
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    TIMEOUT 5
    )

  # install farsi .mo file
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gtk20.mo
    DESTINATION ${CMAKE_INSTALL_DATADIR}/locale/fa/LC_MESSAGES/
    COMPONENT data
    )
endif(WIN32)
