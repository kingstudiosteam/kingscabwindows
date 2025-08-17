#!/bin/bash

# The King's Cab IR Loader Build Script
# This script builds The King's Cab premium IR loader plugin for macOS

set -e

echo "👑 Building The King's Cab IR Loader Plugin..."
echo "=============================================="

# Create build directory
mkdir -p build
cd build

# Configure with CMake for maximum performance
echo "📋 Configuring build for premium audio quality..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
         -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
         -DCMAKE_CXX_FLAGS="-O3 -ffast-math -DNDEBUG" \
         -DJUCE_BUILD_EXAMPLES=OFF \
         -DJUCE_BUILD_EXTRAS=OFF \
         -DJUCE_FETCH_VST3_SDK=ON

# Build the plugin with all available cores
echo "🔨 Building premium IR loader..."
cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "✅ Build completed successfully!"
echo ""
echo "📁 Plugin location:"
echo "   VST3: build/TheKingsCab_artefacts/Release/VST3/TheKingsCab.vst3"
echo "   AU:   build/TheKingsCab_artefacts/Release/AU/TheKingsCab.component"
echo "   Standalone: build/TheKingsCab_artefacts/Release/Standalone/TheKingsCab.app"
echo ""
echo "🎛️  To install the plugin:"
echo "   VST3: Copy to ~/Library/Audio/Plug-Ins/VST3/"
echo "   AU:   Copy to ~/Library/Audio/Plug-Ins/Components/"
echo ""
echo "🎸 Ready to load your premium cabinet IRs!"
echo "📂 Set your IR directory to: /Users/justinmitchell/Desktop/KINGS\\ CAB"
echo ""
echo "✨ Pristine audio quality guaranteed! ✨"