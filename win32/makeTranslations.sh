#! /bin/sh

cd ../po
file_name="/c/Program Files/CrossWire/GnomeSword/share/locale/"
locale_folder="bg"
completion="/LC_MESSAGES/gnomesword.mo"
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