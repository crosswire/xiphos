!macro LINGUAS_USE

    !insertmacro MUI_LANGUAGE "English" ;first language is the default
    ;!insertmacro MUI_LANGUAGE "Bulgarian" ; bg
    !insertmacro MUI_LANGUAGE "Czech" ; cs
    !insertmacro MUI_LANGUAGE "German" ; de
    ;!insertmacro MUI_LANGUAGE "Spanish" ; es
    !insertmacro MUI_LANGUAGE "Farsi" ; fa
    !insertmacro MUI_LANGUAGE "Finnish" ; fi
    !insertmacro MUI_LANGUAGE "French" ; fr
    !insertmacro MUI_LANGUAGE "Hebrew" ; he_IL
    ;!insertmacro MUI_LANGUAGE "Norwegian" ; nb_NO
    !insertmacro MUI_LANGUAGE "Dutch" ; nl
    !insertmacro MUI_LANGUAGE "Portuguese" ; pt, pt_PT
    ;!insertmacro MUI_LANGUAGE "Russian" ; ru
    ;!insertmacro MUI_LANGUAGE "Slovak" ; sk
    !insertmacro MUI_LANGUAGE "Swedish" ; sv_SE
    ;!insertmacro MUI_LANGUAGE "Ukrainian" ; uk

!macroend

!macro LINGUAS_INCLUDE

    !include "i18n\utf16_en.nsh"
    !include "i18n\utf16_cs.nsh"

!macroend


;--------------------------------
; License Translations

!macro LINGUAS_LIC_FILE

    !define LIC_FILE_ENGLISH "i18n\utf16_gpl-2.0_en.txt"
    !define LIC_FILE_CZECH "i18n\utf16_gpl-2.0_cs.txt"
    !define LIC_FILE_GERMAN "i18n\utf16_gpl-2.0_de.txt"
    !define LIC_FILE_SPANISH "i18n\utf16_gpl-2.0_es.txt"
    !define LIC_FILE_FARSI "i18n\utf16_gpl-2.0_fa.txt"
    !define LIC_FILE_FINNISH "i18n\utf16_gpl-2.0_fi.txt"
    !define LIC_FILE_FRENCH "i18n\utf16_gpl-2.0_fr.txt"
    !define LIC_FILE_NORWEGIAN "i18n\utf16_gpl-2.0_nb_NO.txt"
    !define LIC_FILE_DUTCH "i18n\utf16_gpl-2.0_nl.txt"
    !define LIC_FILE_PORTUGUESE "i18n\utf16_gpl-2.0_pt.txt"
    !define LIC_FILE_RUSSIAN "i18n\utf16_gpl-2.0_ru.txt"
    !define LIC_FILE_SLOVAK "i18n\utf16_gpl-2.0_sk.txt"
    !define LIC_FILE_SWEDISH "i18n\utf16_gpl-2.0_sv_SE.txt"
    !define LIC_FILE_UKRAINIAN "i18n\utf16_gpl-2.0_uk.txt"

    ; not (yet) available GPL translations
    !define LIC_FILE_BULGARIAN "i18n\utf16_gpl-2.0_en.txt"
    !define LIC_FILE_HEBREW "i18n\utf16_gpl-2.0_en.txt"

    LicenseLangString LICENSE_FILE ${LANG_ENGLISH} "${LIC_FILE_ENGLISH}"
    LicenseLangString LICENSE_FILE ${LANG_BULGARIAN} "${LIC_FILE_BULGARIAN}"
    LicenseLangString LICENSE_FILE ${LANG_CZECH} "${LIC_FILE_CZECH}"
    LicenseLangString LICENSE_FILE ${LANG_GERMAN} "${LIC_FILE_GERMAN}"
    LicenseLangString LICENSE_FILE ${LANG_SPANISH} "${LIC_FILE_SPANISH}"
    LicenseLangString LICENSE_FILE ${LANG_FARSI} "${LIC_FILE_FARSI}"
    LicenseLangString LICENSE_FILE ${LANG_FINNISH} "${LIC_FILE_FINNISH}"
    LicenseLangString LICENSE_FILE ${LANG_FRENCH} "${LIC_FILE_FRENCH}"
    LicenseLangString LICENSE_FILE ${LANG_HEBREW} "${LIC_FILE_HEBREW}"
    LicenseLangString LICENSE_FILE ${LANG_NORWEGIAN} "${LIC_FILE_NORWEGIAN}"
    LicenseLangString LICENSE_FILE ${LANG_DUTCH} "${LIC_FILE_DUTCH}"
    LicenseLangString LICENSE_FILE ${LANG_PORTUGUESE} "${LIC_FILE_PORTUGUESE}"
    LicenseLangString LICENSE_FILE ${LANG_RUSSIAN} "${LIC_FILE_RUSSIAN}"
    LicenseLangString LICENSE_FILE ${LANG_SLOVAK} "${LIC_FILE_SLOVAK}"
    LicenseLangString LICENSE_FILE ${LANG_SWEDISH} "${LIC_FILE_SWEDISH}"
    LicenseLangString LICENSE_FILE ${LANG_UKRAINIAN} "${LIC_FILE_UKRAINIAN}"

!macroend


!macro LINGUAS_RESERVEFILE

    ReserveFile "i18n\utf16_en.nsh"
    ReserveFile "i18n\utf16_cs.nsh"

    ReserveFile "${LIC_FILE_ENGLISH}"
    ; ReserveFile "${LIC_FILE_BULGARIAN}"
    ReserveFile "${LIC_FILE_CZECH}"
    ReserveFile "${LIC_FILE_GERMAN}"
    ReserveFile "${LIC_FILE_SPANISH}"
    ReserveFile "${LIC_FILE_FARSI}"
    ReserveFile "${LIC_FILE_FINNISH}"
    ReserveFile "${LIC_FILE_FRENCH}"
    ; ReserveFile "${LIC_FILE_HEBREW}"
    ReserveFile "${LIC_FILE_NORWEGIAN}"
    ReserveFile "${LIC_FILE_DUTCH}"
    ReserveFile "${LIC_FILE_PORTUGUESE}"
    ReserveFile "${LIC_FILE_RUSSIAN}"
    ReserveFile "${LIC_FILE_SLOVAK}"
    ReserveFile "${LIC_FILE_SWEDISH}"
    ReserveFile "${LIC_FILE_UKRAINIAN}"

!macroend

