#!/bin/sh

#Copyright (c) 2009 Matthew Talbert

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

cd ..

# configure and build xiphos
./waf configure --enable-gtkhtml --disable-console --prefix='C:/msys/home/xiphos/win32/binaries/Xiphos' --debug-level='optimized'
./waf
./waf install

# convert English help manual to chm
cd help/C
xsltproc /share/docbook/htmlhelp/htmlhelp.xsl xiphos.xml
hhc htmlhelp.hhp
cp htmlhelp.chm ../../win32/binaries/Xiphos/share/help/C/xiphos.chm

# convert French help manual to chm
cd ../fr
xsltproc /share/docbook/htmlhelp/htmlhelp.xsl xiphos.xml
hhc htmlhelp.hhp
cp htmlhelp.chm ../../win32/binaries/Xiphos/share/help/fr/xiphos_fr.chm

cd ../../

# copy sword utils and libs
outdir=win32/binaries/Xiphos/bin/
origdir=/usr/local/bin/

for f in libsword-1-6-0.dll imp2ld.exe addld.exe mod2zmod.exe imp2gbs.exe xml2gbs.exe installmgr.exe imp2vs.exe vpl2mod.exe mkfastmod.exe mod2vpl.exe vs2osisreftxt.exe vs2osisreftxt.exe tei2mod.exe osis2mod.exe mod2osis.exe mod2imp.exe diatheke.exe uconv.exe icuin40.dll icuuc40.dll
do
    cp $origdir$f $outdir$f
    strip $outdir$f
done

strip $outdir"xiphos.exe"

cp -r /usr/local/share/sword/locales.d $outdir"../share/sword/locales.d"

# make installer
cd win32/nsis
makensis installer.nsi



