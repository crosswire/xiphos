#! /bin/sh

triplet=i686-w64-mingw32
prefix=/usr/$triplet/sys-root/$triplet

sudo cp $prefix/share/pkgconfig/iso-codes.pc $prefix/lib/pkgconfig/

export PKG_CONFIG_LIBDIR=$prefix/lib/pkgconfig
export PKG_CONFIG_PREFIX=$prefix
export WINRC=$triplet-windres
export LINK_CXX=$triplet-g++
export LINK_CC=$triplet-gcc
export CXX=$triplet-g++
export CPP=$triplet-g++
export CC=$triplet-gcc
export RANLIB=$triplet-ranlib
export AR=$triplet-ar

cd xiphos-webkit

./waf configure --enable-webkit --target-platform-win32 --disable-dbus --prefix=$prefix --pkgconf-libdir=$prefix/lib/pkgconfig/ --pkgconf-prefix=$prefix --disable-help
