#! /bin/sh

triplet=i686-w64-mingw32
prefix=/usr/$triplet/sys-root/mingw
CC=$triplet-gcc
export PKG_CONFIG_LIBDIR=$prefix/lib/pkgconfig
export PKG_CONFIG_PREFIX=$prefix

sudo cp $prefix/bin/icudata46.dll $prefix/lib/
sudo cp $prefix/bin/icuuc46.dll $prefix/lib/icuuc46.lib
sudo cp $prefix/bin/icui18n46.dll $prefix/lib/
sudo cp $prefix/bin/icuio46.dll $prefix/lib/

cd sword-unicode
./autogen.sh

export CFLAGS=" $CFLAGS -mms-bitfields -I$prefix/include/glib-2.0 -I$prefix/lib/glib-2.0/include -DUSBINARY"
export CXXFLAGS=" $CXXFLAGS -mms-bitfields -I$prefix/include/glib-2.0 -I$prefix/lib/glib-2.0/include -DUSBINARY"

./configure --host=$triplet --prefix=$prefix --with-icusword --enable-shared --disable-static --with-clucene=$prefix/ lt_cv_deplibs_check_method=pass_all --with-curl ac_cv_path_CURL_CONFIG=$prefix/bin/curl-config ac_cv_path_ICU_CONFIG=$prefix/bin/icu-config

make -j3


