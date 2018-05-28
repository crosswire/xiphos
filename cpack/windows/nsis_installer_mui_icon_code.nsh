#
#
######################################
#
# CPACK_NSIS_INSTALLER_MUI_ICON_CODE
#
#######################################
#
#

 ; Re-create settings defined by the original code
 !define MUI_ICON "..\..\..\..\win32\nsis\pixmaps/icon-install.ico"
 !define MUI_UNICON "..\..\..\..\win32\nsis\pixmaps\icon-uninstall.ico"

 ; Our own settings:

 ; Add RTL header image
 !define MUI_UI_HEADERIMAGE_RTL "..\..\..\..\win32\nsis\pixmaps\header-r.bmp"

 ;
 !define INSTPATH_SWORD "Sword"

 ; REG keys to handle url  sword://  by Xiphos
 !define SWURL_REG_ROOT "HKCR" # HKEY_CLASSES_ROOT
 !define SWURL_REG_KEY "sword"
 !define SWURL_REG_KEY_ICON "${SWURL_REG_KEY}\DefaultIcon"
 !define SWURL_REG_KEY_COMMAND "${SWURL_REG_KEY}\shell\open\command"


 ; Include Macro for registering fonts
 !define INCL_DIR "..\..\..\..\win32\nsis\include"
 !define PATH_FONT "..\..\..\..\win32\fonts"
 !include "${INCL_DIR}\fontreg.nsh"

#
#
######################################

