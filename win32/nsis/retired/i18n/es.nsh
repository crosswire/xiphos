;;
;;  es.nsh
;;
;;  Spanish language strings for the Xiphos NSIS installer.
;;
;;  Version 1
;;

;--------------------------------
; Uninstall Log

LangString UNLOG_HEADER ${LANG_SPANISH} \
    "=========== Registro del desinstalador no modifique este archivo ==========="
LangString UNLOG_DAT_NOT_FOUND ${LANG_SPANISH} \
    "no se encuentra ${UNINST_DAT}, no se puede desinstalar"
LangString UNLOG_DEL_FILE ${LANG_SPANISH} '¿Borrar el archivo "$R9"?'
LangString UNLOG_DEL_DIR ${LANG_SPANISH} '¿Borrar el directorio "$R9"?'
LangString UNLOG_EMPTY_DIR ${LANG_SPANISH} \
    "Se detectó una instalación previa en $0.$\n\
    el archivo requerido ${UNINSTALL_LOG}.dat no se encuentra.$\n$\n\
    Se recomienda que seleccione un \
    directoria vacío y haga una instalación \
    en limpio."
LangString UNLOG_ERROR_LOG ${LANG_SPANISH} "Error en el registro ${UNINSTALL_LOG}."
LangString UNLOG_ERROR_CREATE ${LANG_SPANISH} "Error al crear ${UNINSTALL_LOG}."


;--------------------------------
; Text for page with release notes

!insertmacro ReadmeLangStrings "" "${LANG_SPANISH}" \
    "Notas de Publicación" \
    "Por favor, revise la siguiente información." \
    "Acerca de $(^name):" \
    "$\n  Haga clic en la barra de desplazamiento o presione Av Pág para leer el texto."


;--------------------------------
; Components Page

LangString ALLUSERS_SECTITLE ${LANG_SPANISH} "Instalar para todos los usuarios"
LangString STARTMENU_SECTITLE ${LANG_SPANISH} "Accesos Directos del Menú Inicio"
LangString DESKTOP_SECTITLE ${LANG_SPANISH} "Acceso Directo en el Escritorio"
LangString QUICKLAUNCH_SECTITLE ${LANG_SPANISH} "Icono en la Barra de inicio rápido"

