@echo off
REM The King's Cab - Windows Build and Installer Script
REM Builds the plugin and creates a professional installer

echo.
echo 🎛️  Building The King's Cab for Windows...
echo ===========================================

REM Configuration
set PLUGIN_NAME=The King's Cab
set VERSION=1.0.0
set BUILD_DIR=..\build_windows
set INSTALLER_DIR=.\windows

REM Clean previous builds
if exist "%BUILD_DIR%" (
    echo 🧹 Cleaning previous build...
    rmdir /s /q "%BUILD_DIR%"
)

if exist "%INSTALLER_DIR%" (
    rmdir /s /q "%INSTALLER_DIR%"
)

mkdir "%BUILD_DIR%"
mkdir "%INSTALLER_DIR%"

echo.
echo 🔨 Configuring CMake for Windows...
cd "%BUILD_DIR%"

REM Configure for Windows with Visual Studio
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DJUCE_BUILD_EXTRAS=OFF ^
    -DJUCE_BUILD_EXAMPLES=OFF

if %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo 🔧 Building plugin...
cmake --build . --config Release --parallel 4

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Build failed!
    pause
    exit /b 1
)

echo.
echo 📦 Preparing installer files...
cd ..\installers

REM Create Windows installer directory structure
mkdir "%INSTALLER_DIR%\windows_build\VST3"
mkdir "%INSTALLER_DIR%\windows_build\AAX"

REM Copy plugin files
if exist "%BUILD_DIR%\TheKingsCab_artefacts\Release\VST3\%PLUGIN_NAME%.vst3" (
    echo ✅ Copying VST3 plugin...
    xcopy /E /I /Y "%BUILD_DIR%\TheKingsCab_artefacts\Release\VST3\%PLUGIN_NAME%.vst3" "%INSTALLER_DIR%\windows_build\VST3\%PLUGIN_NAME%.vst3"
) else (
    echo ❌ VST3 plugin not found!
    pause
    exit /b 1
)

REM Copy AAX plugin if available
if exist "%BUILD_DIR%\TheKingsCab_artefacts\Release\AAX\%PLUGIN_NAME%.aaxplugin" (
    echo ✅ Copying AAX plugin...
    xcopy /E /I /Y "%BUILD_DIR%\TheKingsCab_artefacts\Release\AAX\%PLUGIN_NAME%.aaxplugin" "%INSTALLER_DIR%\windows_build\AAX\%PLUGIN_NAME%.aaxplugin"
) else (
    echo ⚠️  AAX plugin not found. Pro Tools support will be limited.
)

REM Create license file
echo THE KING'S CAB SOFTWARE LICENSE > "%INSTALLER_DIR%\license.txt"
echo. >> "%INSTALLER_DIR%\license.txt"
echo Copyright (c) King Studios. All rights reserved. >> "%INSTALLER_DIR%\license.txt"
echo. >> "%INSTALLER_DIR%\license.txt"
echo This software is licensed for use as an audio plugin. >> "%INSTALLER_DIR%\license.txt"
echo Redistribution is prohibited without written permission. >> "%INSTALLER_DIR%\license.txt"
echo. >> "%INSTALLER_DIR%\license.txt"
echo For support and premium IR collections: >> "%INSTALLER_DIR%\license.txt"
echo https://www.kingstudiospa.com/store >> "%INSTALLER_DIR%\license.txt"

echo.
echo 🏗️  Building Windows installer...

REM Check if NSIS is installed
where makensis >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ❌ NSIS not found! Please install NSIS from https://nsis.sourceforge.io/
    echo    Then add it to your PATH environment variable.
    pause
    exit /b 1
)

REM Build the installer
cd "%INSTALLER_DIR%"
makensis create_windows_installer.nsi

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Installer creation failed!
    pause
    exit /b 1
)

echo.
echo ✅ Windows Installer created successfully!
echo 📦 Location: installers\windows\TheKingsCab-%VERSION%-Windows.exe
echo.
echo 🎛️  Installation includes:
echo    • VST3 plugin for all DAWs
echo    • AAX plugin for Pro Tools (if built)
echo    • Proper Windows registry entries
echo    • Professional uninstaller
echo.
echo 🚀 Ready for distribution!

pause
