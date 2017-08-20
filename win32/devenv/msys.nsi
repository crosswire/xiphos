; Xiphos Bible Study Tool
; msys.nsi
;
; Copyright (C) 2010-2017 Xiphos Developer Team
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
; Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 
 
; Install and Setup Windows Development Environment for Xiphos

; To create installer for Xiphos development environment you need
; Unicode NSIS (Nullsoft Scriptable Install System).
; http://www.scratchpaper.com/


;--------------------------------
; Script definitions

    !define APP_NAME "Xiphos Development Environment"
    !define INSTALLER_NAME "xiphos_devenv"
    !define APP_BINARY_NAME "xiphos.exe"
    !define APP_VERS "1.3"
    !define APP_EDITION "win32"
    !define APP_URL "http://xiphos.org"
    !define MSYS_NAME "msys"
    !define SHORTCUT_NAME "Xiphos Devel"

    !define UNINST_EXE "uninst-xiphos_devenv.exe"
    !define BAT_NAME "sh.bat"
    !define FSTAB_NAME "etc\fstab"

    ; Paths with application files for installer
    !define PATH_CORE "..\binaries\devenv"
    !define PATH_IMG "../nsis/pixmaps"
    !define PATH_MSYS "${PATH_CORE}\${MSYS_NAME}"

    ; Installers
    !define INST_PYTHON "python-2.6.2.msi"
    !define INST_LIBXML2 "libxml2-python-2.7.3.win32-py2.6.exe"
    !define INST_PERL "strawberry-perl-5.8.9.2.msi"
    !define INST_NSISU "nsis-2.45.1-Unicode-setup.exe"
    !define INST_MSHELP "htmlhelp.exe"
    !define INST_BZR "bzr-2.0.3-2-setup.exe"

    ; Files
    !define FILES_MSYS "${PATH_MSYS}\*.*"
    #!define FILES_SH "${PATH_CORE}\sh.bat"
    !define FILES_PYTHON "${PATH_CORE}\${INST_PYTHON}"
    !define FILES_LIBXML2 "${PATH_CORE}\${INST_LIBXML2}"
    !define FILES_PERL "${PATH_CORE}\${INST_PERL}"
    !define FILES_NSISU "${PATH_CORE}\${INST_NSISU}"
    !define FILES_BZR "${PATH_CORE}\${INST_BZR}"
    !define URL_MSHELP "http://go.microsoft.com/fwlink/?LinkId=14188"

    ; Folders with binaries for PATH
    !define BIN_MINGW "mingw\bin"
    !define BIN_MSYS "bin"
    !define BIN_LOCAL_MSYS "local\bin"
    !define BIN_PYTHON "C:\Python26"
    !define BIN_PERL "C:\strawberry\perl\bin"
    !define BIN_NSISU "$PROGRAMFILES\NSIS\Unicode"
    !define BIN_MSHELP "$PROGRAMFILES\HTML Help Workshop"

    ; Following two definitions required by Uninstall log.
    !define INSTDIR_REG_ROOT "HKLM" # HKEY_LOCAL_MACHINE
    !define INSTDIR_REG_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

    !define STM_REG_ROOT "HKCU" # HKEY_CURRENT_USER
    !define STM_REG_KEY "Software\${APP_NAME}"
    !define STM_REG_VAL "${APP_NAME}"


;--------------------------------
; Compression method

    SetCompressor /SOLID lzma
    SetCompressorDictSize 16 ; in MB, default 8

;--------------------------------
; Includes

    ; Modern UI
    !include "MUI2.nsh"

    !include "nsDialogs.nsh"
    !include "EnvVarUpdate.nsh"
    !include "WriteToFile.nsh"

    ;!include "LogicLib.nsh"
    ;!include "TextFunc.nsh"

    ; Win32 API Messages
    ;!include "WinMessages.nsh"


;--------------------------------
; General

    Name "${APP_NAME}"
    OutFile "${INSTALLER_NAME}-${APP_VERS}-${APP_EDITION}.exe"
    ShowInstDetails hide
    ShowUninstDetails hide
    InstallDir "C:\${MSYS_NAME}"
    InstallDirRegKey ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir"
    CRCCheck force
    SetPluginUnload alwaysoff
    XPStyle on

    ; Request admin privileges in Windows Vista and maybe in other versions
    RequestExecutionLevel admin

;--------------------------------
; Interface Settings

    !define MUI_ABORTWARNING

    ; Look & Feel
    !define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
    !define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
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

;--------------------------------
; Pages

    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_COMPONENTS
    !insertmacro MUI_PAGE_DIRECTORY
    !insertmacro MUI_PAGE_INSTFILES

    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages

    !insertmacro MUI_LANGUAGE "English"
    
