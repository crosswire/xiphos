
;--------------------------------
; Enable Translations

!macro LINGUAS_USE

    ; Unmaintained or less than 50% Xiphos translations are disabled

    !insertmacro MUI_LANGUAGE "English" ;first language is the default
    ;!insertmacro MUI_LANGUAGE "Bulgarian" ; bg
    ;!insertmacro MUI_LANGUAGE "Afrikaans" ; af
    !insertmacro MUI_LANGUAGE "Czech" ; cs
    !insertmacro MUI_LANGUAGE "Welsh" ; cy
    !insertmacro MUI_LANGUAGE "German" ; de
    !insertmacro MUI_LANGUAGE "Spanish" ; es
    !insertmacro MUI_LANGUAGE "Farsi" ; fa
    !insertmacro MUI_LANGUAGE "Finnish" ; fi
    !insertmacro MUI_LANGUAGE "French" ; fr
    !insertmacro MUI_LANGUAGE "Hebrew" ; he_IL
    !insertmacro MUI_LANGUAGE "Hungarian" ; hu
    !insertmacro MUI_LANGUAGE "Norwegian" ; nb_NO
    !insertmacro MUI_LANGUAGE "Dutch" ; nl
    !insertmacro MUI_LANGUAGE "Polish" ; pl
    !insertmacro MUI_LANGUAGE "Portuguese" ; pt, pt_PT
    !insertmacro MUI_LANGUAGE "Romanian" ; ro
    !insertmacro MUI_LANGUAGE "Russian" ; ru
    !insertmacro MUI_LANGUAGE "SimpChinese" ; zh
    !insertmacro MUI_LANGUAGE "Slovak" ; sk
    !insertmacro MUI_LANGUAGE "Swedish" ; sv_SE
    ;!insertmacro MUI_LANGUAGE "Ukrainian" ; uk

!macroend

!macro LINGUAS_INCLUDE

    !include "${LANG_DIR}\en.nsh"
    ;!include "${LANG_DIR}\af.nsh"
    !include "${LANG_DIR}\cs.nsh"
    !include "${LANG_DIR}\cy.nsh"
    !include "${LANG_DIR}\de.nsh"
    !include "${LANG_DIR}\es.nsh"
    !include "${LANG_DIR}\fa.nsh"
    !include "${LANG_DIR}\fi.nsh"
    !include "${LANG_DIR}\fr.nsh"
    !include "${LANG_DIR}\he_IL.nsh"
    !include "${LANG_DIR}\nb_NO.nsh"
    !include "${LANG_DIR}\nl.nsh"
    !include "${LANG_DIR}\pt.nsh"
    !include "${LANG_DIR}\ro.nsh"
    !include "${LANG_DIR}\ru.nsh"
    !include "${LANG_DIR}\sk.nsh"
    !include "${LANG_DIR}\sv_SE.nsh"
    !include "${LANG_DIR}\zh.nsh"

!macroend


;--------------------------------
; License Translations

