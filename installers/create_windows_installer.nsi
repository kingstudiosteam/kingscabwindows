# The King's Cab - Windows Installer Script (NSIS)
# Creates professional installer for VST3 and AAX formats
# Compatible with Pro Tools, Reaper, Cubase, Studio One, and all major DAWs

!define PLUGIN_NAME "The King's Cab"
!define COMPANY_NAME "King Studios"
!define VERSION "1.0.0"
!define PUBLISHER "King Studios"
!define WEBSITE_URL "https://www.kingstudiospa.com"

# Modern UI
!include "MUI2.nsh"
!include "x64.nsh"

# General configuration
Name "${PLUGIN_NAME}"
OutFile "TheKingsCab-${VERSION}-Windows.exe"
Unicode True
RequestExecutionLevel admin

# Default installation directory
InstallDir "$PROGRAMFILES64\King Studios\${PLUGIN_NAME}"

# Variables
Var VST3_DIR
Var AAX_DIR

# Modern UI Configuration
!define MUI_ABORTWARNING
!define MUI_ICON "icon.ico"
!define MUI_WELCOMEPAGE_TITLE "Welcome to ${PLUGIN_NAME} Setup"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of ${PLUGIN_NAME}, a professional impulse response loader.$\r$\n$\r$\nCompatible with Pro Tools, Reaper, Cubase, Studio One, and all major DAWs supporting VST3 format.$\r$\n$\r$\nClick Next to continue."

!define MUI_FINISHPAGE_TITLE "Installation Complete"
!define MUI_FINISHPAGE_TEXT "${PLUGIN_NAME} has been successfully installed.$\r$\n$\r$\nPlugin locations:$\r$\n• VST3: $VST3_DIR$\r$\n• AAX: $AAX_DIR$\r$\n$\r$\nRestart your DAW to load the plugin.$\r$\n$\r$\nVisit ${WEBSITE_URL}/store for premium IR collections."

# Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Languages
!insertmacro MUI_LANGUAGE "English"

# Version information
VIProductVersion "${VERSION}.0"
VIAddVersionKey "ProductName" "${PLUGIN_NAME}"
VIAddVersionKey "Comments" "Professional impulse response loader with 6-slot cabinet simulation"
VIAddVersionKey "CompanyName" "${COMPANY_NAME}"
VIAddVersionKey "LegalCopyright" "© ${COMPANY_NAME}"
VIAddVersionKey "FileDescription" "${PLUGIN_NAME} Installer"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "ProductVersion" "${VERSION}"

# Installer sections
Section "Core Plugin Files" SecCore
    SectionIn RO  # Required section
    
    # Set output path to installation directory
    SetOutPath $INSTDIR
    
    # Determine plugin directories
    ${If} ${RunningX64}
        StrCpy $VST3_DIR "$PROGRAMFILES64\Common Files\VST3"
        StrCpy $AAX_DIR "$PROGRAMFILES64\Common Files\Avid\Audio\Plug-Ins"
    ${Else}
        StrCpy $VST3_DIR "$PROGRAMFILES32\Common Files\VST3"
        StrCpy $AAX_DIR "$PROGRAMFILES32\Common Files\Avid\Audio\Plug-Ins"
    ${EndIf}
    
    # Create directories
    CreateDirectory "$VST3_DIR"
    CreateDirectory "$AAX_DIR"
    
    # Copy VST3 plugin
    DetailPrint "Installing VST3 plugin..."
    SetOutPath "$VST3_DIR"
    File /r "windows_build\VST3\${PLUGIN_NAME}.vst3"
    
    # Copy AAX plugin (if available)
    IfFileExists "windows_build\AAX\${PLUGIN_NAME}.aaxplugin" 0 +3
    DetailPrint "Installing AAX plugin..."
    SetOutPath "$AAX_DIR"
    File /r "windows_build\AAX\${PLUGIN_NAME}.aaxplugin"
    
    # Create application data directory
    CreateDirectory "$APPDATA\King Studios\${PLUGIN_NAME}"
    
    # Write installation info
    SetOutPath $INSTDIR
    WriteUninstaller "uninstall.exe"
    
    # Registry entries for Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "DisplayName" "${PLUGIN_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "Publisher" "${PUBLISHER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "URLInfoAbout" "${WEBSITE_URL}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "NoRepair" 1
    
    # Create start menu shortcuts
    CreateDirectory "$SMPROGRAMS\King Studios"
    CreateShortcut "$SMPROGRAMS\King Studios\Uninstall ${PLUGIN_NAME}.lnk" "$INSTDIR\uninstall.exe"
    
SectionEnd

# Uninstaller section
Section "Uninstall"
    
    # Remove plugin files
    RMDir /r "$VST3_DIR\${PLUGIN_NAME}.vst3"
    RMDir /r "$AAX_DIR\${PLUGIN_NAME}.aaxplugin"
    
    # Remove installation directory
    RMDir /r $INSTDIR
    
    # Remove application data
    RMDir /r "$APPDATA\King Studios\${PLUGIN_NAME}"
    
    # Remove start menu items
    Delete "$SMPROGRAMS\King Studios\Uninstall ${PLUGIN_NAME}.lnk"
    RMDir "$SMPROGRAMS\King Studios"
    
    # Remove registry entries
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}"
    
SectionEnd

# Functions
Function .onInit
    # Check for 64-bit system
    ${IfNot} ${RunningX64}
        MessageBox MB_OK|MB_ICONSTOP "This plugin requires a 64-bit version of Windows."
        Abort
    ${EndIf}
    
    # Check for existing installation
    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PLUGIN_NAME}" "UninstallString"
    StrCmp $R0 "" done
    
    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
        "${PLUGIN_NAME} is already installed. $\n$\nClick 'OK' to remove the previous version or 'Cancel' to cancel this upgrade." \
        IDOK uninst
    Abort
    
    uninst:
        ClearErrors
        ExecWait '$R0 _?=$INSTDIR'
        
        IfErrors no_remove_uninstaller done
        no_remove_uninstaller:
    
    done:
FunctionEnd

# Section descriptions
LangString DESC_SecCore ${LANG_ENGLISH} "Core plugin files (VST3 and AAX formats)"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(DESC_SecCore)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
