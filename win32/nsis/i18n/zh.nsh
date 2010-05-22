;;
;;  zh.nsh
;;
;;  Simplified Chinese language strings for the Xiphos NSIS installer.
;;
;;  Version 1
;;

;--------------------------------
; Uninstall Log

LangString UNLOG_HEADER ${LANG_SIMPCHINESE} \
    "=========== 卸载日志请不要编辑此文件 ==========="
LangString UNLOG_DAT_NOT_FOUND ${LANG_SIMPCHINESE} \
    "${UNINST_DAT} 没有找到，无法执行卸载。"
LangString UNLOG_DEL_FILE ${LANG_SIMPCHINESE} '删除文件 "$R9"？'
LangString UNLOG_DEL_DIR ${LANG_SIMPCHINESE} '删除目录 "$R9"？'
LangString UNLOG_EMPTY_DIR ${LANG_SIMPCHINESE} \
    "以前的安装检测到 $0。$\n\
    所需的文件 ${UNINSTALL_LOG}.dat 缺少。$\n$\n\
    强烈建议选择一个 \
    空的目录并执行一个新的 \
    安装。"
LangString UNLOG_ERROR_LOG ${LANG_SIMPCHINESE} "错误日志 ${UNINSTALL_LOG}。"
LangString UNLOG_ERROR_CREATE ${LANG_SIMPCHINESE} "错误创建 ${UNINSTALL_LOG}。"


;--------------------------------
; Text for page with release notes

!insertmacro ReadmeLangStrings "" "${LANG_SIMPCHINESE}" \
    "发行说明" \
    "请仔细阅读以下重要信息。" \
    "关于 $(^name)：" \
    "$\n  在滚动箭头或按点击下一页审查整个案文。"


;--------------------------------
; Components Page

LangString ALLUSERS_SECTITLE ${LANG_SIMPCHINESE} "为所有用户安装"
LangString STARTMENU_SECTITLE ${LANG_SIMPCHINESE} "开始菜单快捷方式"
LangString DESKTOP_SECTITLE ${LANG_SIMPCHINESE} "桌面快捷方式"
LangString QUICKLAUNCH_SECTITLE ${LANG_SIMPCHINESE} "快速启动图标"

