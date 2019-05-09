Installation instructions
==========================

GNU/Linux distributions:
-----------------------
For regular users, we strongly advise that you use your distribution's packages, they usually provide integration to your distribution and you will automatically get security updates.

On *Fedora*, *Red Hat* or *CentOS*:

    $ sudo dnf install xiphos-gtk3

On *Debian*, *Ubuntu*, or *Linux Mint*:

    $ sudo apt-get update
    $ sudo apt-get install xiphos

For others distributions, check your distribution's help for how to do this.

Windows®:
-------
Download the latest .exe file from: http://xiphos.org/download/ or https://github.com/crosswire/xiphos/releases


Compile Xiphos from source
==========================

Current issues
--------------
Due to a bug in yelp-tools, avoid to have a space in your path to the build directory.
The build will fail quite miserably otherwise, unless you disable building EPUB files. (-DEPUB=OFF)


Dependencies
------------

Before building Xiphos, please ensure that you have the platform
specific dependencies installed:

    gcc                    GCC, the GNU Compiler Collection
    intltool               Set of tools to centralize translation
    libxml2                XML C parser and toolkit
    libgsf                 The G Structured File Library
    gconfmm                C++ wrappers for GConf
    libuuid                universally unique identifier library
    sword >= 1.8.0         CrossWire Bible Society's Bible software
    biblesync              Protocol to support Bible software shared co-navigation
    Gtk+-2.0 or GTK+-3.0   The GIMP Toolkit
    WebKit1 or Webkit2     Port to Gtk+ of the WebKit rendering engine

Optional Dependencies:

    gtkhtml                Lightweight HTML rendering/editing engine
    dbus-glib              API for use of D-Bus from GLib applications

Recommended tools:

    intltool               An internationalization tool
    appstream-util         Utility to validate AppStream metadata
    desktop-file-validate  Validates a desktop file
    itstool                Translates XML documents with PO files
    xmllint                Validates an XML file against an XSD schema
    yelp-build             Creates HTML, EPUB, and other files from help files
	chmcmd                 Creates a Compressed HTML help file (chm)


Get source
----------

To compile Xiphos from source you will need to get a checkout of Xiphos from
GitHub. To do so, do:

    $ git clone https://github.com/crosswire/xiphos.git


Configuring build with CMake
-----------------------------
Xiphos uses CMake to compile code.

CMake is a cross-platform tool for generating makefiles (as well as project files
used by IDEs).

The current version of the buildsystem is capable of building and packaging
Xiphos for the following platforms:

- Linux (GNU Makefiles, Ninja)

Should work, but untested:

- FreeBSD (GNU Makefiles)

Work in progress:

- Linux Cross compiling for Windows® (GNU Makefiles, Ninja)

### Out-of-source tree builds

While the legacy build systems typically used in-source builds, it's highly
recommended to use *out-of-source* builds with CMake.

The necessary runtime dependencies such as generated source files, static
libraries, translated files are copied over to the build directory automatically
during the build process.

Using a build tree that is separate from the source tree will prevent CMake from
generating any files in the source tree, keeping it clean.

To set-up an out-of-source build, create a completely separate directory, usually
as a sibling of the xiphos directory:

    $ ls
	xiphos
    $ mkdir build
	$ ls
	build xiphos
	$ cd build

### Configure CMake options

`CMAKE_INSTALL_PREFIX` Install directory. This variable defaults to `/usr/local` on UNIX and `c:/Program Files/${PROJECT_NAME}` on Windows®.
For example, for setting up install in `/usr`, add: `-DCMAKE_INSTALL_PREFIX=/usr`

`CMAKE_BUILD_TYPE` defaults to `Release` (or to `Debug` if a .git dir exists)
Possible values are empty, `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`.
For forcing to `Release`, add: `-DCMAKE_BUILD_TYPE=Release`

`CMAKE_GENERATOR` A CMake Generator is responsible for writing the input files for a native build system:
- Makefiles for GNU Make: standard feature
- Ninja files for Ninja: `-GNinja`

For other options, see:
https://cmake.org/cmake/help/v3.14/manual/cmake-variables.7.html?highlight=variables


### Configure Xiphos Options

Xiphos supports a number of build options that can enable or disable certain
functionality. These options must be set when running CMake with
`-DENABLE_<OPTION>=<ON|OFF`. `ON` forcefully enables the dependency
and the CMake run will fail if the related dependency is not available.
This is mostly useful for packagers. `OFF` will disable the feature.


Example for forcefully enabling Gtk2 instead of Gtk3

    $ cmake -DGTK2=ON ../xiphos

