#!/usr/bin/env bash
set -ex -o pipefail

mkdir -p /workspace && cd /workspace

curl -o sword.tar.gz https://www.crosswire.org/ftpmirror/pub/sword/source/v1.9/sword-1.9.0.tar.gz
tar xzf sword.tar.gz

cd sword-1.9.0
sed -i configure.ac -e 's#-no-undefined##'
autoreconf
cd ..

function build_sword {
    arch="${1}"
    mkdir -p "sword${arch}" && cd "sword${arch}"
    export CFLAGS="-I/usr/${arch}-w64-mingw32/sys-root/mingw/include -I$(pwd)/../sword-1.9.0/include"
    export CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
    export LDFLAGS="-L/usr/${arch}-w64-mingw32/sys-root/mingw/lib"
    export PATH="/usr/${arch}-w64-mingw32/sys-root/mingw/bin:${PATH}"
    export PKG_CONFIG_PATH="/usr/${arch}-w64-mingw32/sys-root/mingw/lib/pkgconfig"
    ../sword-1.9.0/configure \
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
