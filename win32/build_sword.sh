#!/usr/bin/env bash
set -ex -o pipefail

mkdir -p /workspace && cd /workspace

curl -o sword.tar.gz https://www.crosswire.org/ftpmirror/pub/sword/source/v1.9/sword-1.9.0.tar.gz
tar xzf sword.tar.gz

cd sword-1.9.0
# It confuses gcc when you have this
sed -i configure.ac -e 's#-no-undefined##'
# But ld NEEDS to have it for the shared library
sed -i lib/Makefile.am -E -e 's#libsword_la_LDFLAGS =(.*)#libsword_la_LDFLAGS = -no-undefined \1#'
autoreconf
cd ..

function build_sword {
    arch="${1}"
    mkdir -p "sword${arch}" && cd "sword${arch}"
    export CC="${arch}-w64-mingw32-gcc"
    export CXX="${arch}-w64-mingw32-g++"
    export AR="${arch}-w64-mingw32-ar"
    export RANLIB="${arch}-w64-mingw32-ranlib"
    export STRIP="${arch}-w64-mingw32-strip"
    export LD="${arch}-w64-mingw32-ld"
    export CFLAGS="-I/usr/${arch}-w64-mingw32/sys-root/mingw/include -I$(pwd)/../sword-1.9.0/include -D_ICUSWORD_"
    export CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
    export LDFLAGS="-L/usr/${arch}-w64-mingw32/sys-root/mingw/lib"
    export PATH="/usr/${arch}-w64-mingw32/sys-root/mingw/bin:${PATH}"
    export PKG_CONFIG_PATH="/usr/${arch}-w64-mingw32/sys-root/mingw/lib/pkgconfig"
    ../sword-1.9.0/configure \
        --enable-shared \
        --disable-static \
        --host=${arch}-w64-mingw32 \
        --target=${arch}-w64-mingw32 \
        --build=${arch}-linux-gnu \
        --prefix=/usr/${arch}-w64-mingw32/sys-root/mingw \
        --exec-prefix=/usr/${arch}-w64-mingw32/sys-root/mingw \
        --bindir=/usr/${arch}-w64-mingw32/sys-root/mingw/bin \
        --sbindir=/usr/${arch}-w64-mingw32/sys-root/mingw/sbin \
        --sysconfdir=/usr/${arch}-w64-mingw32/sys-root/mingw/etc \
        --datadir=/usr/${arch}-w64-mingw32/sys-root/mingw/share \
        --includedir=/usr/${arch}-w64-mingw32/sys-root/mingw/include \
        --libdir=/usr/${arch}-w64-mingw32/sys-root/mingw/lib \
        --libexecdir=/usr/${arch}-w64-mingw32/sys-root/mingw/libexec \
        --localstatedir=/usr/${arch}-w64-mingw32/sys-root/mingw/var \
        --sharedstatedir=/usr/${arch}-w64-mingw32/sys-root/mingw/com \
        --mandir=/usr/${arch}-w64-mingw32/sys-root/mingw/share/man \
        --infodir=/usr/${arch}-w64-mingw32/sys-root/mingw/share/info \
        --with-clucene=/usr/${arch}-w64-mingw32/sys-root/mingw \
        --with-icu=/usr/${arch}-w64-mingw32/sys-root/mingw || cat config.log
    make -j10
    make install
    cd ..
}

build_sword i686

export CXXFLAGS="-fpermissive -Wint-to-pointer-cast"
build_sword x86_64

rm -r /workspace