Example for building with WebKitGtk1 and the webkit editor:

    $ cmake -DWEBKIT1=ON -DGTKHTML=OFF ../xiphos

List of options:

Option   | Functionality                                  | Default
-------- | ---------------------------------------------- | -------
GTK2     | Force UI build with Gtk2 instead of Gtk3.      | OFF
GTKHTML  | Force gtkhtml editor instead of webkit editor. | OFF
WEBKIT1  | Force webkit1 instead of webkit2.              | OFF
DBUS     | Use the Xiphos dbus API.                       | ON
CHATTY   | Enable lots of tracing.                        | OFF
HELP     | Creating help files.                           | ON
STRIP    | Strip resulting binary (Release only).         | OFF
DEBUG    | Enable Xiphos g_str debugging functions.       | OFF
DELINT   | Build with: -Wall -Werror.                     | OFF
CONSOLE  | Enable console window in win32.                | ON
POSTINST | Enable post-install tasks.                     | ON


For more information and an updated list of options, please check the file
`cmake/XiphosOptions.cmake`


Build options
-------------

### Build with Makefiles

GNU/Make is the standard build automation tool that automatically builds executable programs and libraries from source code by reading Makefiles.

    $ make -j$(nproc)
or:

    $ make VERBOSE=1 -j$(nproc)
or:

    $ cmake --build . -- VERBOSE=1 -j$(nproc)


### Build with Ninja

Ninja is an alternative build system with a focus on speed. Use `-G Ninja` to force CMake to generate Ninja build files during configuration process.

    $ ninja -v all
or:

    $ cmake --build . -- -v


Build Xiphos on *Fedora*, *Red Hat* or *CentOS*:
------------------------------------------------

### Specific requirements

It is recommended to use Fedora-Toolbox, a Linux® container that isolates processes from the rest of the system.

    $ sudo dnf install fedora-toolbox

All dependencies will be installed in the container, without affecting the base operating system.

### 1. Clone git

    $ git https://github.com/crosswire/xiphos.git

### 2. Create and enter the build directory

Create a build directory as a sibling of the xiphos directory:

    $ mkdir build
    $ ls
	build xiphos
	$ cd build

### 3. Create a toolbox container

    $ toolbox create --container xiphos

### 4. Enter toolbox

	$ toolbox enter --container xiphos

### 5. Install dependencies

    $ sudo dnf install cmake gcc-c++ intltool make gtk3-devel dbus-glib-devel gtkhtml3-devel webkitgtk4-devel libxml2-devel libgsf-devel gconfmm26-devel sword-devel libuuid-devel biblesync-devel intltool libappstream-glib-devel desktop-file-utils itstool yelp yelp-tools

### 6. Configure build

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DGTKHTML=ON ../xiphos

### 7. Build and install, run xiphos

    $ make -j$(nproc) && sudo make install
	$ cd ~
	$ xiphos

### 8. Exit toolbox

    $ exit


Build Xiphos On *Debian*, *Ubuntu*, or *Linux Mint*:
----------------------------------------------------

### 1. Install the required dependencies:

    $ sudo apt-get install cmake cpp intltool libdbus-glib-1-dev libwebkitgtk-3.0-dev libxml2-dev libgsf-1-dev libgconfmm-2.6-dev libsword-dev uuid-dev libwebkitgtk-dev libglade2-dev intltool appstream-util desktop-file-utils itstool libxml2-utils yelp-tools fp-utils uuid-runtime

### 2. Clone git

    $ git https://github.com/crosswire/xiphos.git

### 3. Create and enter the build directory

Create a build directory as a sibling of the xiphos directory:

    $ mkdir build
    $ ls
	build xiphos
	$ cd build

### 4. Configuration:

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DWEBKIT1=ON ../xiphos

### 5. Build and install

    $ make -j$(nproc) && make install


Packaging Xiphos
================
Several targets are provided for creating packages.
Commands are entered in the `build` directory.

Generating Source packages
--------------------------
The following command will generate `zip` and `tar.gz` source files:

    $ make package_source


Generating DEB Packages
-----------------------

This command generates `DEB` packages. It should work on any linux host but it
will produce better deb packages when Debian specific tools ‘dpkg-xxx’ are
usable on the build system.

### Requirements

- `dpkg`    used to install, remove, and provide information about .deb packages.
- `file`    recognize the type of data contained in a file.

### Commands

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DWEBKIT1=ON ../xiphos
    $ make -j$(nproc)
	$ cpack -G DEB


Generating RPM Package
----------------------

This command generates a `RPM` package. A standard `spec` file is generated as
well for building a regular RPM package.

### Requirements

