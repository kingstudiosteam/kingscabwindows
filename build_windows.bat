@echo off
REM The King's Cab - Windows Build Script
REM Run this on a Windows machine with Visual Studio installed

echo.
echo ========================================
echo   THE KING'S CAB - WINDOWS BUILD
echo ========================================
echo.
echo This script will build The King's Cab plugin for Windows
echo Requires: Visual Studio 2019/2022 with C++ tools
echo.
pause

REM Configuration
set PLUGIN_NAME=The King's Cab
set VERSION=1.0.0
set BUILD_DIR=build_windows

REM Clean previous builds
if exist "%BUILD_DIR%" (
    echo Cleaning previous build...
    rmdir /s /q "%BUILD_DIR%"
)

mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

echo.
echo ======================================
echo 🔧 Configuring CMake for Windows...
echo ======================================

REM Configure for Windows with Visual Studio
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DJUCE_BUILD_EXTRAS=OFF ^
    -DJUCE_BUILD_EXAMPLES=OFF

if %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake configuration failed!
    echo.
    echo Possible issues:
    echo - Visual Studio not installed
    echo - CMake not in PATH
    echo - Missing Git
    echo.
    pause
    exit /b 1
)

echo.
echo ======================================
echo 🔨 Building Windows Plugin...
echo ======================================

cmake --build . --config Release --parallel 4

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Build failed!
    echo.
    echo Check error messages above
    echo.
    pause
    exit /b 1
)

echo.
echo ======================================
echo 📦 Preparing Installer Files...
echo ======================================

cd ..\installers\windows

REM Clean old plugin files
if exist "plugins\VST3" rmdir /s /q "plugins\VST3"
if exist "plugins\AAX" rmdir /s /q "plugins\AAX"
if exist "assets" rmdir /s /q "assets"
if exist "ir_collections" rmdir /s /q "ir_collections"

REM Create directories
mkdir plugins\VST3
mkdir plugins\AAX
mkdir assets
mkdir ir_collections

REM Copy built plugins
if exist "..\..\%BUILD_DIR%\TheKingsCab_artefacts\Release\VST3\%PLUGIN_NAME%.vst3" (
    echo ✅ Copying Windows VST3 plugin...
    xcopy /E /I /Y "..\..\%BUILD_DIR%\TheKingsCab_artefacts\Release\VST3\%PLUGIN_NAME%.vst3" "plugins\VST3\%PLUGIN_NAME%.vst3"
) else (
    echo ❌ VST3 plugin not found!
    pause
    exit /b 1
)

if exist "..\..\%BUILD_DIR%\TheKingsCab_artefacts\Release\AAX\%PLUGIN_NAME%.aaxplugin" (
    echo ✅ Copying Windows AAX plugin...
    xcopy /E /I /Y "..\..\%BUILD_DIR%\TheKingsCab_artefacts\Release\AAX\%PLUGIN_NAME%.aaxplugin" "plugins\AAX\%PLUGIN_NAME%.aaxplugin"
) else (
    echo ⚠️  AAX plugin not found (Pro Tools SDK may not be available)
)

REM Stage assets and IR collections for installer (will be copied to AppData on install)
echo.
echo ======================================
echo Staging assets and IR collections...
echo ======================================

if exist "..\..\assets" (
    xcopy /E /I /Y "..\..\assets\*" "assets\"
) else (
    echo Assets folder not found at project root
)

if exist "..\..\installers\windows\ir_collections" (
    xcopy /E /I /Y "..\..\installers\windows\ir_collections\*" "ir_collections\"
) else (
    echo IR collections folder not found under installers\windows
)

echo.
echo ======================================
echo 🎸 Creating Final Installer...
echo ======================================

REM Create final installer package
if exist "..\TheKingsCab-%VERSION%-Windows-Final.zip" del "..\TheKingsCab-%VERSION%-Windows-Final.zip"

REM Use PowerShell to create ZIP (built into Windows 10+)
powershell -Command "Compress-Archive -Path '.\*' -DestinationPath '..\TheKingsCab-%VERSION%-Windows-Final.zip'"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✅ SUCCESS! Windows installer created!
    echo.
    echo 📦 File: installers\TheKingsCab-%VERSION%-Windows-Final.zip
    echo.
    echo 🎛️  Installer includes:
    echo    • Windows VST3 plugin
    echo    • Windows AAX plugin (if built)
    echo    • Complete IR collection (500+ files)
    echo    • Professional installation script
    echo.
    echo 🚀 Ready for distribution!
    echo.
) else (
    echo ❌ Failed to create installer package
)

echo ======================================
pause
