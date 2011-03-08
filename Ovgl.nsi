;--------------------------------
; Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
; General

  ; Name and file
  Name "Ovgl"
  OutFile "Ovgl-0.25a.exe"

  ; The default installation directory
  InstallDir "$PROGRAMFILES\Ovgl"

  ; Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\Ovgl" "Install_Dir"

  ; Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
; Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
; Pages

  !insertmacro MUI_PAGE_LICENSE "LICENSE"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
; The stuff to install
Section "Ovgl Core (required)"

  SectionIn RO
  
  ; Set output path to the installation directory
  SetOutPath $INSTDIR

  ; Create directories
  CreateDirectory "$INSTDIR\include"
  CreateDirectory "$INSTDIR\lib"
  CreateDirectory "$INSTDIR\bin"

  ; Put file there
  File /oname=lib\Ovgl.dll lib\Ovgl.dll
  File /oname=lib\Ovgl.lib lib\Ovgl.lib
  File /oname=lib\Ovgl_d.dll lib\Ovgl_d.dll
  File /oname=lib\Ovgl_d.lib lib\Ovgl_d.lib
  File /oname=lib\fbxsdk_20113_1.dll lib\fbxsdk_20113_1.dll
  File /oname=lib\fbxsdk_20113_1d.dll lib\fbxsdk_20113_1d.dll
  File /oname=lib\NxCharacter.dll lib\NxCharacter.dll
  File /r /x .svn /x *.cpp ovgl
  File /r /x .svn docs
  File /r /x .svn data
  File /r /x .svn examples
  File /r redist

  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\Ovgl" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ovgl" "DisplayName" "Ovgl"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ovgl" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ovgl" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ovgl" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Ovgl"
  CreateShortCut "$SMPROGRAMS\Ovgl\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Ovgl\Samples\Ovgl_FPS.lnk" "$INSTDIR\bin\Ovgl_FPS.exe" "" "$INSTDIR\bin\Ovgl_FPS.exe" 0

SectionEnd

Section 'DirectX'
  ExecWait "$INSTDIR\Redist\dxwebsetup.exe /q"
SectionEnd

Section 'PhysX'
  ExecWait "$INSTDIR\Redist\PhysX_9.09.0814_SystemSoftware.exe /quiet"
SectionEnd

Section 'Microsoft Visual C++ 2008 Redistributable Package (x86)'
  ExecWait "$INSTDIR\Redist\vcredist_x86.exe /q"
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ovgl"
  DeleteRegKey HKLM "SOFTWARE\Ovgl"

  ; Remove files and uninstaller
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\samples"
  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\include"
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\bin"
  Delete "$INSTDIR\uninstall.exe"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Ovgl\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Ovgl"
  RMDir "$INSTDIR"

SectionEnd

Function .onInstSuccess
  RMDir /r "$INSTDIR\redist"
FunctionEnd

Function .onInstFailed
  MessageBox MB_OK "Setup has failed to install the application."
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ovgl"
  DeleteRegKey HKLM "SOFTWARE\Ovgl"

  ; Remove files and uninstaller
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\samples"
  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\include"
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\bin"
  RMDir /r "$INSTDIR\redist"
  Delete "$INSTDIR\uninstall.exe"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Ovgl\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Ovgl"
  RMDir "$INSTDIR"
FunctionEnd