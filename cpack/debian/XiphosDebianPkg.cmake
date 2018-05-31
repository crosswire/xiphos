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

# make debian packages

#message(STATUS "  Debian packaging")

#if(CMAKE_INSTALL_DIR NOT MATCHES "/usr")
#  message(WARNING "Install dir has not been set to '/usr'.")
#endif()

# May be set when invoking cpack in order to trace debug information during
# CPackDeb run.
set(CPACK_DEBIAN_PACKAGE_DEBUG OFF)

# Set individual components

# Enable component packaging for CPackDEB. If enabled (ON) multiple packages
# are generated. By default a single package is generated.
set(CPACK_DEB_COMPONENT_INSTALL "ON" )

# Specify how components are grouped
# ONE_PER_GROUP (default): creates one package file per component group
# ALL_COMPONENTS_IN_ONE : creates a single package with all (requested) component
# IGNORE : creates one package per component, i.e. IGNORE component group
set(CPACK_COMPONENTS_GROUPING "ONE_PER_GROUP")

# Set groups of a components.
set(CPACK_COMPONENT_BINARIES_GROUP "binaries")
set(CPACK_COMPONENT_UNSPECIFIED_GROUP "data")

# The Debian package version
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})

# The Debian package release - Debian revision number. This is the numbering of
# the DEB package itself, i.e. the version of the packaging and not the version
# of the content (see CPACK_DEBIAN_PACKAGE_VERSION). One may change the default
# value if the previous packaging was buggy and/or you want to put here a fancy
# Linux distro specific numbering.
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)

# The Debian package architecture
if (${CPACK_PACKAGE_ARCHITECTURE} MATCHES "x86_64")
  set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
else ()
  # TODO check other results
  set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_PACKAGE_ARCHITECTURE})
endif ()

# Generate dependencies between Debian packages if multi-component setup is used
# and "CPACK_COMPONENT_<compName>_DEPENDS" variables are set. For backward
# compatibility this feature is disabled by default.
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS OFF)

# The Debian package maintainer. Default : CPACK_PACKAGE_CONTACT
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "xiphos-devel@crosswire.org")

# The Debian package description common to all components.
if(NOT CPACK_DEBIAN_PACKAGE_DESCRIPTION)
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/debian/debian.desc" desc_lines)
  foreach(line IN LISTS desc_lines)
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_DEBIAN_PACKAGE_DESCRIPTION} ${line}\n")
  endforeach()
endif()

# The compression used for creating the Debian package.
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")

# The URL of the web site for this package, preferably (when applicable) the
# site from which the original source can be obtained and any additional
# upstream documentation or information may be found.
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://xiphos.org/")

# xiphos binary package specific variables

# Set Package control field (variable is automatically transformed to lower case).
# See https://www.debian.org/doc/debian-policy/#s-f-Source
set(CPACK_DEBIAN_BINARIES_PACKAGE_NAME ${PROJECT_NAME})

# Package file name.
# Preferred setting of this variable is DEB-DEFAULT
set(CPACK_DEBIAN_BINARIES_FILE_NAME "DEB-DEFAULT")

