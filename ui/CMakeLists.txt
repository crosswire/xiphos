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

# install ui files
if (NOT GTK2)
  # Gtk3 only stuff
  install (FILES
    bookmarks.gtkbuilder
    bookmarks_old.gtkbuilder
    editor_link_dialog.gtkbuilder
    editor_link_dialog_old.gtkbuilder
    export-dialog.gtkbuilder
    export-dialog_old.gtkbuilder
    markverse.gtkbuilder
    markverse_old.gtkbuilder
    module-manager.gtkbuilder
    module-manager_old.gtkbuilder
    navbar_versekey.gtkbuilder
    navbar_versekey_old.gtkbuilder
    prefs.gtkbuilder
    prefs_old.gtkbuilder
    search-dialog.gtkbuilder
    search-dialog_old.gtkbuilder
    selector-prefs.gtkbuilder
    selector-prefs_old.gtkbuilder
    xi-menus.gtkbuilder
    xi-menus_old.gtkbuilder
    xi-menus-popup.gtkbuilder
    xi-menus-popup_old.gtkbuilder
    xi-splash.gtkbuilder
    xi-splash_old.gtkbuilder
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    COMPONENT data
    )
else  (NOT GTK2)
  # Gtk2 only stuff
  install (FILES
    bookmarks.glade
    editor_link_dialog.glade
    export-dialog.glade
    markverse.glade
    module-manager.glade
    navbar_versekey.glade
    prefs.glade
    search-dialog.glade
    selector-prefs.glade
    xi-export-bookmarks.glade
    xi-menus.glade
    gtk2_webedit.ui
    gtk_webedit.ui
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
    COMPONENT data
    )
endif (NOT GTK2)

# common stuff
install (FILES
  editor_note.xml
  editor_studypad.xml
  arrow_down_box.png
  arrow_down.png
  arrow_up_box.png
  arrow_up.png
  biblesync-v1-30.png
  biblesync-v1-50.png
  biblesync_icon-16.png
  chat_icon-16.png
  dlg-un-16.png
  epiphany-bookmarks.png
  gnome-session.png
  gnome-sound-properties.png
  gnome-windows.png
  logo.png
  mailing_list_icon-16.png
  new_tab_button.png
  silk-edit-bookmarks.png
  sword3.png
  sword_icon-16.png
  sword.png
  translation_icon-16.png
  xiphos-x-16.png
  languages
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
  COMPONENT data
  )

