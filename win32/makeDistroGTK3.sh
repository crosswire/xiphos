#!/bin/sh

#Copyright (c) 2009-2011 Matthew Talbert

#Permission is hereby granted, free of charge, to any person
#obtaining a copy of this software and associated documentation
#files (the "Software"), to deal in the Software without
#restriction, including without limitation the rights to use,
#copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following
#conditions:

#The above copyright notice and this permission notice shall be
#included in all copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#OTHER DEALINGS IN THE SOFTWARE.

set -e

# copy sword utils and libs
outdir=win32/binaries/Xiphos/32/
sworddir=/usr/i686-w64-mingw32/sys-root/mingw/bin/

CROSS=i686-w64-mingw32-
PKG_CONFIG_PATH=/usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/:/usr/i686-w64-mingw32/sys-root/mingw/share/pkgconfig/
PKG_CONFIG_LIBDIR=/usr/i686-w64-mingw32/sys-root/mingw/lib/pkgconfig/:/usr/i686-w64-mingw32/sys-root/mingw/share/pkgconfig/
PKG_CONFIG_PREFIX=/usr/i686-w64-mingw32/sys-root/mingw/
export PKG_CONFIG_PATH PKG_CONFIG_LIBDIR PKG_CONFIG_PREFIX

CFLAGS="-I/usr/i686-w64-mingw32/sys-root/mingw/include -g"
LDFLAGS="-L/usr/i686-w64-mingw32/sys-root/mingw/lib `pkg-config --libs gthread-2.0`"
MSVC_LIBPATH=/usr/i686-w64-mingw32/sys-root/mingw/

CC=/usr/bin/${CROSS}gcc
CXX=/usr/bin/${CROSS}g++
AR=/usr/bin/${CROSS}ar
RANLIB=/usr/bin/${CROSS}ranlib
WINRC=/usr/bin/${CROSS}windres
PKG_CONFIG=/usr/bin/${CROSS}pkg-config

cd ..

