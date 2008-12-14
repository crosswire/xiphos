; GnomeSword Bible Study Tool
; installer.nsi
;
; Copyright (C) 2008 GnomeSword Developer Team
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU Library General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
 
; Script for Windows installer

; To create windows installer you need
; Unicode NSIS (Nullsoft Scriptable Install System).
; http://www.scratchpaper.com/

;--------------------------------
; Unicode NSIS note:

    ; every source file for Unicode NSIS needs to be in UTF-16LE
    ; doesn't support win9x

;--------------------------------
; Localization note:

    ; Localized variables must be used with parentheses.
    ; Example: $(SOME_DESCRIPTION)


;--------------------------------
; Script definitions

    !define APP_NAME "GnomeSword"
    !define APP_BINARY_NAME "gnomesword.exe"
    !define APP_VERS "3.0"
    !define APP_EDITION "win32-test05"
    !define APP_URL "http://gnomesword.sf.net"

    ; Paths with application files for installer
    !define PATH_CORE "..\gs_bin"
    !define PATH_IMG "img"

    ; Files
    !define CORE_F "${PATH_CORE}\*.*"

    ; Following two definitions required by Uninstall log.
    !define INSTDIR_REG_ROOT "HKLM" # HKEY_LOCAL_MACHINE
    !define INSTDIR_REG_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

    !define STM_REG_ROOT "HKCU" # HKEY_CURRENT_USER
    !define STM_REG_KEY "Software\${APP_NAME}"
    !define STM_REG_VAL "${APP_NAME}"

    ; Remember choosen language for installation
    !define LANG_REG_ROOT "HKCU" # HKEY_CURRENT_USER
    !define LANG_REG_KEY "Software\${APP_NAME}"
    !define LANG_REG_VAL "Installer Language"

    !define INCL_DIR "include"

    ; REG keys to handle url  sword://  by GnomeSword
    !define SWURL_REG_ROOT "HKCR" # HKEY_CLASSES_ROOT
    !define SWURL_REG_KEY "sword"
    !define SWURL_REG_KEY_ICON "${SWURL_REG_KEY}\DefaultIcon"
    !define SWURL_REG_KEY_COMMAND "${SWURL_REG_KEY}\shell\open\command"


;--------------------------------
; Compression method

    SetCompressor /SOLID lzma
    SetCompressorDictSize 16 ; in MB, default 8

;--------------------------------
; Includes

    ; Modern UI
    !include "MUI2.nsh"

    ; Uninstaller Log
    !include "${INCL_DIR}\utf16_advuninstlog.nsh"

    !include "nsDialogs.nsh"
    !include "LogicLib.nsh"
    !include "TextFunc.nsh"

    ; Win32 API Messages
    !include "WinMessages.nsh"

    ; Language definition
    ; contain macros LINGUAS_DEFS, LINGUAS_RESERVE, LINGUAS_USE,
    ; LINGUAS_LIC_FILE
    !include "i18n\utf16_LINGUAS.nsh" ; contain some macros

;--------------------------------
; Global Variables

    Var StartMenuDir

;--------------------------------
; General

    Name "${APP_NAME}"
    OutFile "${APP_NAME}-${APP_VERS}-${APP_EDITION}.exe"
    ShowInstDetails hide
    ShowUninstDetails hide
    InstallDir "$PROGRAMFILES\CrossWire\${APP_NAME}"
    InstallDirRegKey ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir"
    CRCCheck force
    SetPluginUnload alwaysoff
    XPStyle on

    ; Request admin privileges in Windows Vista and maybe in other versions
    RequestExecutionLevel admin

;--------------------------------
; Interface Settings

    !define MUI_ABORTWARNING
    !define MUI_LANGDLL_ALWAYSSHOW

    ; Look & Feel
    !define MUI_ICON "${PATH_IMG}\icon-install.ico"
    !define MUI_UNICON "${PATH_IMG}\icon-uninstall.ico"
    !define MUI_WELCOMEFINISHPAGE_BITMAP "${PATH_IMG}\wizard.bmp"
    !define MUI_UNWELCOMEFINISHPAGE_BITMAP "${PATH_IMG}\wizard-uninstall.bmp"

    !define MUI_HEADERIMAGE
    !define MUI_HEADERIMAGE_BITMAP "${PATH_IMG}\header.bmp"
    !define MUI_HEADERIMAGE_UNBITMAP "${PATH_IMG}\header-uninstall.bmp"

    !define MUI_HEADERIMAGE_BITMAP_RTL "${PATH_IMG}\header-r.bmp"
    !define MUI_HEADERIMAGE_UNBITMAP_RTL "${PATH_IMG}\header-uninstall-r.bmp"

    ;Start Menu Folder Page Configuration
    !define MUI_STARTMENUPAGE_REGISTRY_ROOT "${STM_REG_ROOT}" 
    !define MUI_STARTMENUPAGE_REGISTRY_KEY "${STM_REG_KEY}" 
    !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${STM_REG_VAL}"

    ; The preferred uninstaller operation mode
    !insertmacro UNATTENDED_UNINSTALL

    !define MUI_FINISHPAGE_LINK "${APP_URL}"
    !define MUI_FINISHPAGE_LINK_LOCATION "${APP_URL}"

;--------------------------------
; Language Selection Dialog Settings

    ; Remember the installer language
    !define MUI_LANGDLL_REGISTRY_ROOT "${LANG_REG_ROOT}"
    !define MUI_LANGDLL_REGISTRY_KEY "${LANG_REG_KEY}"
    !define MUI_LANGDLL_REGISTRY_VALUENAME "${LANG_REG_VAL}"

