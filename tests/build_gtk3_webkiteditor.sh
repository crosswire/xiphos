#!/bin/bash
set -ve
src="$(readlink -f "$(dirname "${0}")/../")"
distro="${1}"
tag="${2}"
cd "${src}"
if [[ "${distro}" == "ubuntu" ]]; then
    export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/usr/lib/x86_64-linux-gnu/pkgconfig/;
fi
./waf configure --enable-webkit-editor --gtk=3
./waf build -j2
./waf install