# The Debian package architecture. Should be set to 'all' only if package contains
# architecture agnostic data.
set(CPACK_DEBIAN_BINARIES_PACKAGE_ARCHITECTURE ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

# rename package file name in the Debian style
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.")

# May be set to ON in order to use dpkg-shlibdeps to generate package dependency
# list. You may need set CMAKE_INSTALL_RPATH to an appropriate value if you use
# this feature, because if you don’t dpkg-shlibdeps may fail to find your own
# shared libs. See https://cmake.org/Wiki/CMake_RPATH_handling.
set(CPACK_DEBIAN_BINARIES_PACKAGE_SHLIBDEPS ON)

# Debian dependencies of this package.
# If CPACK_DEBIAN_BINARIES_PACKAGE_SHLIBDEPS is set, the discovered dependencies will
# be automatically appended to CPACK_DEBIAN_BINARIES_PACKAGE_DEPENDS.
# If CPACK_DEBIAN_BINARIES_PACKAGE_DEPENDS is an empty string, only the automatically
# discovered dependencies will be set for this component.
set(CPACK_DEBIAN_BINARIES_PACKAGE_DEPENDS "xiphos-data")

# Section control field e.g. admin, devel, doc ... Default : “devel”
# See https://www.debian.org/doc/debian-policy/#s-subsections
set(CPACK_DEBIAN_BINARIES_PACKAGE_SECTION gnome)

# Priority control field e.g. required, important, standard, optional, extra
# See https://www.debian.org/doc/debian-policy/#s-priorities
set(CPACK_DEBIAN_BINARIES_PACKAGE_PRIORITY optional)

# Sets the Pre-Depends field of the Debian package. Like Depends, except that it
# also forces dpkg to complete installation of the packages named before even
# starting the installation of the package which declares the pre-dependency.
# See http://www.debian.org/doc/debian-policy/#s-binarydeps
#set(CPACK_DEBIAN_BINARIES_PACKAGE_PREDEPENDS "")

# Sets the Enhances field of the Debian package. Similar to Suggests but works
# in the opposite direction: declares that a package can enhance the
# functionality of another package.
# See http://www.debian.org/doc/debian-policy/#s-binarydeps
#set(CPACK_DEBIAN_BINARIES_PACKAGE_ENHANCES "")

# Sets the Breaks field of the Debian package. When a binary package (P)
# declares that it breaks other packages (B), dpkg will not allow the
# package (P) which declares Breaks be unpacked unless the packages that will be
# broken (B) are deconfigured first. As long as the package (P) is configured,
# the previously deconfigured packages (B) cannot be reconfigured again.
# See https://www.debian.org/doc/debian-policy/#s-breaks
set(CPACK_DEBIAN_BINARIES_PACKAGE_BREAKS xiphos)

# Sets the Conflicts field of the Debian package. When one binary package
# declares a conflict with another using a Conflicts field, dpkg will not allow
# them to be unpacked on the system at the same time. This is a stronger
# restriction than Breaks, which prevents the broken package from being
# configured while the breaking package is in the “Unpacked” state but allows
# both packages to be unpacked at the same time.
# See https://www.debian.org/doc/debian-policy/#s-conflicts
#set(CPACK_DEBIAN_BINARIES_PACKAGE_CONFLICTS "")

# Sets the Provides field of the Debian package. A virtual package is one which
# appears in the Provides control field of another package.
# See https://www.debian.org/doc/debian-policy/#s-virtual
set(CPACK_DEBIAN_BINARIES_PACKAGE_PROVIDES sword-frontend)

# Sets the Replaces field of the Debian package. Packages can declare in their
# control file that they should overwrite files in certain other packages, or
# completely replace other packages.
# See http://www.debian.org/doc/debian-policy/#s-binarydeps
set(CPACK_DEBIAN_BINARIES_PACKAGE_REPLACES "xiphos-data")

# Sets the Recommends field of the Debian package. Allows packages to declare
# a strong, but not absolute, dependency on other packages.
# See http://www.debian.org/doc/debian-policy/#s-binarydeps
#set(CPACK_DEBIAN_BINARIES_PACKAGE_RECOMMENDS "")

# Sets the Suggests field of the Debian package. Allows packages to declare a
# suggested package install grouping.
# See http://www.debian.org/doc/debian-policy/#s-binarydeps
set(CPACK_DEBIAN_BINARIES_PACKAGE_SUGGESTS sword-text)

# Indicates if the Debian policy on control files should be strictly followed.
# This overrides the permissions on the original files, following the rules set
# by Debian policy.
# See https://www.debian.org/doc/debian-policy/#s-permissions-owners
set(CPACK_DEBIAN_BINARIES_PACKAGE_CONTROL_STRICT_PERMISSION ON)

# Sets the Source field of the binary Debian package. When the binary package
# name is not the same as the source package name (in particular when several
# components/binaries are generated from one source) the source from which the
# binary has been generated should be indicated with the field Source.
# See https://www.debian.org/doc/debian-policy/#s-f-Source
set(CPACK_DEBIAN_BINARIES_PACKAGE_SOURCE ${CPACK_DEBIAN_PACKAGE_SOURCE})

# Description
set(CPACK_COMPONENT_BINARIES_DESCRIPTION "environment for Bible reading, study, and research
 ${CPACK_DEBIAN_PACKAGE_DESCRIPTION} .
 This package provides the main Xiphos program, a digital text reader with
 bookmarks and full-text search. It allows viewing multiple versions of the same
 text, for parallel reading of alternatives or translations. It also supports
 personal verse-specific notes and a rich-text study pad, both of which have
 spell checking and full-text search.
 .
 Xiphos has a built-in facility to browse and install freely-distributed content
 from CrossWire repositories.")

# xiphos-data package specific variables

# Set Package control field (variable is automatically transformed to lower case).
# See https://www.debian.org/doc/debian-policy/#s-f-Source
set(CPACK_DEBIAN_DATA_PACKAGE_NAME ${PROJECT_NAME}-data)

# The Debian package architecture. Should be set to 'all' only if package contains
# architecture agnostic data.
set(CPACK_DEBIAN_DATA_PACKAGE_ARCHITECTURE all)

# Package file name.
# Preferred setting of this variable is DEB-DEFAULT
set(CPACK_DEBIAN_DATA_FILE_NAME "DEB-DEFAULT")

# Section control field e.g. admin, devel, doc ... Default : “devel”
# See https://www.debian.org/doc/debian-policy/#s-subsections
set(CPACK_DEBIAN_DATA_PACKAGE_SECTION gnome)

# May be set to ON in order to use dpkg-shlibdeps to generate package dependency
# list. You may need set CMAKE_INSTALL_RPATH to an appropriate value if you use
# this feature, because if you don’t dpkg-shlibdeps may fail to find your own
# shared libs. See https://cmake.org/Wiki/CMake_RPATH_handling.
set(CPACK_DEBIAN_DATA_PACKAGE_SHLIBDEPS OFF)

# Sets the Suggests field of the Debian package. Allows packages to declare a
# suggested package install grouping.
# See http://www.debian.org/doc/debian-policy/#s-binarydeps
set(CPACK_DEBIAN_DATA_PACKAGE_SUGGESTS xiphos)

set(CPACK_COMPONENT_DATA_DESCRIPTION "data files for Xiphos Bible study software
 ${CPACK_DEBIAN_PACKAGE_DESCRIPTION} .
 This package contains the data files that are necessary to use Xiphos.")

# xiphos-dbg package specific variables

# Set Package control field (variable is automatically transformed to lower case).
# See https://www.debian.org/doc/debian-policy/#s-f-Source
set(CPACK_DEBIAN_DBG_PACKAGE_NAME ${PROJECT_NAME}-dbg)

# The Debian package architecture. Should be set to 'all' only if package contains
# architecture agnostic data.
set(CPACK_DEBIAN_DBG_PACKAGE_ARCHITECTURE ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})

# Package file name.
# Preferred setting of this variable is DEB-DEFAULT
set(CPACK_DEBIAN_DBG_FILE_NAME "DEB-DEFAULT")

# Section control field e.g. admin, devel, doc ... Default : “devel”
# See https://www.debian.org/doc/debian-policy/#s-subsections
set(CPACK_DEBIAN_DBG_PACKAGE_SECTION debug)

# Priority control field e.g. required, important, standard, optional, extra
# See https://www.debian.org/doc/debian-policy/#s-priorities
set(CPACK_DEBIAN_DBG_PACKAGE_PRIORITY extra)

# May be set to ON in order to use dpkg-shlibdeps to generate package dependency
# list. You may need set CMAKE_INSTALL_RPATH to an appropriate value if you use
# this feature, because if you don’t dpkg-shlibdeps may fail to find your own
# shared libs. See https://cmake.org/Wiki/CMake_RPATH_handling.
set(CPACK_DEBIAN_DBG_PACKAGE_SHLIBDEPS OFF)

# Debian dependencies of this package.
# If CPACK_DEBIAN_BINARIES_PACKAGE_SHLIBDEPS is set, the discovered dependencies will
# be automatically appended to CPACK_DEBIAN_BINARIES_PACKAGE_DEPENDS.
# If CPACK_DEBIAN_BINARIES_PACKAGE_DEPENDS is an empty string, only the automatically
# discovered dependencies will be set for this component.
set(CPACK_DEBIAN_DBG_PACKAGE_DEPENDS xiphos)

# Sets the Breaks field of the Debian package. When a binary package (P)
# declares that it breaks other packages (B), dpkg will not allow the
# package (P) which declares Breaks be unpacked unless the packages that will be
# broken (B) are deconfigured first. As long as the package (P) is configured,
# the previously deconfigured packages (B) cannot be reconfigured again.
# See https://www.debian.org/doc/debian-policy/#s-breaks
set(CPACK_DEBIAN_DBG_PACKAGE_BREAKS xiphos)

# Sets the Suggests field of the Debian package. Allows packages to declare a
# suggested package install grouping.
# See http://www.debian.org/doc/debian-policy/#s-binarydeps
set(CPACK_DEBIAN_DBG_PACKAGE_SUGGESTS sword-text)

# Sets the Provides field of the Debian package. A virtual package is one which
# appears in the Provides control field of another package.
# See https://www.debian.org/doc/debian-policy/#s-virtual
set(CPACK_DEBIAN_DBG_PACKAGE_PROVIDES sword-frontend)

# desc header and footer
set(CPACK_COMPONENT_DBG_DESCRIPTION "debug symbols for Xiphos Bible study software
 ${CPACK_DEBIAN_PACKAGE_DESCRIPTION} .
 This package contains debugging symbols that are necessary to produce
 stacktraces of Xiphos.")