- `rpmbuild` is used to build both binary and source software packages

### Commands

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DGTKHTML=ON ../xiphos
    $ make -j$(nproc)
    $ cpack -G RPM


Cross-Compiling Xiphos for Windows®
===================================

__Warning: Cross-compiling for Windows® is still a work-in-progress.__

These instructions describe cross-compilation for Windows® from Fedora 30 using its MinGW cross-compilation support as this is what developers use.

In general, the instructions work on the current version of Fedora, mainly due to the great support from Greg Hellings for providing mingw packages of Sword, CLucene and other required libraries.

Compilation on other versions will require modifications. Not to mention other distributions.

Specific requirements:
---------------------
It is recommended to use Fedora-Toolbox, a Linux® container that isolates processes from the rest of the system.

    $ sudo dnf install fedora-toolbox

All dependencies, MinGW C and MinGW C++ Compilers, either 64-bit or 32-bit will be installed in the container, without affecting the base operating system.

Building for 32-bit Windows®:
----------------------------

### 1. Clone Xiphos git

    $ git https://github.com/crosswire/xiphos.git

### 2. Clone Biblesync git

    $ git clone https://github.com/karlkleinpaste/biblesync.git

### 3. Create and enter the build directory

Create a build directory as a sibling of the xiphos and biblesync directories:

    $ mkdir build-win32
    $ ls
	biblesync build-win32 xiphos
	$ cd build-win32

### 4. Create a toolbox container

    $ toolbox create --container xiphos-win32

### 5. Enter toolbox

	$ toolbox enter --container xiphos-win32

### 6. Install build tools

    $ sudo dnf install cmake fpc gettext glib2-devel itstool libxslt make yelp-tools

### 7. Install mingw32 dependencies

    $ sudo dnf install mingw32-sword.noarch mingw32-GConf2.noarch mingw32-libgsf.noarch mingw32-libglade2.noarch mingw32-webkitgtk.noarch mingw32-gtk3.noarch mingw32-libtiff.noarch mingw32-libidn.noarch mingw32-gdb.noarch mingw32-nsis

### 8. Build and install Biblesync with Mingw32
Cross-compiling Biblesync using MinGW32. Check the instructions in the INSTALL
file for doing that.

    $ source ../biblesync/win32.opts
    $ mkdir build-bs32 && cd build-bs32
	$ cmake $WIN32_OPTS -DBUILD_SHARED_LIBS=TRUE ../../biblesync
    $ make && sudo make install
	$ cd ..

### 9. Configure
You are now ready to build Xiphos for Windows®:

    $ cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake -DGTK2=ON ../xiphos

### 10. Build .EXE

    $ make -j$(nproc) package

The resulting EXE (32-bit Windows® installer file) will be found in your build directory.

### 11. Exit toolbox

    $ exit


Building for 64-bit Windows®:
----------------------------

### 1. Clone Xiphos git

    $ git https://github.com/crosswire/xiphos.git

### 2. Clone Biblesync git

    $ git clone https://github.com/karlkleinpaste/biblesync.git

### 3. Create and enter the build directory

Create a build directory as a sibling of the xiphos and biblesync directories:

    $ mkdir build-win64
    $ ls
	biblesync build-win64 xiphos
	$ cd build-win64

### 4. Create a toolbox container

    $ toolbox create --container xiphos-win64

### 5. Enter toolbox

	$ toolbox enter --container xiphos-win64

### 6. Install build tools

    $ sudo dnf install cmake fpc gettext glib2-devel itstool libxslt make yelp-tools

### 7. Install mingw dependencies

    $ sudo dnf install mingw64-sword.noarch mingw64-GConf2.noarch mingw64-libgsf.noarch mingw64-libglade2.noarch mingw64-webkitgtk.noarch mingw64-gtk3.noarch mingw64-libtiff.noarch mingw64-libidn.noarch mingw64-gdb.noarch mingw32-nsis

### 8. Build and install Biblesync with Mingw
Cross-compiling Biblesync using MinGW. Check the instructions in the INSTALL
file for doing that.

    $ source ../biblesync/win64.opts
    $ mkdir build-bs64 && cd build-bs64
	$ cmake $WIN64_OPTS -DBUILD_SHARED_LIBS=TRUE ../../biblesync
    $ make && sudo make install
	$ cd ..

### 9. Configure
You are now ready to build Xiphos for Windows®:

    $ cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw64.cmake -DGTK2=ON ../xiphos

### 10. Build .EXE

    $ make -j$(nproc) package

The resulting EXE (64-bit Windows® installer file) will be found in your build directory.

### 11. Exit toolbox

    $ exit
