Installation instructions
==========================

## Summary

- Installation Instructions
   - GNU/Linux packages
   - Windows® install
- Compile Xiphos from source
- Build Xiphos on *Fedora*, *Red Hat* or *CentOS*
- Build Xiphos On *Debian*, *Ubuntu*, or *Linux Mint*
- Make packages with CMake
- Building for Windows®


GNU/Linux packages:
-------------------
For regular users, we strongly advise that you use your distribution's packages,
they usually provide integration to your distribution and you will automatically
get security updates.

### *Fedora*, *Red Hat* or *CentOS*:

    $ sudo dnf install xiphos

### *Debian*, *Ubuntu*, or *Linux Mint*:

Xiphos depends on libgtkhtml that is no longer available in Ubuntu.

You have the choice to either:

- build Xiphos yourself (see Build Xiphos On *Debian*, *Ubuntu*, or *Linux Mint* below)
- install packages from  ppa: https://launchpad.net/~pkgcrosswire/+archive/ubuntu/ppa
- install the flatpak app (3rd party, may not be up to date)
  https://www.flathub.org/apps/details/org.xiphos.Xiphos  
  Note: If you want access to the normal home folders .sword and .xiphos, run:  
  `# flatpak override --filesystem=home org.xiphos.Xiphos`

### Other distributions

Check your distribution's help for how to do this.

Windows® install:
-------------------
Download the latest .exe file from: http://xiphos.org/download/ or
https://github.com/crosswire/xiphos/releases


Compile Xiphos from source
==========================

Current issues
--------------
Avoid to have a space in your path to the build or disable building EPUB files
(-DEPUB=OFF), otherwise the build will fail.


Dependencies
------------

Before building Xiphos, please ensure that you have the platform specific
dependencies installed:

	chmcmd                 Creates a Compressed HTML help file (chm)
    CMake                  Cross-platform make system
    Gtk+-2.0 or GTK+-3.0   The GIMP Toolkit
    WebKit1 or Webkit2     Port to Gtk+ of the WebKit rendering engine
    appstream-util         Utility to validate AppStream metadata
    biblesync>=1.2.0       Protocol to support Bible software shared co-navigation
    dbus-glib              API for use of D-Bus from GLib applications
    desktop-file-validate  Validates a desktop file
    gcc                    GCC, the GNU Compiler Collection
    gconfmm                C++ wrappers for GConf
    gtkhtml                Lightweight HTML rendering/editing engine
    intltool               An internationalization tool
    intltool               Set of tools to centralize translation
    itstool                Translates XML documents with PO files
    libgsf                 The G Structured File Library
    libuuid                universally unique identifier library
    libxml2                XML C parser and toolkit
    make                   A GNU tool which simplifies the build process for users
    minizip                Minizip contrib in zlib
    sword >= 1.8.1         CrossWire Bible Society's Bible software
    xmllint                Validates an XML file against an XSD schema
    yelp-build             Creates HTML, EPUB, and other files from help files

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
- Windows (Cross compiling on Fedora 30 only)

Should work, but actually untested:

- FreeBSD (GNU Makefiles)


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

`CMAKE_INSTALL_PREFIX` Install directory. This variable defaults to `/usr/local`
on UNIX and `c:/Program Files/${PROJECT_NAME}` on Windows®.
For example, for setting up install in `/usr`, add: `-DCMAKE_INSTALL_PREFIX=/usr`

`CMAKE_BUILD_TYPE` defaults to `Release` (or to `Debug` if a .git dir exists)
Possible values are empty, `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`.
For forcing to `Release`, add: `-DCMAKE_BUILD_TYPE=Release`

`CMAKE_GENERATOR` A CMake Generator is responsible for writing the input files
for a native build system:
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
================================================

## 1. Clone Xiphos git

    $ git https://github.com/crosswire/xiphos.git

## 2. Create and enter the build directory

Create a build directory as a sibling of the xiphos directory:

    $ mkdir build
    $ ls
	build xiphos
	$ cd build

## 3. Install dependencies

    $ sudo dnf install cmake gcc-c++ intltool make gtk3-devel dbus-glib-devel gtkhtml3-devel webkitgtk4-devel libidn-devel libxml2-devel libgsf-devel minizip-devel sword-devel libuuid-devel biblesync-devel intltool libappstream-glib-devel desktop-file-utils itstool yelp yelp-tools

## 4. Configure build

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DGTKHTML=ON ../xiphos

## 5. Build and install, run xiphos

    $ make -j$(nproc) && sudo make install
	$ cd ~
	$ xiphos


Build Xiphos On *Debian*, *Ubuntu*, or *Linux Mint*:
===================================================

## 1. Install GTKHtml

    $ curl -Ls -o gtkhtml-4.10.0.tar.xz https://download.gnome.org/sources/gtkhtml/4.10/gtkhtml-4.10.0.tar.xz
    $ tar xf gtkhtml-4.10.0.tar.xz
    $ cd gtkhtml-4.10.0
    $ ./configure --prefix=/usr --sysconfdir=/etc --libexecdir=/usr/lib/gtkhtml4 --localstatedir=/var --disable-static
    $ make -j2
    $ make install

