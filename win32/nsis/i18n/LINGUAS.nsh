!macro LINGUAS_USE

    !insertmacro MUI_LANGUAGE "English" ;first language is the default
    !insertmacro MUI_LANGUAGE "Bulgarian" ; bg
    !insertmacro MUI_LANGUAGE "Czech" ; cs
    !insertmacro MUI_LANGUAGE "German" ; de
    !insertmacro MUI_LANGUAGE "Spanish" ; es
    !insertmacro MUI_LANGUAGE "Farsi" ; fa
    !insertmacro MUI_LANGUAGE "Finnish" ; fi
    !insertmacro MUI_LANGUAGE "French" ; fr
    !insertmacro MUI_LANGUAGE "Hebrew" ; he_IL
    !insertmacro MUI_LANGUAGE "Norwegian" ; nb_NO
    !insertmacro MUI_LANGUAGE "Dutch" ; nl
    !insertmacro MUI_LANGUAGE "Portuguese" ; pt, pt_PT
    !insertmacro MUI_LANGUAGE "Russian" ; ru
    !insertmacro MUI_LANGUAGE "Slovak" ; sk
    !insertmacro MUI_LANGUAGE "Swedish" ; sv_SE
    !insertmacro MUI_LANGUAGE "Ukrainian" ; uk

!macroend

!macro LINGUAS_INCLUDE

    !include "i18n\utf16_en.nsh"
    !include "i18n\utf16_cs.nsh"

!macroend

!macro LINGUAS_RESERVEFILE

    ReserveFile "i18n\utf16_en.nsh"
    ReserveFile "${LIC_FILE_ENGLISH}"
    ReserveFile "i18n\utf16_cs.nsh"
    ReserveFile "${LIC_FILE_CZECH}"

!macroend

