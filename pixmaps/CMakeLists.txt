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

# install pixmaps files
install (FILES
  about.png
  book_closed.png
  book_closed_rtol.png
  book_open.png
  book_open_rtol.png
  dlg-un.png
  epiphany-bookmark-page.png
  epiphany-secure.png
  epiphany-unsecure.png
  gnome-window-manager.png
  gs2-48x48.png
  indexed-16.png
  indexed.png
  init-settings.png
#  mozilla-icon.png
  splash.png
  sword-big.png
  tt.png
  window-close.png
  xiphos-button-125.png
  gs2.xpm
  xiphos.xpm
  gs2-48x48.ico
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
  COMPONENT data
  )
