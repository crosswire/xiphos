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
Usage: ${0##*/} [-win32|-win64]
Build Xiphos 32bit, 64bit or both 32/64bit windows executables (needs toolbox).
Default values are: -win32 -win64

    -h          display this help and exit
    -win32      build for 32 bit binary for Windows
    -win64	build for 64 bit binary for windows

You should invoke this script on a system that includes the "sudo" command, either
by running in a VM or in a container with your favorite container environment that
has sudo installed. Currently it depends on a Fedora 30 environment to do the builds
and won't work for later versions of Fedora because of missing dependncies. For example:

git checkout https://github.com/crosswire/xiphos.git
cd xiphos
docker build -t xiphos win32
docker run -it --rm -v "$(pwd):/source" xiphos -win32"
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
        -?*)
            printf 'WARN: Unknown option (ignored): %s\n' "$1" >&2
            ;;
        *)
            break
    esac
    shift
done
# Set default values
[ "$((WIN32+WIN64))" -lt '1' ] && WIN64='1' && WIN32='1'

# Check paths
[ ! -d "$XIPHOS_PATH" ] && die "ERROR: Xiphos repository <$XIPHOS_PATH> not found."

echo "Build for Windows 32-bit    = $([ "$WIN32" -gt 0 ] && echo 'Yes' || echo 'No')"
echo "Build for Windows 64-bit    = $([ "$WIN64" -gt 0 ] && echo 'Yes' || echo 'No')"
echo "Xiphos Source directory     = ${XIPHOS_PATH}"

trap 'exit 1' ERR

# Configure && build .EXE
function do_build {
    bits="${1}"
    mkdir -p win${bits} && cd win${bits}
    cmake -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw${bits}.cmake -DGTK2=ON -DCONSOLE=OFF "${XIPHOS_PATH}"
    make VERBOSE=1
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
