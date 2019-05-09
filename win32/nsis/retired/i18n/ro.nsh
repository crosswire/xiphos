;;
;;  ro.nsh
;;
;;  Romanian language strings for the Xiphos installer.
;;
;;  Version 1
;;

;--------------------------------
; Uninstall Log

LangString UNLOG_HEADER ${LANG_ROMANIAN} \
    "=========== Jurnal de Dezinstalare, te rog nu edita acest fisier ==========="
LangString UNLOG_DAT_NOT_FOUND ${LANG_ROMANIAN} \
    "${UNINST_DAT} nu s-a gasit, nu se poate face dezinstalarea."
LangString UNLOG_DEL_FILE ${LANG_ROMANIAN} 'Sterge Fisierul "$R9"?'
LangString UNLOG_DEL_DIR ${LANG_ROMANIAN} 'Sterge Directorul "$R9"?'
LangString UNLOG_EMPTY_DIR ${LANG_ROMANIAN} \
    "S-a detectat o instalare anterioara la $0.$\n\
    Fisierul necesar ${UNINSTALL_LOG}.dat lipseste.$\n$\n\
    Este foarte recomandat sa selectezi un \
    director gol si sa faci o instalare \
    proaspata."
LangString UNLOG_ERROR_LOG ${LANG_ROMANIAN} "Eroare in jurnalul ${UNINSTALL_LOG}."
LangString UNLOG_ERROR_CREATE ${LANG_ROMANIAN} "Eroare in crearea ${UNINSTALL_LOG}."


;--------------------------------
; Text for page with release notes

!insertmacro ReadmeLangStrings "" "${LANG_ROMANIAN}" \
    "Note de demarare" \
    "Te rog sa revizuiesti urmatoarele informatii importante." \
    "Despre $(^nume):" \
    "$\n  Apasa pe sageti sau Page Down pentru a vedea tot textul."


;--------------------------------
; Components Page

LangString ALLUSERS_SECTITLE ${LANG_ROMANIAN} "Instalare pentru Toti Userii"
LangString STARTMENU_SECTITLE ${LANG_ROMANIAN} "Shortcut-uri in Meniul Start"
LangString DESKTOP_SECTITLE ${LANG_ROMANIAN} "Shortcut pe Ecran"
LangString QUICKLAUNCH_SECTITLE ${LANG_ROMANIAN} "Iconita de Lansare Rapida"