;--------------------------------
; Reserve Files

    ; If you are using solid compression, files that are required before
    ; the actual installation should be stored first in the data block
    ; because this will make your installer start faster.
    
    ReserveFile "${NSISDIR}\Plugins\System.dll"
    ReserveFile "${NSISDIR}\Plugins\nsDialogs.dll"

    ; plugin for downloading files from the Internet (used for MS Help Compil.)
    ReserveFile "${NSISDIR}\Plugins\NSISdl.dll"

    ReserveFile "${NSISDIR}\Include\nsDialogs.nsh"

    ;ReserveFile "${NSISDIR}\Include\LogicLib.nsh"
    ;ReserveFile "${NSISDIR}\Include\TextFunc.nsh"
    ;ReserveFile "${NSISDIR}\Include\WinMessages.nsh"

    ReserveFile "EnvVarUpdate.nsh"

;--------------------------------
; Core Section

Section "msys" SecCore
    SetShellVarContext all 

    SetOutPath '$INSTDIR'
    File /r '${FILES_MSYS}'
    #File '${FILES_SH}'

    # write command to execute bash in windows prompt
    ${WriteToFile} "$INSTDIR\bin\sh --login$\r$\n" "$INSTDIR\${BAT_NAME}"

    # write right MINGW path to /etc/fstab for mounting MINGW to '/mingw'
    ${WriteToFile} "$INSTDIR\mingw /mingw$\n" "$INSTDIR\${FSTAB_NAME}"

    ; Add binaries to system PATH
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\${BIN_MINGW}"
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\${BIN_MSYS}"
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\${BIN_LOCAL_MSYS}"
    

    ; Add uninstall information to Add/Remove Programs
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "InstallDir" "$INSTDIR"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "DisplayName" "${APP_NAME}"
    WriteRegStr ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}" "UninstallString" "$INSTDIR\${UNINST_EXE}"

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\${UNINST_EXE}"

SectionEnd

Section "Desktop Shortcut" SecDesktop
    SetShellVarContext all 
    CreateShortCut '$DESKTOP\${SHORTCUT_NAME}.lnk' '$INSTDIR\${BAT_NAME}'
SectionEnd

;--------------------------------
; Install 3rd party installers
; installers will be unpacked to TMP folder and then executed
; Installation of other installers will be silent (default options)
; system PATH is also set

Section "Python 2.6.2" Sec01
    SetOutPath '$TEMP'
    File '${FILES_PYTHON}'
    ; MSI installer
    ExecWait '"msiexec" /i "$TEMP\${INST_PYTHON}" /QB- /passive  ALLUSERS=1'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "${BIN_PYTHON}"
SectionEnd

Section "libxml2 2.7.3 (bindings for Python)" Sec02
    SetOutPath '$TEMP'
    File '${FILES_LIBXML2}'
    ; FIXME unknown installer type, SILENT installation doesn't work
    ExecWait '"$TEMP\${INST_LIBXML2}" ALLUSERS=1'
SectionEnd

Section "Perl 5.8.9.2 (strawberry)" Sec03
    SetOutPath '$TEMP'
    File '${FILES_PERL}'
    ; MSI installer
    ExecWait '"msiexec" /i "$TEMP\${INST_PERL}" /QB- /passive  ALLUSERS=1'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "${BIN_PERL}"
SectionEnd

Section "NSIS 2.45.1 (for creating installers)" Sec04
    SetOutPath '$TEMP'
    File '${FILES_NSISU}'
    ; NSIS installer
    ExecWait '"$TEMP\${INST_NSISU}" /S ALLUSERS=1'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "${BIN_NSISU}"
SectionEnd

Section "Bazaar 2.0.3 (version control system)" Sec05
    SetOutPath '$TEMP'
    File '${FILES_BZR}'
    ; Inno Setup installer
    ExecWait '"$TEMP\${INST_BZR}" /SILENT ALLUSERS=1'
SectionEnd

Section "MS Help Compiler (EULA will be accepted)" Sec06
    ; download installer to TMP folder
    ; It is not legal to include MS Help Compiler directly to installer
    NSISdl::download '${URL_MSHELP}' '$TEMP\${INST_MSHELP}' 
    ; works cli option '/?' - it says use '/Q' for silent instalation
    ExecWait '"$TEMP\${INST_MSHELP}" /Q'
    ; hhc command is necessary
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "${BIN_MSHELP}"
SectionEnd

;--------------------------------
; Uninstaller Part

Section Uninstall

    SetShellVarContext all

    ; Delete files recursively without asking!
    RMDir /r "$INSTDIR\*.*"
    Delete "$INSTDIR\${BAT_NAME}"

    ; remove desktop icon
    Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"

    ; paths from system PATH
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\${BIN_MINGW}"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\${BIN_MSYS}"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "${BIN_PYTHON}"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "${BIN_PERL}"
    ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "${BIN_NSISU}"

    ; clean Windows Registry
    
    ; delete from Add/Remove Programs
    DeleteRegKey /ifempty ${INSTDIR_REG_ROOT} "${INSTDIR_REG_KEY}"

SectionEnd

