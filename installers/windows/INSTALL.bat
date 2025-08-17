@echo off
REM The King's Cab - Windows Installation Launcher
REM This batch file will run the PowerShell installer with proper permissions

echo.
echo ========================================
echo   THE KING'S CAB - WINDOWS INSTALLER
echo ========================================
echo.
echo This installer will install The King's Cab plugin for:
echo   * VST3 compatible DAWs (Reaper, Cubase, Studio One, etc.)
echo   * Pro Tools (AAX format)
echo.
echo IMPORTANT: This installer requires Administrator privileges.
echo You may see a User Account Control (UAC) prompt.
echo.
pause

REM Check if running as administrator
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Running with Administrator privileges...
    goto :install
)

REM Request administrator privileges
echo Requesting Administrator privileges...
powershell -Command "Start-Process cmd -ArgumentList '/c cd /d \"%~dp0\" && powershell.exe -ExecutionPolicy Bypass -File \"TheKingsCab-Windows-Installer.ps1\"' -Verb RunAs"
goto :end

:install
echo Installing The King's Cab...
powershell.exe -ExecutionPolicy Bypass -File "%~dp0TheKingsCab-Windows-Installer.ps1"

:end
echo.
echo Installation process completed.
pause
