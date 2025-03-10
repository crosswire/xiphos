#!/bin/bash
set -ex -o pipefail
CMAKE_OPTS=
if [ $(command -v apt-get) ]; then
	apt-get update
	export DEBIAN_FRONTEND=noninteractive
	ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime
	apt-get -y install appstream-util cmake g++ desktop-file-utils \
		fp-utils gsettings-desktop-schemas-dev intltool itstool libdbus-glib-1-dev \
		libenchant-dev libgail-3-dev libglade2-dev libgsf-1-dev libgtk-3-dev \
		libsword-dev libwebkitgtk-3.0-dev libxml2-dev libxml2-utils make \
		python-dev swig uuid-dev uuid-runtime yelp-tools xzip libbiblesync-dev \
		libsword-dev zip
	CMAKE_OPTS="-DPACKAGE_RPM:BOOL=OFF -DWEBKIT1:BOOL=ON"
elif [ $(command -v dnf) ]; then
	dnf install -y 'dnf-command(builddep)' make rpm-build zip
	dnf builddep -y xiphos
	CMAKE_OPTS="-DPACKAGE_DEB:BOOL=OFF -DGTKHTML:BOOL=ON"
fi
cd /workspace
rm -rf build
cmake -B build ${CMAKE_OPTS} .
make -C build
make -C build package package_source
