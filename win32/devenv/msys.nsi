
!define APP_NAME "Xiphos Development Environment"
!define INSTALLER_NAME "xiphos-devel"
!define APP_VERS "1.0"

SetCompressor /SOLID lzma
SetCompressorDictSize 16

!include "MUI2.nsh"

!include "nsDialogs.nsh"
!include "EnvVarUpdate.nsh"

Name "${APP_NAME}"
OutFile "${INSTALLER_NAME}-${APP_VERS}"
ShowInstDetails hide
InstallDir "C:\"
XPStyle on

RequestExecutionLevel admin

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES



Section "Core" SecCore
	SetOutPath '$INSTDIR'
	File /r msys
	File /r strawberry
	File /r Python26
	File sh.bat
${EnvVarUpdate} $0 "PATH" "A" "HKLM" "C:\msys\mingw\bin"
${EnvVarUpdate} $0 "PATH" "A" "HKLM" "C:\msys\bin"
${EnvVarUpdate} $0 "PATH" "A" "HKLM" "C:\strawberry\perl\bin"
${EnvVarUpdate} $0 "PATH" "A" "HKLM" "C:\Python26"
SectionEnd

Section "Desktop" SecDesktop
	CreateShortCut '$DESKTOP\Xiphos Devel.lnk' '$INSTDIR\sh.bat'
SectionEnd


	