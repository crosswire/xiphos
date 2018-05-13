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

# install doc files
install (FILES
  ${PROJECT_SOURCE_DIR}/AUTHORS
  ${PROJECT_SOURCE_DIR}/COPYING
  ${PROJECT_SOURCE_DIR}/ChangeLog
  ${PROJECT_SOURCE_DIR}/INSTALL-cmake.md
  ${PROJECT_SOURCE_DIR}/README.md
  ${PROJECT_SOURCE_DIR}/RELEASE-NOTES
  ${PROJECT_SOURCE_DIR}/TODO
  Translating-Xiphos.md
  "Writing Xiphos Manual.md"
  Xiphos.ogg
  "i18n reminder for developers.md"
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
  COMPONENT data
  )

