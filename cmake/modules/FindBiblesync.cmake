# FindBiblesync.cmake
#
# Finds the Biblesync library
#
# This will define the following variables
#
#    BIBLESYNC_FOUND
#    BIBLESYNC_INCLUDE_DIRS
#    BIBLESYNC_LIBRARY_DIR
#    BIBLESYNC_VERSION
#
# and the following imported target
#
#     Biblesync::Biblesync
#

# find a directory containing biblesync.hh
find_path(BIBLESYNC_INCLUDE_DIR
  NAMES biblesync.hh
  PATHS ${BIBLESYNC_INCLUDE_DIRS}
  PATH_SUFFIXES biblesync)

# find a library,
# PATHS specify directories to search in addition to the default locations
find_library(BIBLESYNC_LIBRARY_DIR biblesync
  PATHS ${BIBLESYNC_LIBRARY_DIRS})

# set Found
if(BIBLESYNC_INCLUDE_DIR AND BIBLESYNC_LIBRARY_DIR)
  set(BIBLESYNC_FOUND TRUE)
endif()

# set Version
if(BIBLESYNC_INCLUDE_DIR)
  # get string holding version from biblesync-version.hh
  file(STRINGS
    "${BIBLESYNC_INCLUDE_DIR}/biblesync-version.hh"
    _BIBLESYNC_VERSION_STR_CONTENTS
    REGEX "#define BIBLESYNC_VERSION_STR "
    )
  # keep only quoted version
  string(REPLACE
    "#define BIBLESYNC_VERSION_STR "
    ""
    BIBLESYNC_QUOTED_VERSION
    ${_BIBLESYNC_VERSION_STR_CONTENTS}
    )
  # remove quotes
  string(REPLACE
    "\""
    ""
    BIBLESYNC_VERSION
    ${BIBLESYNC_QUOTED_VERSION}
    )
endif()

# Mark the named cached variables as advanced.
# An advanced variable will not be displayed in any of the cmake GUIs
mark_as_advanced(
  BIBLESYNC_FOUND
  BIBLESYNC_INCLUDE_DIR
  BIBLESYNC_LIBRARY_DIR
  BIBLESYNC_VERSION
  )

# handle the REQUIRED and VERSION-related arguments of find_package
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Biblesync
    REQUIRED_VARS BIBLESYNC_INCLUDE_DIR BIBLESYNC_LIBRARY_DIR
    VERSION_VAR BIBLESYNC_VERSION)

# If found, add target
if(BIBLESYNC_FOUND AND NOT TARGET Biblesync::Biblesync)
  add_library(PkgConfig::Biblesync INTERFACE IMPORTED)
  target_include_directories(PkgConfig::Biblesync INTERFACE ${BIBLESYNC_INCLUDE_DIR})
  target_link_libraries(PkgConfig::Biblesync INTERFACE ${BIBLESYNC_LIBRARY_DIR})
endif()
