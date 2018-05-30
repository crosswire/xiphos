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

# help files
#
# TODO:
# - switch from itstool to portable gettext utilities (msgfmt and xgettext)


if (HELP)
  message (STATUS "Configuring Help files")

  # list of pages
  list (APPEND help_pages
    index.page
    legal.xml
    xiphos-01-introduction.page
    xiphos-02-shortcut-keys.page
    xiphos-10-start.page
    xiphos-11-interface.page
    xiphos-20-modules-introduction.page
    xiphos-21-modules-configuration.page
    xiphos-22-modules-install.page
    xiphos-23-modules-maintenance.page
    xiphos-24-modules-third-party.page
    xiphos-30-search.page
    xiphos-31-advanced-search.page
    xiphos-32-search-syntax.page
    xiphos-33-original-language.page
    xiphos-40-personal-commentary.page
    xiphos-41-studypad.page
    xiphos-42-journals.page
    xiphos-50-preferences-general-settings.page
    xiphos-51-preferences-biblesync.page
    xiphos-52-preferences-fonts-colors.page
    xiphos-53-preferences-modules.page
    xiphos-60-online-help.page
    xiphos-90-credits.page
    )

  # list of figures
  list (APPEND help_figures
    figures/first_start.png
    figures/gtkhtml3-vs-mozembed.png
    figures/interface_biblepane-options.png
    figures/interface_biblepane.png
    figures/interface_commentarypane.png
    figures/interface_dictionary.png
    figures/interface_menubar.png
    figures/interface_menubar-view.png
    figures/interface_parallel.png
    figures/interface_parallel-separate.png
    figures/interface.png
    figures/interface_searchpane.png
    figures/interface_shortcut.png
    figures/interface_sidepane.png
    figures/interface_toolbar.png
    figures/interface_viewer.png
    figures/journal.png
    figures/module.png
    figures/personal.png
    figures/preferences_fonts-color.png
    figures/preferences_fonts-misc.png
    figures/preferences_general-misc.png
    figures/preferences_general-biblesync.png
    figures/preferences_modules-misc.png
    figures/preferences_modules-parallel.png
    figures/preferences.png
    figures/search_search.png
    figures/studypad.png
    figures/sword3.png
    figures/sword_config.png
    figures/sword_install.png
    figures/sword_remove.png
    figures/sword_sources.png
    figures/xiphos-icon.png
    figures/xiphos-trail-icon.png
    )

  add_custom_target(handbook) # help is a reserved keyword

  # read languages from LINGUAS
  file (READ ${CMAKE_CURRENT_SOURCE_DIR}/HELP_LINGUAS linguas)
  # transform languages string into a CMake regular list
  string (REGEX MATCHALL "[a-zA-Z_]+" help_locales "${linguas}")
  # build a list of all languages by adding 'C' to the list
  list (APPEND help_all_langs "C" ${help_locales})

  # build all Mallard files

  # 1. copy C files into build_dir
  file (COPY "C" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  # add timestamp
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/C/pages.timestamp "EMPTY")
  # add target
  add_custom_target(help-C-pages
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/C/pages.timestamp
    )
  add_dependencies(handbook help-C-pages)

  # 2. create translated pages for all other languages than C
  foreach (lang ${help_locales})

    # create a .PO file if none exists
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${lang}/${lang}.po")
      message(STATUS "Creating ${lang}/${lang}.po from HELP_LINGUAS.")
      file(
	MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${lang}/")
      file(
	COPY "${CMAKE_CURRENT_SOURCE_DIR}/xiphos.pot"
	DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/${lang}/")
      file(
	RENAME "${CMAKE_CURRENT_SOURCE_DIR}/${lang}/xiphos.pot"
	"${CMAKE_CURRENT_SOURCE_DIR}/${lang}/${lang}.po")
    endif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${lang}/${lang}.po")

    # copy <lang> files into build_dir
    file (COPY ${lang} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    # create translated pages for <lang>
    set(pofile "${CMAKE_CURRENT_BINARY_DIR}/${lang}/${lang}.po")
    set(mofile "${CMAKE_CURRENT_BINARY_DIR}/${lang}/${lang}.mo")
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/pages.timestamp
      # translate pages
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o "${mofile}" "${pofile}"
      COMMAND ${CMAKE_COMMAND} -E env d="${CMAKE_CURRENT_SOURCE_DIR}/C"
      ${ITSTOOL} -m "${mofile}" ${help_pages} --output ${CMAKE_CURRENT_BINARY_DIR}/${lang}/
      # add timestamp
      COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${lang}/pages.timestamp
      DEPENDS ${pofile}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/C
      COMMENT "Generating '${lang}' help files"
      )

    # copy figures or import missing figures from C into build_dir
    foreach (media_file ${help_figures})
      if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${lang}/${media_file}")
	# the <lang> figure has not been created, install the corresponding C figure
	file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/C/${media_file}
	  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${lang}/figures
	  )
      endif (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${lang}/${media_file}")
    endforeach (media_file)

    add_custom_target(help-${lang}-pages
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}/pages.timestamp
      )

    add_dependencies(handbook help-${lang}-pages)

  endforeach (lang)


  # validate mallard pages
  set (mallard-schema ${CMAKE_CURRENT_SOURCE_DIR}/xml/mallard-1.0.rng)
  foreach (lang ${help_all_langs})
    add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/validate.txt
#      COMMAND ${XMLLINT} --noout --xinclude --noent --relaxng ${mallard-schema} *.page > ${CMAKE_CURRENT_BINARY_DIR}/${lang}/validate.txt 2>&1
      COMMAND ${XMLLINT} --noout --xinclude --noent --relaxng ${mallard-schema} *.page
      DEPENDS help-${lang}-pages
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang}
      COMMENT "'${lang}' help - mallard pages validation report"
      )
    add_custom_target(validate-${lang}-pages
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}/validate.txt
      )

    add_dependencies(handbook validate-${lang}-pages)

  endforeach (lang)


  # install help files
  if (UNIX)
    # install mallard help files
    foreach (lang ${help_all_langs})
      # list help pages to install
      set (install_pages)
      foreach(page ${help_pages})
	list(APPEND install_pages ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${page})
      endforeach(page)
      # install help files
      install (FILES ${install_pages}
	DESTINATION ${HELPDIR}/${lang}/${PROJECT_NAME}
	)
      # list figures to install
      set (install_media)
      foreach(media ${help_figures})
	list(APPEND install_media ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${media})
      endforeach(media)
      # install figures
      install (FILES ${install_media}
	DESTINATION ${HELPDIR}/${lang}/${PROJECT_NAME}/figures
	)
    endforeach (lang)
  endif (UNIX)


  # custom targets for updating translation files
  # NOTE: these targets will update files in the source tree

  # target to refresh .pot file
  set (help_pot ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pot)
  # list help pages to translate
  set (pot_pages)
  foreach(page ${help_pages})
    list(APPEND pot_pages ${CMAKE_CURRENT_BINARY_DIR}/C/${page})
  endforeach(page)
  add_custom_target (help_pot
    COMMAND ${ITSTOOL} -o ${help_pot} ${pot_pages}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Updating help pot file."
    )

  # target for updating a given .po file
  set (help_po_update_all echo)
  foreach (lang ${help_locales})
    add_custom_target (help_${lang}.po
      COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --update --verbose ${lang}.po ${help_pot}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${lang}
      COMMENT "Updating ${lang}.po file"
      )
    # prepare command for the all po target
    set (help_po_update_all ${help_po_update_all} && echo ${lang}.po: && ${GETTEXT_MSGMERGE_EXECUTABLE} --update --verbose ${CMAKE_CURRENT_SOURCE_DIR}/${lang}/${lang}.po ${help_pot})
  endforeach()

  # target for updating all po files
  add_custom_target (help_po
    COMMAND ${help_po_update_all}
    VERBATIM
    )



  ### optional features

  # help files export options

  # for building MS Help, we need HTML files as sources
  if(MSHELP)
    set(HTMLHELP ON)
  endif()

  # for building PDF, we need the ePub file as source
  if(PDFHELP)
    set(EPUB ON)
  endif()


  # export Help to epub

  if (EPUB)
    message(STATUS "Configuring Help to ePub export")
    foreach (lang ${help_all_langs})
      # list help pages to convert
      set (mallard_pages)
      foreach(page ${help_pages})
	list(APPEND mallard_pages ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${page})
      endforeach(page)

      # convert Mallard pages into ePub
      add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${PROJECT_NAME}-${lang}.epub
	COMMAND
        ${YELP_BUILD} epub -o ${PROJECT_NAME}-${lang}.epub ${mallard_pages}
	DEPENDS validate-${lang}-pages
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang}
	COMMENT "Generating '${lang}' ePub Help file"
	)

      add_custom_target(${lang}-epub-help
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${PROJECT_NAME}-${lang}.epub
	)

      add_dependencies(handbook ${lang}-epub-help)

      # install ePub file
      install (FILES
	${CMAKE_CURRENT_BINARY_DIR}/${lang}/${PROJECT_NAME}-${lang}.epub
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
	COMPONENT data
	)

    endforeach ()
  endif(EPUB)


  # export Help to HTML

  if(HTMLHELP)
    message(STATUS "Configuring Help to HTML export")
    foreach (lang ${help_all_langs})

      # list help pages to convert
      set (mallard_pages)
      foreach(page ${help_pages})
	list(APPEND mallard_pages ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${page})
      endforeach(page)

      # use a custom css (specify font size, hide footer..)
      set(custom_css ${CMAKE_CURRENT_SOURCE_DIR}/xml/html.css.custom.xsl)
      # convert Mallard pages into html files
      add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/html.timestamp
	COMMAND ${YELP_BUILD} html -x ${custom_css} ${mallard_pages}
	# make a tarball of all the .html files
	COMMAND ${CMAKE_COMMAND} -E tar cvzf xiphos-html-help.tar.gz *.html
	DEPENDS validate-${lang}-pages
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang}
	COMMENT "Generating '${lang}' html files"
	)
      add_custom_target(${lang}-html-help
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}/html.timestamp
	)
      add_dependencies(handbook ${lang}-html-help)

      # install HTML files
      install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos-html-help.tar.gz
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
	COMPONENT data
	)

    endforeach ()
  endif(HTMLHELP)


  # export Help to MS Help file

  if (MSHELP)
    # create Windows© Help files
    message(STATUS "Configuring Help to Windows© Help export")

    # Notes about errors reported by chmcmd:
    # 1. Warning about https://github.com/crosswire/xiphos:
    #    old versions of chmcmd do not support https links
    # 2. Error: Anchor undefined:
    #    due to missing anchors in index.html

    # generate a .chm file for each <lang>
    foreach (lang ${help_all_langs})
      # Create xiphos.hhp file (PROJECT)
      # 1. find the according Microsoft language ID
      file(STRINGS win32/ms-langID ms_langID REGEX "^${lang}[ ]")
      # if empty, default to the en-US langID
      if (NOT ms_langID)
	set(ms_langID "C 0x0409 English (United States)")
      endif ()
      # remove lang, keep only langID and country values
      string(LENGTH ${lang} len_lang)
      math(EXPR len_lang "${len_lang} + 1")
      string(SUBSTRING ${ms_langID} ${len_lang} -1 MS_LANGUAGE_CODE)

      # 2. set the .chm output file name
      set(MS_COMPILED_FILE "xiphos_${lang}.chm")

      # 3. set the list of files to compile
      foreach(line ${help_pages})
	set(MS_FILES_LIST "${MS_FILES_LIST}${line}\n")
      endforeach()
      foreach(line ${help_figures})
	set(MS_FILES_LIST "${MS_FILES_LIST}${line}\n")
      endforeach()

      # 4. create HHP file (Project file)
      configure_file(${PROJECT_SOURCE_DIR}/mhelp/win32/xiphos.hhp.in
	${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.hhp)

      # Create xiphos.hhc (TOC)
      # 1. find MS codepage for lang
      file(STRINGS win32/ms-codepages ms_codepage REGEX "^${lang}[ ]")
      # if codepage unset, default to windows-1252
      if (${lang} STREQUAL ${ms_codepage})
	message(WARNING "No codepage set for ${lang}, please add it to the win32/ms-codepages file")
	set(ms_codepage "${lang} windows-1252")
      endif ()
      # remove lang
      string(LENGTH ${lang} len_lang)
      math(EXPR len_lang "${len_lang} + 1")
      string(SUBSTRING ${ms_codepage} ${len_lang} -1 MS_WINDOWS_CODEPAGE)

      # 3. create XSLT file
      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/win32/toc.xsl.in
	${CMAKE_CURRENT_BINARY_DIR}/${lang}/toc.xsl)

      # 4. build HHC file
      add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.hhc
	# Create HHC file (TOC)
	COMMAND ${XSLTPROC} --html ${CMAKE_CURRENT_BINARY_DIR}/${lang}/toc.xsl ${CMAKE_CURRENT_BINARY_DIR}/${lang}/index.html > ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.hhc
	# Compile HTML files
	DEPENDS ${lang}-html-help
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang}
	COMMENT "Generating '${lang}' Windows HHC (Table of Contents) file"
	)

      # 6. TODO: Create a valid HHK file (Index file)
      # here is a workaround:
      file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.hhk
	"<!DOCTYPE html>"
	"<html>"
	"<head>"
	"<!-- Sitemap 1.0 -->"
	"</head><body>"
	"<object type=\"text/site properties\">"
	"</object>"
	"<ul>"
	"</ul>"
	"</body></html>"
	)

      # 7. Build CHM
      if (WIN32 AND NOT CMAKE_CROSSCOMPILING)
	# pure Windows: building help with HHC.EXE [NOTE: First switch to XSLT]
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${MS_COMPILED_FILE}
	  COMMAND ${HHC_EXE} xiphos.hhp
	  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang}
	  COMMENT "Generating '${lang}' Windows© Help file - HHC.EXE"
	  )
      else (WIN32 AND NOT CMAKE_CROSSCOMPILING)
	# Linux: building help using the chmcmd utility from Free Pascal
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${MS_COMPILED_FILE}
	  # Compile HTML files
	  COMMAND ${CHMCMD} xiphos.hhp
	  COMMAND chmod 644 ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${MS_COMPILED_FILE}
	  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.hhc
	  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang}
	  COMMENT "Generating '${lang}' Windows© Help file - chmcmd"
	  )
      endif(WIN32 AND NOT CMAKE_CROSSCOMPILING)

      # add target to current target
      add_custom_target(${lang}-windows-help
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${MS_COMPILED_FILE}
	)

      add_dependencies(handbook ${lang}-windows-help)

      # install MS-Help files
      install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${lang}/${MS_COMPILED_FILE}
	DESTINATION ${CMAKE_INSTALL_DATADIR}/help
	COMPONENT data
	)

    endforeach (lang)
  endif (MSHELP)


  # export Help to PDF

  if (PDFHELP)
    set(lang "C")
    # create a PDF file
    message(STATUS "Configuring Help to PDF export")
    # convert from ePub to PDF
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.pdf
      COMMAND ${EBOOK_CONVERT} xiphos-C.epub xiphos.pdf --base-font-size 8 --linearize-tables --chapter //h:h1 --use-auto-toc
      DEPENDS ${lang}-epub-help
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${lang}
      COMMENT "Generating '${lang}' PDF file"
      )

    # add target to current target
    add_custom_target(${lang}-pdf-help
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.pdf
      )

    add_dependencies(handbook ${lang}-pdf-help)

    # install PDF file
    install (FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${lang}/xiphos.pdf
      DESTINATION ${CMAKE_INSTALL_DOCDIR}
      COMPONENT data
      )

  endif (PDFHELP)

  add_dependencies(xiphos handbook)

endif (HELP)
