#!/bin/sh

# Tell it to error if any command errors, including inside of a pipe
set -ex -o pipefail

# Gets full path to the script
SCRIPT="$(readlink -f "${0}")"
# Gets full path to Xihpos source directory
XIPHOS_PATH="$(dirname "$(dirname "${SCRIPT}")")"

# Usage info
show_help() {
cat << EOF
Usage: ${0##*/} [-win32|-win64] [-b|-bbs=BIBLETIME_SOURCE_PATH]
Build Xiphos 32bit, 64bit or both 32/64bit windows executables (needs toolbox).
Default values are: -win32 -win64 -bbs=../biblesync

    -h          display this help and exit
    -b=BBS	use BBSS as Biblesync source path
    -win32      build for 32 bit binary for Windows
    -win64	build for 64 bit binary for windows

You should invoke this script on a system that includes the "sudo" command, either
by running in a VM or in a container with your favorite container environment that
has sudo installed. Currently it depends on a Fedora 30 environment to do the builds
and won't work for later versions of Fedora because of missing dependncies. For example:

mkdir src && cd src
git checkout https://github.com/karlkleinpaste/biblesync.git
git checkout https://github.com/crosswire/xiphos.git
docker run -it --rm -v "$(pwd):/source" fedora:30 /source/xiphos/win32/xc-xiphos-win.sh -win32 -b=/source/biblesync"
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
[ "$((WIN32+WIN64))" -lt '1' ] && WIN64='1' ; WIN32='1'
[ "$BIBLESYNC_PATH" ] || BIBLESYNC_PATH='../biblesync'

# Check paths
[ ! -d "$XIPHOS_PATH" ] && die "ERROR: Xiphos repository <$XIPHOS_PATH> not found."
[ ! -d "$BIBLESYNC_PATH" ] && die "ERROR: Biblesync repository <$BIBLESYNC_PATH> not found."

echo "Build for Windows 32-bit    = $([ "$WIN32" -gt 0 ] && echo 'Yes' || echo 'No')"
echo "Build for Windows 64-bit    = $([ "$WIN64" -gt 0 ] && echo 'Yes' || echo 'No')"
echo "Xiphos Source directory     = ${XIPHOS_PATH}"
echo "Biblesync Source directory  = ${BIBLESYNC_PATH}"



trap 'exit 1' ERR

# Update packages
echo '** Upgrading packages on the system:'
sudo dnf -y upgrade --refresh


# Install build tools
echo '** Installing build tools in the toolbox:'
sudo dnf -y install cmake git fpc glib2-devel itstool libxslt make yelp-tools zip


# Install mingw dependencies
if [ "$WIN32" -gt '0' ]; then
    echo '** Installing mingw32 dependencies:'
    sudo dnf -y install mingw32-gettext mingw32-sword mingw32-minizip mingw32-libgsf mingw32-libglade2 mingw32-webkitgtk mingw32-gtk3 mingw32-libtiff mingw32-libidn mingw32-gdb mingw32-nsis mingw32-dbus-glib
fi
if [ "$WIN64" -gt '0' ]; then
    echo '** Installing mingw64 dependencies:'
    sudo dnf -y install mingw64-gettext mingw64-sword mingw64-minizip mingw64-libgsf mingw64-libglade2 mingw64-webkitgtk mingw64-gtk3 mingw64-libtiff mingw64-libidn mingw64-gdb mingw32-nsis mingw64-dbus-glib
fi


# Build and install Biblesync with Mingw
if [ "$WIN32" -gt '0' ]; then
    echo '** Building Biblesync 32 bit:'
    mkdir -p build-bs32 && cd build-bs32
    cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr/i686-w64-mingw32/sys-root/mingw -DCMAKE_INSTALL_LIBDIR:PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib -DINCLUDE_INSTALL_DIR:PATH=/usr/i686-w64-mingw32/sys-root/mingw/include -DLIB_INSTALL_DIR:PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib -DSHARE_INSTALL_PREFIX:PATH=/usr/i686-w64-mingw32/sys-root/mingw/share -DCMAKE_SKIP_RPATH:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake -DLIBBIBLESYNC_LIBRARY_TYPE=Shared -DCMAKE_BUILD_TYPE=Debug -DCROSS_COMPILE_MINGW32=TRUE -DBUILD_SHARED_LIBS=TRUE "../${BIBLESYNC_PATH}"
    sudo make install
    cd ..
fi

if [ "$WIN64" -gt '0' ]; then
    echo '** Building Biblesync-64 bit:'
    mkdir -p build-bs64 && cd build-bs64
    cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw -DCMAKE_INSTALL_LIBDIR:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/lib -DINCLUDE_INSTALL_DIR:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/include -DLIB_INSTALL_DIR:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/lib -DSHARE_INSTALL_PREFIX:PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/share -DCMAKE_SKIP_RPATH:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw64.cmake -DLIBBIBLESYNC_LIBRARY_TYPE=Shared -DCMAKE_BUILD_TYPE=Debug -DCROSS_COMPILE_MINGW32=TRUE -DBUILD_SHARED_LIBS=TRUE "../${BIBLESYNC_PATH}"
    sudo make install
    cd ..
fi

# Configure && build .EXE
function do_build {
    bits="${1}"
    mkdir -p win${bits} && cd win${bits}
    cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw${bits}.cmake -DGTK2=ON "../${XIPHOS_PATH}"
    make -j$(nproc)
    make mhelp-epub-{C,fa,fr,it} package
    cd ..
}

if [ "$WIN32" -gt '0' ]; then
    echo '** Building Xiphos for Windows-32 bit:'
    do_build 32
fi
if [ "$WIN64" -gt '0' ]; then
    echo '** Building Xiphos for Windows-64 bit:'
    do_build 64
fi


# List of exe
echo '*Result of build'
[ "$WIN32" -gt '0' ] && mv win32/*.exe "${XIPHOS_PATH}"
[ "$WIN64" -gt '0' ] && mv win64/*.exe "${XIPHOS_PATH}"
ls -l "${XIPHOS_PATH}/"*.exe
