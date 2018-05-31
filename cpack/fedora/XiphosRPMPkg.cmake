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

# Generate RPM

# configure the standard spec file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/fedora/xiphos.spec.in" "${CMAKE_BINARY_DIR}/xiphos.spec" @ONLY IMMEDIATE)

# get current date
set(CURRENT_DATE "Thu May 31 2018")
if(UNIX)
  execute_process(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE ${CURRENT_DATE})
endif(UNIX)

# The RPM package summary
set(CPACK_RPM_PACKAGE_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")

# The RPM package name
set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")

# The RPM package version
set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")

# The RPM package architecture
set(CPACK_RPM_PACKAGE_ARCHITECTURE "${CPACK_PACKAGE_ARCHITECTURE}")

# The RPM package release
set(CPACK_RPM_PACKAGE_RELEASE "1")

# The dist tag that is added RPM Release: field
set(CPACK_RPM_PACKAGE_RELEASE_DIST "%{dist}")

# Package file name
set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}${CPACK_RPM_PACKAGE_RELEASE_DIST}.${CPACK_PACKAGE_ARCHITECTURE}.rpm")

# The RPM package license policy
set(CPACK_RPM_PACKAGE_LICENSE "GPLv2+")

# The RPM package vendor
set(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_VENDOR}")

# The projects URL
set(CPACK_RPM_PACKAGE_URL "${XIPHOS_WEBSITE}")

# RPM package description
set(CPACK_RPM_PACKAGE_DESCRIPTION
"Xiphos is a Bible study tool written for Linux,
UNIX, and Windows under the GNOME toolkit, offering a rich and featureful
environment for reading, study, and research using modules from The SWORD
Project and elsewhere.
This build produced by Xiphos development; not from official Fedora repo.")

# RPM spec requires field
set(CPACK_RPM_PACKAGE_REQUIRES
  "sword, biblesync, dbus-glib, libglade2, gtk3, webkitgtk4, gtkhtml3, yelp")

# RPM spec obsoletes field.
set(CPACK_RPM_PACKAGE_OBSOLETES "xiphos-gtk2, xiphos-gtk3, xiphos-common")

# RPM changelog file
# get ChangeLog date
file(TIMESTAMP ${CMAKE_SOURCE_DIR}/ChangeLog
  XIPHOS_RPM_CHANGELOG_DATE "%a %b %d %Y" UTC)
# configure ChangeLog file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/fedora/xiphos.changelog.in" "${CMAKE_CURRENT_BINARY_DIR}/ChangeLog" @ONLY IMMEDIATE)
# set RPM changelog file name
set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_CURRENT_BINARY_DIR}/ChangeLog")
