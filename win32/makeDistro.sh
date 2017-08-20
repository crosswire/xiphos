#!/bin/bash

#Copyright (c) 2009-2011 Matthew Talbert
#Copyright (c) 2014-2017 Karl Kleinpaste

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

# Invocation:
# ./makeDistro.sh BITS STYLE GTK
# where:
# BITS is 32 or 64
# STYLE is debug (console window) or final (no console window, optimized)
# GTK is 2 or 3
# default build (no arg) is 32-bit, debug, 2.

#
# architecture
#
bits=32
winprefix=i686-w64-mingw32
case "$1" in
    64) bits=64
	winprefix=x86_64-w64-mingw32
	;;
    32|'') # redundant
	;;
    *)  echo '"'"$1"'"' is not 32 or 64. && exit 1
	;;
esac
shift
#
# build options, debug or final
#
buildopts="-d debug"
case "$1" in
    f|final)	buildopts="-d optimized --disable-console" ;;
    d|debug|'')	;; # redundant
    *)		echo '"'"$1"'"' is not debug or final. && exit 1
esac
shift
#
# toolkit
#
gtkver=2
gtkfiles='libglade-2.0-0.dll
	  libgailutil-18.dll
	  libgtk-win32-2.0-0.dll
	  libjavascriptcoregtk-1.0-0.dll
	  libwebkitgtk-1.0-0.dll'
wkver=1
#
# we don't use gtkhtml editor in win32 at all any more.
# libgtkhtml-3.14-19.dll libgtkhtml-editor-3.14-0.dll
#
case "$1" in
    3)	  gtkver=3
	  gtkfiles='libgailutil-3-0.dll
		    libgtk-3-0.dll
		    libjavascriptcoregtk-3.0-0.dll
		    libwebkitgtk-3.0-0.dll'
	  wkver=3
	  ;;
    2|'') # redundant
          ;;
    *)    echo '"'"$1"'"' is not 2 or 3. && exit 1
esac

echo -e '\n\n***' Building for win$bits with gtk$gtkver and flags $buildopts '***\n\n'

# exit on any error
set -e

# get in the right directory
dir=~0
[ "${dir##*/}" != win32 ] && echo -e '\n\n*** NOT IN WIN32 DIR -- CD WIN32 ***\n\n' && cd win32

# copy sword utils and libs
outdir=win32/binaries/Xiphos/"$bits"/
sworddir=/usr/"$winprefix"/sys-root/mingw/bin/

CROSS="$winprefix"-
PKG_CONFIG_PATH=/usr/"$winprefix"/sys-root/mingw/lib/pkgconfig/:/usr/"$winprefix"/sys-root/mingw/share/pkgconfig/
PKG_CONFIG_LIBDIR=/usr/"$winprefix"/sys-root/mingw/lib/pkgconfig/:/usr/"$winprefix"/sys-root/mingw/share/pkgconfig/
PKG_CONFIG_PREFIX=/usr/"$winprefix"/sys-root/mingw/
export PKG_CONFIG_PATH PKG_CONFIG_LIBDIR PKG_CONFIG_PREFIX

CFLAGS="-I/usr/$winprefix/sys-root/mingw/include -g"
CXXFLAGS="-I/usr/$winprefix/sys-root/mingw/include -g"
LDFLAGS="-L/usr/$winprefix/sys-root/mingw/lib `pkg-config --libs gthread-2.0`"
MSVC_LIBPATH=/usr/"$winprefix"/sys-root/mingw/

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
export CROSS CC CXX AR RANLIB CFLAGS CXXFLAGS LDFLAGS WINRC \
	PKG_CONFIG MSVC_LIBPATH
./waf configure \
	--target-platform-win32 \
	--gtk="$gtkver" \
	--disable-dbus \
	--disable-help \
	--prefix=${outdir} \
	--enable-webkit-editor \
	-b build-win"$bits" \
	$buildopts

#
# GROSS HACK - TEMPORARY
# our configuration is failing to find a lot of *.h that are actually present.
# rather than fix the nightmare that is waf right now, in the interest of getting
# this release out the door, we force these items to be set.  we'll figure out
# what's wrong with waf later.
for symbol in LOCALE_H INTTYPES_H MEMORY_H STDINT_H STDLIB_H STRINGS_H STRING_H SYS_STAT_H UNISTD_H WINSOCK_H ; do
    sed -i -e "/undef.*$symbol/d" build-win"$bits"/default/config.h
done
for symbol in LOCALEDIR GNOMELOCALEDIR PACKAGE_LOCALE_DIR ; do
    sed -i -e "/$symbol/d" build-win"$bits"/default/config.h
done
ed -s build-win"$bits"/default/config.h << \EOF
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

