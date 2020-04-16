#!/bin/bash
set -ex -o pipefail
if [ $(command -v apt-get) ]; then
	apt-get update
	apt-get install appstream-util cmake g++ desktop-file-utils \
		fp-utils gsettings-desktop-schemas-dev intltool itstool libdbus-glib-1-dev \
		libenchant-dev libgail-3-dev libglade2-dev libgsf-1-dev libgtk-3-dev \
		libsword-dev libwebkit2gtk-4.0-dev libxml2-dev libxml2-utils make \
		python-dev swig uuid-dev uuid-runtime yelp-tools xzip libbiblesync-dev \
		libsword-dev zip
elif [ $(command -v dnf) ]; then
	dnf install -y 'dnf-command(builddep)' make zip
	dnf builddep -y xiphos
fi
cd /workspace
rm -rf build
cmake -B build -DGTKHTML:BOOL=ON .
make -C build
make -C build package
