Installation instructions
==========================

On Linux install Xiphos through you package manager, for example on Fedora:

    $ sudo dnf install xiphos-gtk3

Or on Debian, Ubuntu, or Linux Mint:

    $ sudo apt-get update
    $ sudo apt-get install xiphos

If you use Windows download the .exe file from: http://xiphos.org/download/

Compile Xiphos from source
==========================

Get source
----------

To compile Xiphos from source you will need to get a checkout of Xiphos from
GitHub. To do so, do:

    $ git clone https://github.com/crosswire/xiphos.git

Dependencies
------------

Before building Xiphos with CMake, please ensure that you have the platform
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

For *Fedora*, you need to make sure you have these packages installed:

    $ sudo dnf install cmake gcc-c++ intltool gtk3-devel dbus-glib-devel gtkhtml3-devel webkitgtk4-devel libxml2-devel libgsf-devel gconfmm26-devel sword-devel libuuid-devel biblesync-devel intltool libappstream-glib-devel desktop-file-utils itstool yelp-tools fpc

Or for *Ubuntu*, both GTK3 and GTK2 versions, install:

    $ sudo apt-get install cmake cpp intltool libdbus-glib-1-dev libwebkitgtk-3.0-dev libxml2-dev libgsf-1-dev libgconfmm-2.6-dev libsword-dev uuid-dev libwebkitgtk-dev libglade2-dev intltool appstream-util desktop-file-utils itstool libxml2-utils yelp-tools fp-utils uuid-runtime


Building Xiphos with CMake
--------------------------

CMake is a cross-platform tool for generating makefiles (as well as project files
used by IDEs).

The current version of the buildsystem is capable of building and packaging
Xiphos for the following platforms:

- Linux (GNU Makefiles, Ninja)

Should work, but untested:
- FreeBSD (GNU Makefiles)

Work in progress:
- Linux Cross compiling for Windows (GNU Makefiles, Ninja)

While the legacy build systems typically used in-source builds, it's highly
recommended to use *out-of-source* builds with CMake.

The necessary runtime dependencies such as generated source files, static
libraries, translated files are copied over to the build directory automatically
during the build process.


Commands for building Xiphos
----------------------------

This section lists the necessary commands for building Xiphos with CMake.
CMake supports different generators (like GNU/NMake Makefiles or Ninja).

The cmake command-line tool --help output lists available generators on the
current platform. Use its -G option to specify the generator for a new build tree.

Set-up the out-of-source build. Using a build tree that is separate from the
source tree will prevent CMake from generating any files in the source tree.
Create a completely separate directory:


    mkdir xiphos-build && cd xiphos-build


All examples below are for out-of-source builds with Xiphos checked out to
`<XIPHOS_SOURCE_DIR>`:

### Linux build with Ninja


    cmake <XIPHOS_SOURCE_DIR> -G Ninja
    cmake --build . -- -v  # or: ninja -v all

`CMAKE_BUILD_TYPE` defaults to `Release` (or to `Debug` if a .git dir exists)

### Linux build with Makefiles

    cmake <XIPHOS_SOURCE_DIR>
    cmake --build . -- VERBOSE=1 -j$(nproc)  # or: make VERBOSE=1 -j$(nproc)


Build Options
-------------

Xiphos supports a number of build options that can enable or disable certain
functionality. These options must be set when running CMake with
`-DENABLE_<OPTION>=<ON|OFF`. `ON` forcefully enables the dependency
and the CMake run will fail if the related dependency is not available.
This is mostly useful for packagers. `OFF` will disable the feature.


Example for forcefully enabling Gtk2 instead of Gtk3

    cmake <XIPHOS_SOURCE_DIR> -DGTK2=ON

Example for building with WebKitGtk1 and the webkit editor:

    cmake <XIPHOS_SOURCE_DIR> -DWEBKIT1=ON -DGTKHTML=OFF

List of options:

Option | Functionality | Default
------------- | -------------------------------------------- | -------
GTK2 | Force UI build with Gtk2 instead of Gtk3. | OFF
GTKHTML | Force gtkhtml editor instead of webkit editor. | OFF
WEBKIT1 | Force webkit1 instead of webkit2. | OFF
DBUS | Use the Xiphos dbus API. | ON
CHATTY | Enable lots of tracing. | OFF
HELP | Creating help files. | ON
STRIP | Strip resulting binary (Release only). | OFF
DEBUG | Enable Xiphos g_str debugging functions. | OFF
DELINT | Build with: -Wall -Werror. | OFF
CONSOLE | Enable console window in win32. | ON
POSTINST | Enable post-install tasks. | ON

Examples:
- release build with gtk3 + webkit2 + GtkHtml:
````
$ cmake <XIPHOS_SOURCE_DIR> -DGTKHTL=ON
````
- release build with Gtk2:
```
$ cmake <XIPHOS_SOURCE_DIR> -DGTK2=ON
```
- debug build:
```
$ cmake <XIPHOS_SOURCE_DIR> -DCMAKE_BUILD_TYPE=debug -DCHATTY=ON
```
For more information and an updated list of options, please check the file
cmake/XiphosOptions.cmake

Cross-Compiling Xiphos for Windows
==================================
Cross-compiling Xiphos with MinGW under Linux has been verified on Fedora
(Fedora 27).
In order to create a Windows© binary of Xiphos, you need to:
1. Install either in 64-bit or 32-bit, the MinGW C Compiler and MinGW C++
Compiler (mingw64-gcc..) and all Xiphos dependencies (mingw64-sword,
mingw64-webkitgtk...). Here is the list of packages to install on Fedora:
- mingw[32|64]-sword.noarch
- mingw[32|64]-GConf2.noarch
- mingw[32|64]-libgsf.noarch
- mingw[32|64]-libglade2.noarch
- mingw[32|64]-webkitgtk.noarch
- mingw[32|64]-gtk3.noarch
- mingw[32|64]-libtiff.noarch
- mingw[32|64]-libidn.noarch
- mingw[32|64]-gdb.noarch

2. Build and install Biblesync with Mingw, follow the instructions in the
INSTALL file for doing that.

You are now ready to build Xiphos for Windows©:

    $ cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw64.cmake -DGTK2=ON <XIPHOS_SOURCE_DIR>
    $ make package

The resulting EXE (Windows© installer file) will be found in your build directory.

Packaging Xiphos
================
Several targets are provided for creating packages.
- Commands are entered in the `xiphos-build` directory.

Creating Source packages
------------------------
The following command will generate `zip` and `tar.gz` source files:

    $ make package_source


Creating Binary Packages
------------------------
This command generates `DEB` packages. It should work on any linux host but it
will produce better deb packages when Debian specific tools ‘dpkg-xxx’ are
usable on the build system.

    $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DWEBKIT1=ON <XIPHOS_SOURCE_DIR>
    $ make package
