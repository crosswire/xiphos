;;
;;  ru.nsh
;;
;;  Russian language strings for the Xiphos NSIS installer.
;;
;;  Version 1
;;

;--------------------------------
; Uninstall Log

LangString UNLOG_HEADER ${LANG_RUSSIAN} \
    "=========== Лог Программы Деинсталляции, пожалуйста, не редактируйте этот файл ==========="
LangString UNLOG_DAT_NOT_FOUND ${LANG_RUSSIAN} \
    "${UNINST_DAT} не найден, невозможно произвести деинсталляцию."
LangString UNLOG_DEL_FILE ${LANG_RUSSIAN} 'Удалить Файл "$R9"?'
LangString UNLOG_DEL_DIR ${LANG_RUSSIAN} 'Удалить Директорию "$R9"?'
LangString UNLOG_EMPTY_DIR ${LANG_RUSSIAN} \
    "Найдена предыдущая установка в $0.$\n\
    Необходимый файл ${UNINSTALL_LOG}.dat не найден.$\n$\n\
    Настоятельно рекомендуется выбрать \
    пустую директорию и произвести установку \
    начисто."
LangString UNLOG_ERROR_LOG ${LANG_RUSSIAN} "Ошибка в логе ${UNINSTALL_LOG}."
LangString UNLOG_ERROR_CREATE ${LANG_RUSSIAN} "Ошибка создания ${UNINSTALL_LOG}."


;--------------------------------
; Text for page with release notes

!insertmacro ReadmeLangStrings "" "${LANG_RUSSIAN}" \
    "Информация" \
    "Пожалуйста, изучите нижеприведенную важную информацию." \
    "О $(^name):" \
    "$\n  Используйте полосу прокрутки или нажмите Page Down чтобы просмотреть весь текст."


;--------------------------------
; Components Page

LangString ALLUSERS_SECTITLE ${LANG_RUSSIAN} "Установка для Всех Пользователей"
LangString STARTMENU_SECTITLE ${LANG_RUSSIAN} "Ярлыки в Главном Меню"
LangString DESKTOP_SECTITLE ${LANG_RUSSIAN} "Ярлык на Рабочем Столе"
LangString QUICKLAUNCH_SECTITLE ${LANG_RUSSIAN} "Иконка в Панели Быстрого Запуска"

