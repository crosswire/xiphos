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

# building MO files
# We copy all PO files in the build dir in order to not alter the source tree
# as 'gettext_process_pot_file' rebuilds each PO file from the POT file.
# The only case a PO file is created is when a language is set in LINGUAS
# and the PO file is missing.

if (GETTEXT_FOUND)
  # get a list of active translations from LINGUAS
  file (READ ${CMAKE_CURRENT_SOURCE_DIR}/LINGUAS linguas)
  # transform linguas string to a cmake regular list
  string (REGEX MATCHALL "[a-zA-Z_]+" langs "${linguas}")

  # copy PO files into build dir
  foreach (lang ${langs})
    set(pofile "${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po")
    if(NOT EXISTS ${pofile})
      # missing PO, add a new PO file
      message(STATUS "Creating po/${lang}.po from LINGUAS.")
      # copy xiphos.pot into build dir
      file(
	COPY "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pot"
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
      # rename pot copy to ${lang}.po
      file(
	RENAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pot"
	"${CMAKE_CURRENT_BINARY_DIR}/${lang}.po")
      # copy new ${lang}.po into source dir
      file(
	COPY "${CMAKE_CURRENT_BINARY_DIR}/${lang}.po"
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})
    else ()
      # copy existing PO file into build dir
      file (
      COPY "${CMAKE_CURRENT_SOURCE_DIR}/${lang}.po"
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif ()
  endforeach(lang)

  # create and install MO files
  gettext_process_pot_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pot
    ALL
    INSTALL_DESTINATION ${CMAKE_INSTALL_LOCALEDIR}
    LANGUAGES ${langs}
    )

  add_custom_target(process-pot-file)

  add_dependencies(xiphos process-pot-file )


  # Custom targets for updating translation files
  # NOTE: these targets will update files in the source tree

  # target to refresh POT file
  find_program(INTLTOOL_UPDATE intltool-update)
  add_custom_target (pot
    COMMAND ${INTLTOOL_UPDATE} --pot --gettext-package=${PROJECT_NAME}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/po
    COMMENT "Updating pot file."
    )

  # target for updating a given PO file
  foreach (language ${langs})
    add_custom_target (${language}.po
      COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --update --verbose ${language}.po ${PROJECT_NAME}.pot
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/po
      COMMENT "Updating ${language}.po file"
      )
    # prepare command for the all PO target
    set (PO_UPDATE_ALL ${PO_UPDATE_ALL} && echo ${language}.po: && ${GETTEXT_MSGMERGE_EXECUTABLE} --update --verbose ${language}.po ${PROJECT_NAME}.pot)
  endforeach()

  # target for updating all PO files
  set (PO_UPDATE_ALL pwd ${PO_UPDATE_ALL})
  add_custom_target (po
    COMMAND ${PO_UPDATE_ALL}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/po
    COMMENT "Updating all PO files"
    VERBATIM
    )
endif ()