# Setup build tree
rm -rf $outdir
mkdir -p ${outdir}
cp -r -t ${outdir} win32/extras/*
# configure and build xiphos
export CROSS CC CXX AR RANLIB CFLAGS LDFLAGS WINRC \
	PKG_CONFIG MSVC_LIBPATH
./waf configure \
	--target-platform-win32 \
	--gtk=3 \
	--disable-dbus \
	--disable-help \
	--prefix=${outdir} \
	-d debug \
	-b build-win32
	#--disable-console

#
# GROSS HACK - TEMPORARY
# our configuration is failing to find a lot of *.h that are actually present.
# rather than fix the nightmare that is waf right now, in the interest of getting
# this release out the door, we force these items to be set.  we'll figure out
# what's wrong with waf later.
for symbol in LOCALE_H INTTYPES_H MEMORY_H STDINT_H STDLIB_H STRINGS_H STRING_H SYS_STAT_H UNISTD_H WINSOCK_H ; do
    sed -i -e "/undef.*$symbol/d" build-win32/default/config.h
done
for symbol in LOCALEDIR GNOMELOCALEDIR PACKAGE_LOCALE_DIR ; do
    sed -i -e "/$symbol/d" build-win32/default/config.h
done
ed -s build-win32/default/config.h << \EOF
$i
#define HAVE_LOCALE_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_WINSOCK_H 1
#define LOCALEDIR "../share/locale"
#define GNOMELOCALEDIR "../share/locale"
#define PACKAGE_LOCALE_DIR "../share/locale"
.
w
q
EOF
# GROSS HACK - END

./waf
./waf install

for f in libsword.dll \
	imp2ld.exe addld.exe mod2zmod.exe imp2gbs.exe xml2gbs.exe imp2vs.exe vpl2mod.exe mkfastmod.exe mod2vpl.exe tei2mod.exe osis2mod.exe mod2osis.exe mod2imp.exe \
	installmgr.exe  diatheke.exe vs2osisreftxt.exe \
	uconv.exe icui18n50.dll icuuc50.dll icudata50.dll icule50.dll \
	libcairo-gobject-2.dll iconv.dll \
	libcairo-2.dll libfontconfig-1.dll libexpat-1.dll libfreetype-6.dll zlib1.dll libpixman-1-0.dll libpng15-15.dll libgdk_pixbuf-2.0-0.dll \
	libgdk-3-0.dll \
	libgio-2.0-0.dll libglib-2.0-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll libffi-6.dll libjasper-1.dll libjpeg-62.dll libtiff-5.dll libpango-1.0-0.dll \
	libpangocairo-1.0-0.dll libpangoft2-1.0-0.dll libpangowin32-1.0-0.dll libatk-1.0-0.dll libgtk-3-0.dll libxml2-2.dll libgsf-1-114.dll \
	libbz2-1.dll libgtkhtml-4.0-0.dll libgailutil-3-0.dll libgthread-2.0-0.dll libgnurx-0.dll libgtkhtml-editor-4.0-0.dll \
	libenchant.dll libcurl-4.dll libidn-11.dll libssh2-1.dll libclucene-core.dll libclucene-shared.dll \
	libjavascriptcoregtk-3.0-0.dll pthreadGC2.dll libsoup-2.4-1.dll libsqlite3-0.dll libxslt-1.dll libintl-8.dll \
	libgcc_s_sjlj-1.dll libstdc++-6.dll \
	gdb.exe libwebkitgtk-3.0-0.dll libwebp-4.dll \
	libcrypto-10.dll libssl-10.dll libgstapp-1.0-0.dll libgstbase-1.0-0.dll libgstreamer-1.0-0.dll libgstpbutils-1.0-0.dll \
	libgstvideo-1.0-0.dll libgstaudio-1.0-0.dll libgstbase-1.0-0.dll libgstcontroller-1.0-0.dll \
	libgstfft-1.0-0.dll libgstnet-1.0-0.dll libgstriff-1.0-0.dll libgstrtp-1.0-0.dll libgstrtsp-1.0-0.dll libgstsdp-1.0-0.dll \
	libgsttag-1.0-0.dll libharfbuzz-0.dll libharfbuzz-icu-0.dll \
	gspawn-win32-helper.exe gspawn-win32-helper-console.exe
do
    echo "Copying and stripping ${f}"
    cp ${sworddir}${f} ${outdir}bin/${f}
    #strip -o ${outdir}bin/${f} ${sworddir}${f}
done

# Fetch extra libraries
for d in enchant gtk-3.0/3.0.0/immodules
do
	mkdir -p ${outdir}/lib/${d}
done
for f in enchant/libenchant_myspell.dll
do
	echo "Copying and stripping ${f}"
	cp ${sworddir}../lib/${f} ${outdir}lib/${f}
	#strip -o ${outdir}lib/${f} ${sworddir}../lib/${f}
done
for f in $(find ${sworddir}../lib/gtk-3.0/3.0.0/immodules/ -name '*.dll')
do
	echo "Copying and stripping ${f}"
	cp ${f} ${outdir}lib/gtk-3.0/3.0.0/immodules/
	#strip -o ${outdir}lib/gtk-3.0/3.0.0/immodules/`basename ${f}` $f
done
mkdir -p ${outdir}lib/pango/1.8.0/modules/
for f in $(find ${sworddir}../lib/pango/1.8.0/modules/ -name '*.dll')
do
	echo "Copying and stripping ${f}"
	cp ${f} ${outdir}lib/pango/1.8.0/modules/
	#strip -o ${outdir}lib/pango/1.8.0/modules/`basename ${f}` $f
done

# Strip the main Xiphos binary
#strip ${outdir}bin/xiphos.exe

# Copy shared files
mkdir -p ${outdir}share/sword/
cp -r ${sworddir}../share/sword/locales.d ${outdir}share/sword/locales.d
for d in enchant gtkhtml-4.0 webkitgtk-3.0 #pixmaps 
do
	cp -r ${sworddir}../share/${d} ${outdir}share/${d}
done
mkdir -p ${outdir}etc
cp -r ${sworddir}../etc/fonts ${outdir}etc/fonts
cp -r ${sworddir}../etc/pango ${outdir}etc/pango

# update gtk+ mo files

cd win32/po
for f in `ls *.po`; do msgfmt $f; done
cp messages.mo ../../${outdir}/share/locale/fa/LC_MESSAGES/gtk20.mo

# remove .svn dirs that were cp'd into the dist tree.
find ../../${outdir} -name .svn -delete #| xargs rm -r {}

# make installer
cd ../nsis
#${sworddir}makensis installer.nsi
wine ~/.wine/drive_c/Program\ Files*/NSIS/Unicode/makensis.exe installer.nsi
