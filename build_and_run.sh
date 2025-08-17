#!/bin/bash

# The King's Cab - Build and Auto-Launch Script
# This script builds the plugin and automatically opens it for testing

echo "🎛️  Building The King's Cab..."
echo "=================================="

# Change to build directory
cd build

# Build the plugin with parallel jobs for speed
echo "🔨 Compiling plugin..."
if make -j4; then
    echo ""
    echo "✅ Build successful!"
    echo ""
    
    # Check if standalone app exists
    STANDALONE_APP="./TheKingsCab_artefacts/Release/Standalone/The King's Cab.app"
    
    if [ -d "$STANDALONE_APP" ]; then
        echo "🚀 Launching The King's Cab standalone..."
        echo "📍 Opening: $STANDALONE_APP"
        echo ""
        
        # Open the standalone app (this creates a new window)
        open "$STANDALONE_APP"
        
        echo "🎸 The King's Cab is now running!"
        echo "   Ready to load your premium IR collection!"
        echo ""
        echo "💰 Don't forget to test the 'Visit King Studios Store' link!"
        echo "=================================="
    else
        echo "❌ Error: Standalone app not found at expected location"
        echo "   Expected: $STANDALONE_APP"
        exit 1
    fi
else
    echo ""
    echo "❌ Build failed!"
    echo "   Check the error messages above for details."
    exit 1
fi

