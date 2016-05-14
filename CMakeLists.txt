cmake_minimum_required(VERSION 3.5.2)
project(xiphos)
set(PACKAGE_VERSION "4.0.4")
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#add_subdirectory(src)
file(GLOB xiphos_SRC
	"src/*/*.h"
	"src/*/*.hh"
	"src/*/*.c"
	"src/*/*.cc"
	"src/*/*.cpp"
)

include_directories(src)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(WK2 REQUIRED webkit2gtk-4.0)
pkg_check_modules(GCONF REQUIRED gconf-2.0)
pkg_check_modules(LIBGSF REQUIRED libgsf-1)
pkg_check_modules(LIBXML REQUIRED libxml-2.0)
pkg_check_modules(GMODULE REQUIRED gmodule-2.0)

message (${GTK3_VERSION})

if (${GTK3_VERSION} VERSION_GREATER "3.2")
	add_definitions(-DHAVE_GTK_32)
endif()
if (${GTK3_VERSION} VERSION_GREATER "3.4")
	add_definitions(-DHAVE_GTK_34)
endif()
if (${GTK3_VERSION} VERSION_GREATER "3.8")
	add_definitions(-DHAVE_GTK_38)
endif()
if (${GTK3_VERSION} VERSION_GREATER "3.10")
	add_definitions(-DHAVE_GTK_310)
endif()
if (${GTK3_VERSION} VERSION_GREATER "3.12")
	add_definitions(-DHAVE_GTK_312)
endif()
if (${GTK3_VERSION} VERSION_GREATER "3.14")
	add_definitions(-DHAVE_GTK_314)
endif()
if (${GTK3_VERSION} VERSION_GREATER "3.20")
	add_definitions(-DHAVE_GTK_320)
endif()

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS}
		${WK2_INCLUDE_DIRS}
		${GCONF_INCLUDE_DIRS}
		${LIBGSF_INCLUDE_DIRS}
		${LIBXML_INCLUDE_DIRS}
		${GMODULE_INCLUDE_DIRS}
)
link_directories(${GTK3_LIBRARY_DIRS}
		${WK2_LIBRARY_DIRS}
		${GCONF_INCLUDE_DIRS}
		${LIBGSF_INCLUDE_DIRS}
		${LIBXML_INCLUDE_DIRS}
		${GMODULE_INCLUDE_DIRS}
)

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER}
		${WK2_CFLAGS_OTHER}
		${GCONF_CFLAGS_OTHER}
		${LIBGSF_CFLAGS_OTHER}
		${LIBXML_CFLAGS_OTHER}
		${GMODULE_CFLAGS_OTHER}
)
add_definitions(-DVERSION="${PACKAGE_VERSION}")
add_definitions(-DUSE_GTK_3)
add_definitions(-DUSE_CMAKE)
add_definitions(-DUSE_WEBKIT2)

add_executable(xiphos ${xiphos_SRC})

target_link_libraries(xiphos
		${GTK3_LIBRARIES}
		${WK2_LIBRARIES}
		${GCONF_LIBRARIES}
		${LIBGSF_LIBRARIES}
		${LIBXML_LIBRARIES}
		${GMODULE_LIBRARIES}
)
