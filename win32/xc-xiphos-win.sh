#!/bin/sh



# Usage info
show_help() {
cat << EOF
Usage: ${0##*/} [-win32|-win64] [-s=|--src=XIPHOS_SOURCE_PATH] [-b|-bbs=BIBLETIME_SOURCE_PATH]
Build Xiphos 32bit, 64bit or both 32/64bit windows executables in a toolbox.
The result is written in the current build directory.
Default values are -win64 --src=../xiphos -bbs=../biblesync

    -h          display this help and exit
    -s=XIPHOS   use XIPHOS as Xiphos _source path
    -b=BBS	use BBSS as Biblesync source path
    -win32      build for 32 bit binary for Windows
    -win64	build for 64 bit binary for windows
EOF
}

die() {
    printf '%s\n' "$1" >&2
    exit 1
}

# Initialize all the option variables.
# This ensures we are not contaminated by variables from the environment.
WIN32='0'
WIN64='0'
X_SRC_NUM='0'
BBS_SRC_NUM='0'
unset XIPHOS_PATH
unset BIBLESYNC_PATH

while :; do
    case $1 in
        -h|-\?|--help)
	    # Display a usage synopsis.
            show_help
            exit
            ;;
        -win32)
	    # Target is win32
	    WIN32=$((WIN32+1))
            ;;
	-win64)
	    # Target is win64
	    WIN64=$((WIN64+1))
            ;;
        -s=?*|--src=?*)
	    # Path to source
            XIPHOS_PATH=${1#*=}
	    X_SRC_NUM=$((X_SRC_NUM+1))
	    [ "$X_SRC_NUM" -gt '1' ] && die 'ERROR parameter "--src" already exists.'
            ;;
        -s= |--src= )
	    # Handle the case of an empty path
            die 'ERROR: "--src" requires a non-empty option argument.'
            ;;
        -b=?*|--bbs=?*)
	    # Path to source
            BIBLESYNC_PATH=${1#*=}
	    BBS_SRC_NUM=$((BBS_SRC_NUM+1))
	    [ "$BBS_SRC_NUM" -gt '1' ] && die 'ERROR parameter "--bbt" already exists.'
            ;;
        -b= |--bbt= )
	    # Handle the case of an empty path
            die 'ERROR: "--bbt" requires a non-empty option argument.'
            ;;

        -?*)
            printf 'WARN: Unknown option (ignored): %s\n' "$1" >&2
            ;;
        *)
            break
    esac
    shift
done
# Set default values
[ "$((WIN32+WIN64))" -lt '1' ] && WIN64='1'
[ "$XIPHOS_PATH" ] || XIPHOS_PATH='../xiphos'
[ "$BIBLESYNC_PATH" ] || BIBLESYNC_PATH='../biblesync'

# Check values
[ "$((WIN32+WIN64))" -gt '1' ] && die "ERROR: Win32/Win64 Cannot build for more than 1 arch"
[ ! -d "$XIPHOS_PATH" ] && die "ERROR: Xiphos repository <$XIPHOS_PATH> not found."
[ ! -d "$BIBLESYNC_PATH" ] && die "ERROR: Biblesync repository <$BIBLESYNC_PATH> not found."

echo "Build for Windows 32-bit    = $([ "$WIN32" -gt 0 ] && echo 'Yes' || echo 'No')"
echo "Build for Windows 64-bit    = $([ "$WIN64" -gt 0 ] && echo 'Yes' || echo 'No')"
echo "Xiphos Source directory     = ${XIPHOS_PATH}"
echo "Biblesync Source directory  = ${BIBLESYNC_PATH}"



trap 'exit 1' ERR

### Check if we are running in a container
if cat /proc/1/cgroup | grep '../../init' ; then
    Die 'ERROR: ${0##*/} is running in a container.'
fi


# Create a dedicated container
echo '** Creating toolbox:'
toolbox -y create --container xiphos-win --image fedora-toolbox:30


