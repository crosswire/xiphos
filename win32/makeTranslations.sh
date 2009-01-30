#! /bin/sh

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


cd ../po
file_name="/c/Program Files/CrossWire/Xiphos/share/locale/"
locale_folder="bg"
completion="/LC_MESSAGES/xiphos.mo"
msgfmt --output-file="$file_name$locale_folder$completion" bg.po
locale_folder="cs"
msgfmt -o "$file_name$locale_folder$completion" cs.po
locale_folder="de"
msgfmt -o "$file_name$locale_folder$completion" de.po
locale_folder="en_GB"
msgfmt -o "$file_name$locale_folder$completion" en_GB.po
locale_folder="es"
msgfmt -o "$file_name$locale_folder$completion" es.po
locale_folder="fa"
msgfmt -o "$file_name$locale_folder$completion" fa.po
locale_folder="fi"
msgfmt -o "$file_name$locale_folder$completion" fi.po
locale_folder="fr"
msgfmt -o "$file_name$locale_folder$completion" fr.po
locale_folder="he"
msgfmt -o "$file_name$locale_folder$completion" he_IL.po
locale_folder="nb"
msgfmt -o "$file_name$locale_folder$completion" nb_NO.po
locale_folder="nl"
msgfmt -o "$file_name$locale_folder$completion" nl.po
locale_folder="pt"
msgfmt -o "$file_name$locale_folder$completion" pt.po
locale_folder="pt_BR"
msgfmt -o "$file_name$locale_folder$completion" pt.po
locale_folder="ru"
msgfmt -o "$file_name$locale_folder$completion" ru.po
locale_folder="sk"
msgfmt -o "$file_name$locale_folder$completion" sk.po
locale_folder="sv"
msgfmt -o "$file_name$locale_folder$completion" sv_SE.po
locale_folder="uk"
msgfmt -o "$file_name$locale_folder$completion" uk.po