!macro LINGUAS_LIC_FILE

    !define LIC_FILE_ENGLISH "${LANG_DIR}\gpl-2.0_en.txt"
    !define LIC_FILE_CZECH "${LANG_DIR}\gpl-2.0_cs.txt"
    !define LIC_FILE_WELSH "${LANG_DIR}\gpl-2.0_cy.txt"
    !define LIC_FILE_GERMAN "${LANG_DIR}\gpl-2.0_de.txt"
    !define LIC_FILE_SPANISH "${LANG_DIR}\gpl-2.0_es.txt"
    !define LIC_FILE_FARSI "${LANG_DIR}\gpl-2.0_fa.txt"
    !define LIC_FILE_FINNISH "${LANG_DIR}\gpl-2.0_fi.txt"
    !define LIC_FILE_FRENCH "${LANG_DIR}\gpl-2.0_fr.txt"
    !define LIC_FILE_NORWEGIAN "${LANG_DIR}\gpl-2.0_nb_NO.txt"
    !define LIC_FILE_DUTCH "${LANG_DIR}\gpl-2.0_nl.txt"
    !define LIC_FILE_PORTUGUESE "${LANG_DIR}\gpl-2.0_pt.txt"
    !define LIC_FILE_ROMANIAN "${LANG_DIR}\gpl-2.0_ro.txt"
    !define LIC_FILE_RUSSIAN "${LANG_DIR}\gpl-2.0_ru.txt"
    !define LIC_FILE_SLOVAK "${LANG_DIR}\gpl-2.0_sk.txt"
    !define LIC_FILE_SWEDISH "${LANG_DIR}\gpl-2.0_sv_SE.txt"
    ;!define LIC_FILE_UKRAINIAN "${LANG_DIR}\gpl-2.0_uk.txt"

    ; not (yet) available GPL translations - use English
    ;!define LIC_FILE_AFRIKAANS "${LANG_DIR}\gpl-2.0_en.txt"
    ;!define LIC_FILE_BULGARIAN "${LANG_DIR}\gpl-2.0_en.txt"
    !define LIC_FILE_HEBREW "${LANG_DIR}\gpl-2.0_en.txt"
    !define LIC_FILE_HUNGARIAN "${LANG_DIR}\gpl-2.0_en.txt"
    !define LIC_FILE_POLISH "${LANG_DIR}\gpl-2.0_en.txt"
    !define LIC_FILE_SIMPCHINESE "${LANG_DIR}\gpl-2.0_en.txt"

    LicenseLangString LICENSE_FILE ${LANG_ENGLISH} "${LIC_FILE_ENGLISH}"
    ;LicenseLangString LICENSE_FILE ${LANG_BULGARIAN} "${LIC_FILE_BULGARIAN}"
    ;LicenseLangString LICENSE_FILE ${LANG_AFRIKAANS} "${LIC_FILE_AFRIKAANS}"
    LicenseLangString LICENSE_FILE ${LANG_CZECH} "${LIC_FILE_CZECH}"
    LicenseLangString LICENSE_FILE ${LANG_WELSH} "${LIC_FILE_WELSH}"
    LicenseLangString LICENSE_FILE ${LANG_GERMAN} "${LIC_FILE_GERMAN}"
    LicenseLangString LICENSE_FILE ${LANG_SPANISH} "${LIC_FILE_SPANISH}"
    LicenseLangString LICENSE_FILE ${LANG_FARSI} "${LIC_FILE_FARSI}"
    LicenseLangString LICENSE_FILE ${LANG_FINNISH} "${LIC_FILE_FINNISH}"
    LicenseLangString LICENSE_FILE ${LANG_FRENCH} "${LIC_FILE_FRENCH}"
    LicenseLangString LICENSE_FILE ${LANG_HEBREW} "${LIC_FILE_HEBREW}"
    LicenseLangString LICENSE_FILE ${LANG_HUNGARIAN} "${LIC_FILE_HUNGARIAN}"
    LicenseLangString LICENSE_FILE ${LANG_NORWEGIAN} "${LIC_FILE_NORWEGIAN}"
    LicenseLangString LICENSE_FILE ${LANG_DUTCH} "${LIC_FILE_DUTCH}"
    LicenseLangString LICENSE_FILE ${LANG_POLISH} "${LIC_FILE_POLISH}"
    LicenseLangString LICENSE_FILE ${LANG_PORTUGUESE} "${LIC_FILE_PORTUGUESE}"
    LicenseLangString LICENSE_FILE ${LANG_ROMANIAN} "${LIC_FILE_ROMANIAN}"
    LicenseLangString LICENSE_FILE ${LANG_RUSSIAN} "${LIC_FILE_RUSSIAN}"
    LicenseLangString LICENSE_FILE ${LANG_SIMPCHINESE} "${LIC_FILE_SIMPCHINESE}"
    LicenseLangString LICENSE_FILE ${LANG_SLOVAK} "${LIC_FILE_SLOVAK}"
    LicenseLangString LICENSE_FILE ${LANG_SWEDISH} "${LIC_FILE_SWEDISH}"
    ;LicenseLangString LICENSE_FILE ${LANG_UKRAINIAN} "${LIC_FILE_UKRAINIAN}"

!macroend


!macro LINGUAS_RESERVEFILE

    ReserveFile "${LANG_DIR}\en.nsh"
    ;ReserveFile "${LANG_DIR}\af.nsh"
    ReserveFile "${LANG_DIR}\cs.nsh"
    ReserveFile "${LANG_DIR}\cy.nsh"
    ReserveFile "${LANG_DIR}\de.nsh"
    ReserveFile "${LANG_DIR}\es.nsh"
    ReserveFile "${LANG_DIR}\fa.nsh"
    ReserveFile "${LANG_DIR}\fi.nsh"
    ReserveFile "${LANG_DIR}\fr.nsh"
    ReserveFile "${LANG_DIR}\he_IL.nsh"
    ReserveFile "${LANG_DIR}\nb_NO.nsh"
    ReserveFile "${LANG_DIR}\nl.nsh"
    ReserveFile "${LANG_DIR}\pt.nsh"
    ReserveFile "${LANG_DIR}\ro.nsh"
    ReserveFile "${LANG_DIR}\ru.nsh"
    ReserveFile "${LANG_DIR}\sv_SE.nsh"
    ReserveFile "${LANG_DIR}\zh.nsh"

    ReserveFile "${LIC_FILE_ENGLISH}"
    ; ReserveFile "${LIC_FILE_BULGARIAN}"
    ;ReserveFile "${LIC_FILE_AFRIKAANS}"
    ReserveFile "${LIC_FILE_CZECH}"
    ReserveFile "${LIC_FILE_WELSH}"
    ReserveFile "${LIC_FILE_GERMAN}"
    ReserveFile "${LIC_FILE_SPANISH}"
    ReserveFile "${LIC_FILE_FARSI}"
    ReserveFile "${LIC_FILE_FINNISH}"
    ReserveFile "${LIC_FILE_FRENCH}"
    ; ReserveFile "${LIC_FILE_HEBREW}"
    ReserveFile "${LIC_FILE_NORWEGIAN}"
    ReserveFile "${LIC_FILE_DUTCH}"
    ReserveFile "${LIC_FILE_PORTUGUESE}"
    ReserveFile "${LIC_FILE_ROMANIAN}"
    ReserveFile "${LIC_FILE_RUSSIAN}"
    ; ReserveFile "${LIC_FILE_SIMPCHINESE}"
    ReserveFile "${LIC_FILE_SLOVAK}"
    ReserveFile "${LIC_FILE_SWEDISH}"
    ; ReserveFile "${LIC_FILE_UKRAINIAN}"

!macroend