;--------------------------------
; Pages

    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_LICENSE \
        $(LICENSE_FILE) ; defined in language file
    ;!insertmacro MUI_PAGE_COMPONENTS
    !insertmacro MUI_PAGE_DIRECTORY
    !insertmacro MUI_PAGE_INSTFILES

    !insertmacro MUI_PAGE_STARTMENU Application $StartMenuDir

    #Page custom basicInfo ; Basic instructions for OpenSong

    !insertmacro MUI_PAGE_FINISH

    !insertmacro MUI_UNPAGE_WELCOME
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES
    !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Languages

    !define UNINSTALLOG_LOCALIZE 

    !insertmacro LINGUAS_USE

    ; Include localizations
    !insertmacro LINGUAS_INCLUDE
    !insertmacro LINGUAS_LIC_FILE

;--------------------------------
; Reserve Files

    ; If you are using solid compression, files that are required before
    ; the actual installation should be stored first in the data block
    ; because this will make your installer start faster.

    !insertmacro MUI_RESERVEFILE_LANGDLL
    ReserveFile "${NSISDIR}\Plugins\System.dll"
    ReserveFile "${NSISDIR}\Plugins\nsDialogs.dll"
    ReserveFile "${INCL_DIR}\utf16_advuninstlog.nsh"

    !insertmacro LINGUAS_RESERVEFILE

;--------------------------------
; Core Section

; Section is like one component
Section $(CORE_SEC_TITLE) SecCore

    ; shortcuts will be installed for all users - Desktop/Startmenu
    SetShellVarContext all

    SetOutPath '$INSTDIR'

    ; Log installed files

    !insertmacro UNINSTALL.LOG_OPEN_INSTALL
        File /r "${CORE_F}"
    !insertmacro UNINSTALL.LOG_CLOSE_INSTALL

    !define STM_DIR "$SMPROGRAMS\$StartMenuDir"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        ;Create shortcuts
        CreateDirectory '${STM_DIR}'
        CreateShortCut '${STM_DIR}\${APP_NAME}.lnk' '$INSTDIR\bin\${APP_BINARY_NAME}'
        CreateShortCut '${STM_DIR}\Uninstall.lnk' '${UNINST_EXE}'
    !insertmacro MUI_STARTMENU_WRITE_END

    ; Desktop shortcuts
    CreateShortCut '$DESKTOP\${APP_NAME}.lnk' '$INSTDIR\bin\${APP_BINARY_NAME}'

    ; Add uninstall information to Add/Remove Programs
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir" "$INSTDIR"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "DisplayName" "${APP_NAME}"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "UninstallString" "${UNINST_EXE}"

    ; GnomeSword as default handler of url  sword://
    ;WriteRegStr HKCR "sword" "" "URL:sword Protocol"
    ;WriteRegStr HKCR "sword" "URL Protocol" ""
    ;WriteRegStr HKCR "sword\DefaultIcon" "" \
        ;'"$INSTDIR\bin\${APP_BINARY_NAME}"'
    ;WriteRegStr HKCR "sword\shell\open\command" "" \
        ;'"$INSTDIR\bin\${APP_BINARY_NAME}" "%1"'

    WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY} "" "URL:sword Protocol"
    WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY} "URL Protocol" ""
    WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY_ICON} "" \
        '"$INSTDIR\bin\${APP_BINARY_NAME}"'
    WriteRegStr ${SWURL_REG_ROOT} ${SWURL_REG_KEY_COMMAND} "" \
        '"$INSTDIR\bin\${APP_BINARY_NAME}" "%1"'

    ;Create uninstaller
    WriteUninstaller "${UNINST_EXE}"

SectionEnd

;--------------------------------
; Installer Functions

Function .onInit

    !insertmacro UNINSTALL.LOG_PREPARE_INSTALL
    !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

Function .onInstSuccess

         ;create/update log always within .onInstSuccess function
         !insertmacro UNINSTALL.LOG_UPDATE_INSTALL

FunctionEnd

;--------------------------------
; Section Descriptions

    ;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
        ;!insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(CORE_SEC_DESCRIPTION)
        ;!insertmacro MUI_DESCRIPTION_TEXT ${SecCompname} $(COMPNAME_SEC_DESCRIPTION)
    ;!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
; Uninstaller Part

Section Uninstall

    ; Hack to speed up uninstaller
    RMDir /r "$INSTDIR\bin\*.*"
    RMDir /r "$INSTDIR\etc\*.*"
    RMDir /r "$INSTDIR\lib\*.*"
    RMDir /r "$INSTDIR\libexec\*.*"
    RMDir /r "$INSTDIR\share\*.*"

    ;uninstall from path, must be repeated for every install
    ;logged path individual
    !insertmacro UNINSTALL.LOG_UNINSTALL "$INSTDIR"

    ;end uninstall, after uninstall from all logged paths has been performed
    !insertmacro UNINSTALL.LOG_END_UNINSTALL

    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuDir
    !define ST_MENU "$SMPROGRAMS\$StartMenuDir"

    Delete "${ST_MENU}\${APP_NAME}.lnk"
    Delete "${ST_MENU}\Uninstall.lnk"
    RMDir "${ST_MENU}"
    Delete "$DESKTOP\${APP_NAME}.lnk"

    ; clean Windows Registry
    
    ; delete installer language
    DeleteRegKey ${LANG_REG_ROOT} "${LANG_REG_KEY}"

    ; delete from Add/Remove Programs
    DeleteRegKey /ifempty ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}"

    ; delete handler of url  sword://
    DeleteRegKey ${SWURL_REG_ROOT} ${SWURL_REG_KEY}

SectionEnd

;--------------------------------
; Uninstaller Function

Function un.onInit

    !insertmacro MUI_UNGETLANGUAGE

    ;begin uninstall, could be added on top of uninstall section instead
    !insertmacro UNINSTALL.LOG_BEGIN_UNINSTALL

FunctionEnd

