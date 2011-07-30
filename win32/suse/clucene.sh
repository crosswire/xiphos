#! /bin/sh

triplet=i686-w64-mingw32
prefix=/usr/$triplet/sys-root/mingw

cd clucene-core-0.9.21b
rm -rf autom4te.cache
./autogen.sh
./configure --host=$triplet --enable-shared --disable-static --prefix=$prefix --disable-multithreading lt_cv_deplibs_check_method=pass_all

make -j3
