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

# copy sword utils and libs
outdir=win32/binaries/Xiphos/64/
sworddir=/usr/x86_64-w64-mingw32/sys-root/mingw/bin/

CROSS=x86_64-w64-mingw32-
CFLAGS="-I/usr/x86_64-w64-mingw32/sys-root/mingw/include -g"
LDFLAGS="-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib `x86_64-w64-mingw32-pkg-config --libs gthread-2.0`"
PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/:/usr/x86_64-w64-mingw32/sys-root/mingw/share/pkgconfig/
PKG_CONFIG_LIBDIR=/usr/x86_64-w64-mingw32/sys-root/mingw/lib/pkgconfig/:/usr/x86_64-w64-mingw32/sys-root/mingw/share/pkgconfig/
PKG_CONFIG_PREFIX=/usr/x86_64-w64-mingw32/sys-root/mingw/
MSVC_LIBPATH=/usr/x86_64-w64-mingw32/sys-root/mingw/

CC=/usr/bin/${CROSS}gcc
CXX=/usr/bin/${CROSS}g++
AR=/usr/bin/${CROSS}ar
RANLIB=/usr/bin/${CROSS}ranlib
WINRC=/usr/bin/${CROSS}windres
PKG_CONFIG=/usr/bin/${CROSS}pkg-config

cd ..

# configure and build xiphos
rm -rf $outdir
export CROSS CC CXX AR RANLIB CFLAGS LDFLAGS WINRC \
	PKG_CONFIG PKG_CONFIG_PATH PKG_CONFIG_LIBDIR \
	PKG_CONFIG_PREFIX MSVC_LIBPATH
./waf configure \
	--target-platform-win32 \
	--gtk=2 \
	--disable-dbus \
	--disable-help \
	--prefix=${outdir} \
	-d debug \
	-b build-win64
	#--disable-console
./waf
./waf install

for f in libsword.dll \
	imp2ld.exe addld.exe mod2zmod.exe imp2gbs.exe xml2gbs.exe imp2vs.exe vpl2mod.exe mkfastmod.exe mod2vpl.exe tei2mod.exe osis2mod.exe mod2osis.exe mod2imp.exe \
	installmgr.exe  diatheke.exe vs2osisreftxt.exe \
	uconv.exe icui18n48.dll icuuc48.dll icudata48.dll \
	libcairo-2.dll libfontconfig-1.dll libexpat-1.dll libfreetype-6.dll zlib1.dll libpixman-1-0.dll libpng15-15.dll libgdk-win32-2.0-0.dll libgdk_pixbuf-2.0-0.dll \
	libgio-2.0-0.dll libglib-2.0-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll libffi-5.dll libjasper-1.dll libjpeg-8.dll libtiff-3.dll libpango-1.0-0.dll \
	libpangocairo-1.0-0.dll libpangoft2-1.0-0.dll libpangowin32-1.0-0.dll libglade-2.0-0.dll libatk-1.0-0.dll libgtk-win32-2.0-0.dll libxml2-2.dll libgsf-1-114.dll \
	bz2-1.dll libgtkhtml-3.14-19.dll libgailutil-18.dll libgconf-2-4.dll libORBit-2-0.dll libgthread-2.0-0.dll libgnurx-0.dll libgtkhtml-editor-3.14-0.dll \
	libenchant-1.dll libcurl-4.dll libidn-11.dll libssh2-1.dll libgcrypt-11.dll libgpg-error-0.dll libclucene-core.dll libclucene-shared.dll \
	libjavascriptcoregtk-1.0-0.dll pthreadGC2.dll libsoup-2.4-1.dll libsqlite3-0.dll libxslt-1.dll \
	libgcc_s_sjlj-1.dll libstdc++-6.dll \
	gdb.exe libwebkitgtk-1.0-0.dll
do
    echo "Copying and stripping ${f}"
    cp ${sworddir}${f} ${outdir}bin/${f}
    #strip -o ${outdir}bin/${f} ${sworddir}${f}
done

#strip -o ${outdir}bin/libwebkitgtk-1.0-0.dll ${sworddir}/libwebkitgtk-1.0-0.dll
#strip ${outdir}bin/xiphos.exe

cp -r /usr/local/share/sword/locales.d ${outdir}share/sword/

# update gtk+ mo files

cd win32/po
for f in `ls *.po`; do msgfmt $f; done
cp messages.mo ../../${outdir}/share/locale/fa/LC_MESSAGES/gtk20.mo

# make installer
cd ../nsis
makensis installer.nsi