# Update packages
echo '** Upgrading packages in the toolbox:'
toolbox run --container xiphos-win sudo dnf -y upgrade --refresh


# Install build tools
echo '** Installing build tools in the toolbox:'
toolbox run --container xiphos-win sudo dnf -y install cmake fpc gettext glib2-devel itstool libxslt make yelp-tools


# Install mingw dependencies
if [ "$WIN32" -gt '0' ]; then
    echo '** Installing mingw32 dependencies:'
    toolbox run --container xiphos-win sudo dnf -y install mingw32-sword mingw32-minizip mingw32-libgsf mingw32-libglade2 mingw32-webkitgtk mingw32-gtk3 mingw32-libtiff mingw32-libidn mingw32-gdb mingw32-nsis mingw32-dbus-glib
fi
if [ "$WIN64" -gt '0' ]; then
    echo '** Installing mingw64 dependencies:'
    toolbox run --container xiphos-win sudo dnf -y install mingw64-sword mingw64-minizip mingw64-libgsf mingw64-libglade2 mingw64-webkitgtk mingw64-gtk3 mingw64-libtiff mingw64-libidn mingw64-gdb mingw32-nsis mingw64-dbus-glib
fi

set -x
# Build and install Biblesync with Mingw
if [ "$WIN32" -gt '0' ]; then
    echo '** Building Biblesync 32 bit:'
    toolbox run --container xiphos-win mkdir -p build-bs32
    cd build-bs32
    toolbox run --container xiphos-win cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr/i686-w64-mingw32/sys-root/mingw -DCMAKE_INSTALL_LIBDIR:PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib -DINCLUDE_INSTALL_DIR:PATH=/usr/i686-w64-mingw32/sys-root/mingw/include -DLIB_INSTALL_DIR:PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib -DSHARE_INSTALL_PREFIX:PATH=/usr/i686-w64-mingw32/sys-root/mingw/share -DCMAKE_SKIP_RPATH:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake -DLIBBIBLESYNC_LIBRARY_TYPE=Shared -DCMAKE_BUILD_TYPE=Debug -DCROSS_COMPILE_MINGW32=TRUE -DBUILD_SHARED_LIBS=TRUE "../${BIBLESYNC_PATH}"
    toolbox run --container xiphos-win sudo make install
    cd ..
fi

if [ "$WIN64" -gt '0' ]; then
    echo '** Building Biblesync-64 bit:'
    toolbox run --container xiphos-win mkdir -p build-bs64
    cd build-bs64
    toolbox run --container xiphos-win cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw -DCMAKE_INSTALL_LIBDIR:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/lib -DINCLUDE_INSTALL_DIR:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/include -DLIB_INSTALL_DIR:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/lib -DSHARE_INSTALL_PREFIX:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/share -DCMAKE_SKIP_RPATH:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw64.cmake -DLIBBIBLESYNC_LIBRARY_TYPE=Shared -DCMAKE_BUILD_TYPE=Debug -DCROSS_COMPILE_MINGW32=TRUE -DBUILD_SHARED_LIBS=TRUE "../${BIBLESYNC_PATH}"
    toolbox run --container xiphos-win sudo make install
    cd ..
fi

# Configure && build .EXE
if [ "$WIN32" -gt '0' ]; then
    echo '** Building Xiphos for Windows-32 bit:'
    toolbox run --container xiphos-win cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake -DGTK2=ON "${XIPHOS_PATH}"
    toolbox run --container xiphos-win make -j$(nproc) package
fi
if [ "$WIN64" -gt '0' ]; then
    echo '** Building Xiphos for Windows-64 bit:'
    toolbox run --container xiphos-win cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw64.cmake -DGTK2=ON "${XIPHOS_PATH}"
    toolbox run --container xiphos-win make -j$(nproc) package
fi

# Removing container
echo '*Removing container'
toolbox rm -f xiphos-win
