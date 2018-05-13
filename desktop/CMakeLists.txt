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

# configure desktop related files


if (UNIX)

  add_custom_target(desktop)

  # xiphos.desktop file
  # describes how xiphos program is to be launched and how it appears in menus
  message(STATUS "Configuring ${PROJECT_NAME}.desktop file")
  # copy xiphos.desktop.in
  configure_file (${PROJECT_NAME}.desktop.in ${PROJECT_NAME}.desktop.in)
  # add translations
  if (GETTEXT_FOUND)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.desktop
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --desktop --template=${PROJECT_NAME}.desktop.in -d ${PROJECT_SOURCE_DIR}/po -o ${PROJECT_NAME}.desktop
      COMMAND ${DESKTOP_FILE_VALIDATE} ${PROJECT_NAME}.desktop
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Configuring ${PROJECT_NAME}.desktop file"
      )

    # install xiphos.desktop file
    install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.desktop
      DESTINATION  ${CMAKE_INSTALL_DATADIR}/applications
      COMPONENT data
      )

    add_custom_target(desktop-entry
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.desktop
      )
    add_dependencies(desktop desktop-entry)
  endif (GETTEXT_FOUND)

   # Icon
   # for use in the programs menu
   # TODO: on uninstall run gtk-update-icon-cache
   # install icon
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/xiphos.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    COMPONENT data
    )

  # run gtk-update-icon-cache to create a cache file for icon theme
  IF (POSTINST)
    # check that gtk-update-icon-cache is installed
    find_program (GTK_UPDATE_ICON_CACHE gtk-update-icon-cache)
    if (GTK_UPDATE_ICON_CACHE)
      # gtk-update-icon-cache was found
      message (STATUS "Installing gtk-update-icon-cache.")
      # run gtk-update-icon as a post-building command
      set(_update_icon_cache_cmd
	"${GTK_UPDATE_ICON_CACHE} -f -t ${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor")
      install(CODE
	"execute_process(
       COMMAND ${CMAKE_COMMAND} -E chdir . ${_update_icon_cache_cmd}
       )"
	)
    else (GTK_UPDATE_ICON_CACHE)
      # gtk-update-icon-cache was not found
      message(WARNING "gtk-update-icon-cache not found.
  Make sure to call ${GTK_UPDATE_ICON_CACHE} -f -t \"${CMAKE_INSTALL_DATADIR}/icons/hicolor\" after install and uninstall")
    endif (GTK_UPDATE_ICON_CACHE)
  endif (POSTINST)



  # xiphos.appdata.xml
  # allows the user to look at screenshots and a long description of the application
  # before it is installed on a Linux system.
  message (STATUS "Configuring the AppData metadata file")
  # add translations and validate xiphos.appdata.xml
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.appdata.xml
    COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --xml --template "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.appdata.xml" -d "${PROJECT_SOURCE_DIR}/po" -o "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.appdata.xml"
    COMMAND ${APPSTREAM_UTIL} validate-relax --nonet ${PROJECT_NAME}.appdata.xml > validate.timestamp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Configuring the AppData metadata file"
    )
  # install file
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.appdata.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/appdata
    COMPONENT data
    )

  add_custom_target(appdata
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.appdata.xml
    )
  add_dependencies(desktop appdata)


  # unix man pages
  # TODO: make man pages translatable
  message (STATUS "Building man pages")
  # configure xiphos man page
  configure_file (${PROJECT_NAME}.1.in ${PROJECT_NAME}.1)
  # install xiphos man pages
  install (
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
    COMPONENT data
    )

  if (HAVE_DBUS)
    # configure xiphos-nav man page
    configure_file (${PROJECT_NAME}-nav.1.in ${PROJECT_NAME}-nav.1)
    # install xiphos-nav man pages
    install (
      FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-nav.1
      DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
      COMPONENT data
      )
  endif (HAVE_DBUS)

  add_dependencies(xiphos desktop)

endif (UNIX)
