;;
;;  fa.nsh
;;
;;  Farsi language strings for the Xiphos installer.
;;
;;  Version 1
;;

;--------------------------------
; Text for page with release notes

!insertmacro ReadmeLangStrings "" "${LANG_FARSI}" \
    "Release Notes" \
    "Please review the following important information." \
    "About $(^name):" \
    "$\n  Click on scrollbar arrows or press Page Down to review the entire text."


;--------------------------------
; Components Page

LangString ALLUSERS_SECTITLE ${LANG_FARSI} "Install for All Users"
LangString STARTMENU_SECTITLE ${LANG_FARSI} "Start Menu Shortcuts"
LangString DESKTOP_SECTITLE ${LANG_FARSI} "Desktop Shortcut"
LangString QUICKLAUNCH_SECTITLE ${LANG_FARSI} "Quick Launch Icon"