## 2. Install Biblesync

    $ curl -Ls -o biblesync-1.2.0.tar.gz https://github.com/karlkleinpaste/biblesync/archive/1.2.0.tar.gz
    $ tar xf biblesync-1.2.0.tar.gz
    $ cd biblesync-1.2.0
    $ mkdir -p build
    $ cd build
    $ cmake -DBUILD_SHARED_LIBS=TRUE -DCMAKE_INSTALL_PREFIX=/usr -DLIBDIR=/usr/lib ..
    $ make -j2
    $ make install

## 3. Clone git

    $ git https://github.com/crosswire/xiphos.git

## 4. Create and enter the build directory

Create a build directory as a sibling of the xiphos directory:

    $ mkdir build
    $ ls
	build xiphos
	$ cd build

## 5. Install the required dependencies:

    $ sudo apt-get appstream-util cmake g++ desktop-file-utils fp-utils git gsettings-desktop-schemas-dev intltool itstool libdbus-glib-1-dev libenchant-dev libgail-3-dev libglade2-dev libgtk-3-dev libminizip-dev libsword-dev libwebkit2gtk-4.0-dev libxml2-dev libxml2-utils make python-dev swig uuid-dev uuid-runtime yelp-tools xzip

## 6. Configuration:

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DWEBKIT1=ON ../xiphos

## 7. Build and install

    $ make -j$(nproc) && make install


Make packages with CMake
========================
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

- `rpmbuild` is required to build both binary and source software packages

### Commands

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DGTKHTML=ON ../xiphos
    $ make -j$(nproc)
    $ cpack -G RPM

or for building a regular RPM package (recommended):

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DGTKHTML=ON ../xiphos
	$ make package_source
	$ fedpkg --release f30 local


Cross-Compiling Xiphos for Windows®
===================================

These instructions describe cross-compilation for Windows® using Fedora MinGW
cross-compilation support.

These instructions work on Fedora 30, compilation on Fedora 31+ versions will
not work anymore as wekbitgtk has been deprecated.

Specific requirements:
---------------------

We recommend to use a container, which provides an environment where development
tools and libraries can be installed and used, whithout cluttering your regular
environment.

All dependencies, MinGW C and MinGW C++ Compilers, either 64-bit or 32-bit are
currently available in cross-compile mode (from the mingw32/64 packages) on
Fedora 30, mainly due to the great support from Greg Hellings for providing
mingw packages for Sword, CLucene and other required libraries.

**Note: If you're running Fedora 31 or newer, you'll need to pull a Fedora 30
container.**


Building for Windows®:
---------------------

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

### 6. Install build tools

    $ sudo dnf install cmake fpc gettext glib2-devel itstool libxslt make yelp-tools

### 7. Install dependencies

For Win32:

    $ sudo dnf install mingw32-sword.noarch mingw32-minizip.noarch mingw32-libgsf.noarch mingw32-libglade2.noarch mingw32-webkitgtk.noarch mingw32-gtk3.noarch mingw32-libtiff.noarch mingw32-libidn.noarch mingw32-gdb.noarch mingw32-nsis mingw32-dbus-glib

For Win64:

    $ sudo dnf install mingw64-sword.noarch mingw64-minizip.noarch mingw64-libgsf.noarch mingw64-libglade2.noarch mingw64-webkitgtk.noarch mingw64-gtk3.noarch mingw64-libtiff.noarch mingw64-libidn.noarch mingw64-gdb.noarch mingw32-nsis mingw64-dbus-glib

### 8. Build and install Biblesync with Mingw
Cross-compiling Biblesync using MinGW. Check the instructions in the INSTALL.md
file for doing that.

For Win32:

    $ source ../biblesync/win32.opts
    $ mkdir -p build-bs32 && cd build-bs32
	$ cmake $WIN32_OPTS -DBUILD_SHARED_LIBS=TRUE ../../biblesync
    $ make && sudo make install
	$ cd ..

For Win64:

    $ source ../biblesync/win64.opts
    $ mkdir build-bs64 && cd build-bs64
	$ cmake $WIN64_OPTS -DBUILD_SHARED_LIBS=TRUE ../../biblesync
    $ make && sudo make install
	$ cd ..

### 9. Configure
You are now ready to build Xiphos for Windows®:

For Win32:

    $ cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake -DGTK2=ON ../xiphos

For Win64:

    $ cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw64.cmake -DGTK2=ON ../xiphos

### 10. Build .EXE

    $ make -j$(nproc) package

The resulting EXE (Windows® installer file) will be found in your build directory.


Build script for Windows®:
---------------------------
A script named `xc-xiphos-win.sh` in the win32 directory will run automatically
all the steps above.

See intructions in the file `win32/WindowsBuildNotes.txt`
