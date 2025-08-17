@echo off
title The King's Cab v1.0.0 - Professional IR Loader Installation
color 0A

cls
echo.
echo     ████████╗██╗  ██╗███████╗    ██╗  ██╗██╗███╗   ██╗ ██████╗ ███████╗
echo     ╚══██╔══╝██║  ██║██╔════╝    ██║ ██╔╝██║████╗  ██║██╔════╝ ██╔════╝
echo        ██║   ███████║█████╗      █████╔╝ ██║██╔██╗ ██║██║  ███╗███████╗
echo        ██║   ██╔══██║██╔══╝      ██╔═██╗ ██║██║╚██╗██║██║   ██║╚════██║
echo        ██║   ██║  ██║███████╗    ██║  ██╗██║██║ ╚████║╚██████╔╝███████║
echo        ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝
echo.
echo                           ██████╗ █████╗ ██████╗ 
echo                          ██╔════╝██╔══██╗██╔══██╗
echo                          ██║     ███████║██████╔╝
echo                          ██║     ██╔══██║██╔══██╗
echo                          ╚██████╗██║  ██║██████╔╝
echo                           ╚═════╝╚═╝  ╚═╝╚═════╝ 
echo.
echo ===============================================================================
echo                     PROFESSIONAL IMPULSE RESPONSE LOADER
echo                              Version 1.0.0
echo                             By King Studios
echo ===============================================================================
echo.
echo Compatible with:
echo   ✓ Pro Tools (AAX Plugin)
echo   ✓ Reaper, Cubase, Studio One (VST3 Plugin)  
echo   ✓ All major 64-bit DAWs
echo.
echo Installation includes:
echo   ✓ Professional plugin binaries
echo   ✓ Complete IR collection (500+ impulse responses)
echo   ✓ Vintage Neve/API style interface
echo   ✓ 6-slot cabinet simulation system
echo.
echo SYSTEM REQUIREMENTS:
echo   • Windows 10 (64-bit) or later
echo   • Administrator privileges
echo   • 50MB free disk space
echo.
echo ===============================================================================
echo.
set /p confirm="Press ENTER to begin installation or CTRL+C to cancel: "

echo.
echo Checking system requirements...
timeout /t 1 /nobreak >nul

REM Check for 64-bit Windows
if not "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    echo ❌ ERROR: This plugin requires 64-bit Windows
    pause
    exit /b 1
)
echo ✓ 64-bit Windows detected

REM Check for administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo.
    echo ⚠️  Administrator privileges required!
    echo    Please run this installer as Administrator.
    echo.
    echo    Right-click this file and select "Run as administrator"
    echo.
    pause
    exit /b 1
)
echo ✓ Administrator privileges confirmed

echo.
echo ┌─────────────────────────────────────────────────────────────────────────────┐
echo │                          INSTALLING THE KING'S CAB                         │
echo └─────────────────────────────────────────────────────────────────────────────┘
echo.

echo [1/6] Creating directories...
mkdir "%ProgramFiles%\Common Files\VST3" 2>nul
mkdir "%ProgramFiles%\Common Files\Avid\Audio\Plug-Ins" 2>nul
mkdir "%ProgramData%\King Studios" 2>nul
mkdir "%ProgramData%\King Studios\The King's Cab" 2>nul
mkdir "%ProgramData%\King Studios\The King's Cab\IR Collections" 2>nul
echo      ✓ Plugin directories created

echo [2/6] Installing VST3 plugin...
if exist "%~dp0plugins\VST3\The King's Cab.vst3" (
    xcopy /E /I /Y "%~dp0plugins\VST3\The King's Cab.vst3" "%ProgramFiles%\Common Files\VST3\The King's Cab.vst3" >nul 2>&1
    echo      ✓ VST3 plugin installed
) else (
    echo      ⚠️  VST3 plugin files not found
)

echo [3/6] Installing AAX plugin for Pro Tools...
if exist "%~dp0plugins\AAX\The King's Cab.aaxplugin" (
    xcopy /E /I /Y "%~dp0plugins\AAX\The King's Cab.aaxplugin" "%ProgramFiles%\Common Files\Avid\Audio\Plug-Ins\The King's Cab.aaxplugin" >nul 2>&1
    echo      ✓ AAX plugin installed
) else (
    echo      ⚠️  AAX plugin files not found
)

echo [4/6] Installing IR Collections...
if exist "%~dp0ir_collections" (
    xcopy /E /I /Y "%~dp0ir_collections\*" "%ProgramData%\King Studios\The King's Cab\IR Collections" >nul 2>&1
    echo      ✓ IR Collections installed
    
    echo      📁 Collections included:
    for /d %%i in ("%~dp0ir_collections\*") do echo         • %%~nxi
) else (
    echo      ⚠️  IR Collections not found
)

echo [5/6] Setting permissions...
icacls "%ProgramFiles%\Common Files\VST3\The King's Cab.vst3" /grant Users:F /T >nul 2>&1
icacls "%ProgramFiles%\Common Files\Avid\Audio\Plug-Ins\The King's Cab.aaxplugin" /grant Users:F /T >nul 2>&1
icacls "%ProgramData%\King Studios\The King's Cab" /grant Users:F /T >nul 2>&1
echo      ✓ Permissions configured

echo [6/6] Creating registry entries...
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\The King's Cab" /v "DisplayName" /t REG_SZ /d "The King's Cab" /f >nul 2>&1
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\The King's Cab" /v "DisplayVersion" /t REG_SZ /d "1.0.0" /f >nul 2>&1
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\The King's Cab" /v "Publisher" /t REG_SZ /d "King Studios" /f >nul 2>&1
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\The King's Cab" /v "URLInfoAbout" /t REG_SZ /d "https://www.kingstudiospa.com" /f >nul 2>&1
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\The King's Cab" /v "NoModify" /t REG_DWORD /d 1 /f >nul 2>&1
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\The King's Cab" /v "NoRepair" /t REG_DWORD /d 1 /f >nul 2>&1
echo      ✓ Registry entries created

echo.
echo ┌─────────────────────────────────────────────────────────────────────────────┐
echo │                        ✅ INSTALLATION COMPLETED!                           │
echo └─────────────────────────────────────────────────────────────────────────────┘
echo.
echo 🎸 The King's Cab has been successfully installed!
echo.
echo 📍 INSTALLATION LOCATIONS:
echo    • VST3: %ProgramFiles%\Common Files\VST3\
echo    • AAX:  %ProgramFiles%\Common Files\Avid\Audio\Plug-Ins\
echo    • IRs:  %ProgramData%\King Studios\The King's Cab\IR Collections\
echo.
echo 🚀 NEXT STEPS:
echo    1. Restart your DAW (Pro Tools, Reaper, Cubase, etc.)
echo    2. Look for "The King's Cab" in your plugin list
echo    3. Load your first IR and start creating!
echo.
echo 💰 PREMIUM CONTENT:
echo    Visit https://www.kingstudiospa.com/store for additional IR collections
echo.
echo ===============================================================================
echo              Thank you for choosing King Studios professional audio!
echo ===============================================================================
echo.
pause

REM Optional: Open the store page
set /p openstore="Would you like to visit the King Studios store now? (y/n): "
if /i "%openstore%"=="y" start https://www.kingstudiospa.com/store
