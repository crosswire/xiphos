# Register font filename with its name in
# Windows registry

# code taken from:
#
# http://nsis.sourceforge.net/Register_Fonts

# Changes to original code:
# - removed macro 'InstallTTFFont' because it required a binary file
# - macro 'InstallFONFont' renamed to 'InstallTTFFont'

var FONT_DIR
 
!ifndef CSIDL_FONTS
  !define CSIDL_FONTS '0x14' ;Fonts directory path constant
!endif
!ifndef CSIDL_FLAG_CREATE
  !define CSIDL_FLAG_CREATE 0x8000
!endif
 
### Modified Code from FileFunc.nsh    ###
### Original by Instructor and kichik  ###
 
!ifmacrondef GetFileNameCall
        !macro GetFileNameCall _PATHSTRING _RESULT
                Push `${_PATHSTRING}`
              	Call GetFileName
               	Pop ${_RESULT}
        !macroend
!endif
 
!ifndef GetFileName
	!define GetFileName `!insertmacro GetFileNameCall`
 
	Function GetFileName
		Exch $0
		Push $1
		Push $2
 
		StrCpy $2 $0 1 -1
		StrCmp $2 '\' 0 +3
		StrCpy $0 $0 -1
		goto -3
 
		StrCpy $1 0
		IntOp $1 $1 - 1
		StrCpy $2 $0 1 $1
		StrCmp $2 '' end
		StrCmp $2 '\' 0 -3
		IntOp $1 $1 + 1
		StrCpy $0 $0 '' $1
 
		end:
		Pop $2
		Pop $1
		Exch $0
	FunctionEnd
!endif
 
### End Code From ###
 
 
!macro InstallTTFFont FontFile FontName
  Push $0  
  Push $R0
  Push $R1
 
  !define Index 'Line${__LINE__}'
 
; Get the Font's File name
  ${GetFileName} ${FontFile} $0
  !define FontFileName $0
 
  SetOutPath $FONT_DIR
  IfFileExists "$FONT_DIR\${FontFileName}" ${Index}
    File '${FontFile}'
 
${Index}:
  ClearErrors
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
  IfErrors "${Index}-9x" "${Index}-NT"
 
"${Index}-NT:"
  StrCpy $R1 "Software\Microsoft\Windows NT\CurrentVersion\Fonts"
  goto "${Index}-GO"
 
"${Index}-9x:"
  StrCpy $R1 "Software\Microsoft\Windows\CurrentVersion\Fonts"
  goto "${Index}-GO"
 
"${Index}-GO:"
  ClearErrors
  ReadRegStr $R0 HKLM "$R1" "${FontName}"
  IfErrors 0 "${Index}-End"
    System::Call "GDI32::AddFontResourceW(t) i ('${FontFileName}') .s"
    WriteRegStr HKLM "$R1" "${FontName}" "${FontFileName}"
    goto "${Index}-End"
 
"${Index}-End:"
 
  !undef Index
  !undef FontFileName
 
  pop $R1
  Pop $R0  
  Pop $0
!macroend
 
 
; Uninstaller entries
 
 
!macro RemoveOTFFont FontFile FontName
  Push $0  
  Push $R0
  Push $R1
 
  !define Index 'Line${__LINE__}'
 
; Get the Font's File name
  ${GetFileName} ${FontFile} $0
  !define FontFileName $0
 
  SetOutPath $FONT_DIR
  IfFileExists "$FONT_DIR\${FontFileName}" "${Index}" "${Index}-End"
 
${Index}:
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
  IfErrors "${Index}-9x" "${Index}-NT"
 
"${Index}-NT:"
  StrCpy $R1 "Software\Microsoft\Windows NT\CurrentVersion\Fonts"
  goto "${Index}-GO"
 
"${Index}-9x:"
  StrCpy $R1 "Software\Microsoft\Windows\CurrentVersion\Fonts"
  goto "${Index}-GO"
 
"${Index}-GO:"
  System::Call "GDI32::RemoveFontResourceW(t) i ('${FontFileName}') .s"
  DeleteRegValue HKLM "$R1" "${FontName}"
  delete /REBOOTOK "$FONT_DIR\${FontFileName}"
  goto "${Index}-End"
 
"${Index}-End:"
 
  !undef Index
  !undef FontFileName
 
  pop $R1
  Pop $R0  
  Pop $0
!macroend
