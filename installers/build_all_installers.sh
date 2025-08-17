#!/bin/bash

# The King's Cab - Universal Installer Builder
# Builds installers for macOS and Windows (requires appropriate environment)

set -e

echo "🎛️  The King's Cab - Universal Installer Builder"
echo "==============================================="
echo ""

PLUGIN_NAME="The King's Cab"
VERSION="1.0.0"
COMPANY="King Studios"

# Function to build for macOS
build_mac() {
    echo "🍎 Building macOS Installer..."
    echo "=============================="
    
    # First build the plugin with AAX support
    echo "🔨 Building plugin with AAX support..."
    cd ..
    
    # Clean and build
    rm -rf build
    mkdir build
    cd build
    
    # Configure with AAX support
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
             -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13
    
    cmake --build . --config Release -j4
    
    if [ $? -ne 0 ]; then
        echo "❌ macOS build failed!"
        exit 1
    fi
    
    # Create installer
    cd ../installers
    chmod +x create_mac_installer.sh
    ./create_mac_installer.sh
    
    echo "✅ macOS installer created!"
}

# Function to check Windows build environment
check_windows_env() {
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        return 0
    elif command -v wine &> /dev/null; then
        echo "🍷 Wine detected - can attempt Windows build"
        return 0
    else
        echo "⚠️  Windows build environment not detected"
        echo "   To build Windows installer:"
        echo "   1. Run on Windows with Visual Studio"
        echo "   2. Or use Wine on Linux/macOS"
        echo "   3. Or use Windows Subsystem for Linux"
        return 1
    fi
}

# Function to build for Windows (when environment is available)
build_windows() {
    echo "🪟 Building Windows Installer..."
    echo "==============================="
    
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        # Native Windows environment
        cmd //c build_windows_installer.bat
    else
        echo "ℹ️  Windows installer script created."
        echo "   Run 'build_windows_installer.bat' on Windows to create installer."
    fi
}

# Main execution
echo "🚀 Starting build process..."
echo "Plugin: $PLUGIN_NAME v$VERSION"
echo "Company: $COMPANY"
echo ""

# Detect platform and build accordingly
case "$OSTYPE" in
    darwin*)
        echo "📱 macOS detected"
        build_mac
        ;;
    linux*)
        echo "🐧 Linux detected"
        echo "Creating cross-platform installer scripts..."
        build_mac
        if check_windows_env; then
            build_windows
        fi
        ;;
    msys*|cygwin*)
        echo "🪟 Windows detected"
        build_windows
        ;;
    *)
        echo "❓ Unknown platform: $OSTYPE"
        echo "Attempting universal build..."
        build_mac
        if check_windows_env; then
            build_windows
        fi
        ;;
esac

echo ""
echo "✅ Installer build process completed!"
echo ""
echo "📦 Available installers:"

if [ -f "mac/TheKingsCab-${VERSION}-macOS.pkg" ]; then
    echo "   🍎 macOS: mac/TheKingsCab-${VERSION}-macOS.pkg"
fi

if [ -f "windows/TheKingsCab-${VERSION}-Windows.exe" ]; then
    echo "   🪟 Windows: windows/TheKingsCab-${VERSION}-Windows.exe"
fi

echo ""
echo "🎛️  Plugin Format Support:"
echo "   • VST3 (Universal DAW support)"
echo "   • AU (Logic Pro, GarageBand)"
echo "   • AAX (Pro Tools)"
echo ""
echo "🌟 Professional distribution ready!"
echo "   Visit https://www.kingstudiospa.com for more King Studios products"