for f in $gtkfiles \
	libsword.dll \
	'../*/libbiblesync.dll' \
	libgconf-2-4.dll \
	libdbus-1-3.dll libdbus-glib-1-2.dll \
	addld.exe emptyvss.exe imp2gbs.exe imp2ld.exe imp2vs.exe \
	mkfastmod.exe mod2imp.exe mod2osis.exe mod2vpl.exe mod2zmod.exe \
	osis2mod.exe tei2mod.exe vpl2mod.exe xml2gbs.exe \
	installmgr.exe  diatheke.exe vs2osisreftxt.exe \
	uconv.exe icui18n50.dll icuuc50.dll icudata50.dll icule50.dll \
	libcairo-gobject-2.dll iconv.dll \
	libcairo-2.dll libfontconfig-1.dll libexpat-1.dll libfreetype-6.dll \
	zlib1.dll libpixman-1-0.dll libpng16-16.dll libgdk_pixbuf-2.0-0.dll \
	libgdk-3-0.dll libgdk-win32-2.0-0.dll \
	libgio-2.0-0.dll libglib-2.0-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll \
	libffi-6.dll libjasper-1.dll libjpeg-62.dll libtiff-5.dll libpango-1.0-0.dll \
	libpangocairo-1.0-0.dll libpangoft2-1.0-0.dll libpangowin32-1.0-0.dll \
	libatk-1.0-0.dll libxml2-2.dll libgsf-1-114.dll \
	libbz2-1.dll libgthread-2.0-0.dll libgnurx-0.dll \
	libenchant.dll libcurl-4.dll libidn-11.dll libssh2-1.dll \
	libclucene-core.dll libclucene-shared.dll \
	libwinpthread-1.dll libsoup-2.4-1.dll libsqlite3-0.dll libxslt-1.dll libintl-8.dll \
	'libgcc_s_*-1.dll' \
	libstdc++-6.dll \
	gdb.exe \
	libwebp-6.dll \
	libcrypto-10.dll libssl-10.dll libgstapp-1.0-0.dll \
	libgstbase-1.0-0.dll libgstreamer-1.0-0.dll libgstpbutils-1.0-0.dll \
	libgstvideo-1.0-0.dll libgstaudio-1.0-0.dll libgstbase-1.0-0.dll \
	libgstcontroller-1.0-0.dll libgstfft-1.0-0.dll libgstnet-1.0-0.dll \
	libgstriff-1.0-0.dll libgstrtp-1.0-0.dll libgstrtsp-1.0-0.dll libgstsdp-1.0-0.dll \
	libgsttag-1.0-0.dll libharfbuzz-0.dll libharfbuzz-icu-0.dll \
	libpcre-1.dll \
	'gspawn-win*-helper.exe' 'gspawn-win*-helper-console.exe'
do
    echo "Copying and stripping ${f}"
    cp ${sworddir}${f} ${outdir}bin/
    #strip -o ${outdir}bin/${f} ${sworddir}${f}
done

# there are no gtk3 equivalents of these.
if [ "$gtkver" = 2 ] ; then
    # Fetch extra libraries
    for d in gtk-2.0/2.10.0/engines gtk-2.0/modules
    do
	mkdir -p ${outdir}/lib/${d}
    done
    for f in gtk-2.0/2.10.0/engines/libpixmap.dll \
	     gtk-2.0/2.10.0/engines/libwimp.dll \
	     gtk-2.0/modules/libgail.dll
    do
	echo "Copying and stripping ${f}"
	cp ${sworddir}../lib/${f} ${outdir}lib/${f}
	#strip -o ${outdir}lib/${f} ${sworddir}../lib/${f}
    done
fi

mkdir -p ${outdir}/lib/enchant
cp ${sworddir}../lib/enchant/libenchant_myspell.dll ${outdir}lib/enchant

### pango modules are ... gone in F24?
### mkdir -p ${outdir}lib/pango/1.8.0/modules/
### # additional broken path expected by pango - duplicated path elements.
### mkdir -p ${outdir}lib/pango/lib/pango/1.8.0/modules/
### for f in $(find ${sworddir}../lib/pango/1.8.0/modules/ -name '*.dll')
### do
### 	echo "Copying and stripping ${f}"
### 	cp ${f} ${outdir}lib/pango/1.8.0/modules/
### 	# duplicate modules at the (broken) path that pango (bogusly) expects.
### 	cp ${f} ${outdir}lib/pango/lib/pango/1.8.0/modules/
### 	#strip -o ${outdir}lib/pango/1.8.0/modules/`basename ${f}` $f
### done

# Strip the main Xiphos binary
#strip ${outdir}bin/xiphos.exe

# Copy shared files
mkdir -p ${outdir}share/sword/
cp -r ${sworddir}../share/sword/locales.d ${outdir}share/sword/locales.d
for d in enchant gtkhtml-3.14 webkitgtk-"$wkver".0
do
	cp -r ${sworddir}../share/${d} ${outdir}share/${d}
done
mkdir -p ${outdir}etc
cp -r ${sworddir}../etc/fonts ${outdir}etc/fonts
#cp -r ${sworddir}../etc/gtk-2.0 ${outdir}etc/gtk-2.0
#cp win32/gtkrc ${outdir}etc/gtk-2.0/
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
wine ~/.wine/drive_c/Program\ Files*/NSIS/Unicode/makensis.exe installer"$bits".nsi
