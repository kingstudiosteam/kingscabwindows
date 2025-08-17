#!/bin/bash

# The King's Cab - macOS Installer Creator
# Creates a professional .pkg installer for VST3, AU, and AAX formats
# Compatible with Logic Pro, Pro Tools, and all major DAWs

set -e

echo "🎛️  Creating The King's Cab macOS Installer..."
echo "=============================================="

# Configuration
PLUGIN_NAME="The King's Cab"
COMPANY_NAME="King Studios"
VERSION="1.0.0"
BUNDLE_ID="com.kingstudios.thekingscab"
BUILD_DIR="../build"
INSTALLER_DIR="./mac"
PKG_DIR="${INSTALLER_DIR}/pkg_contents"
RESOURCES_DIR="${INSTALLER_DIR}/resources"

# Clean and create directories
rm -rf "${INSTALLER_DIR}"
mkdir -p "${PKG_DIR}/Applications/Audio/Plug-Ins"
mkdir -p "${PKG_DIR}/Library/Audio/Plug-Ins"
mkdir -p "${RESOURCES_DIR}"

echo "📦 Preparing plugin files..."

# Create plugin directories
mkdir -p "${PKG_DIR}/Library/Audio/Plug-Ins/VST3"
mkdir -p "${PKG_DIR}/Library/Audio/Plug-Ins/Components"
mkdir -p "${PKG_DIR}/Library/Application Support/Avid/Audio/Plug-Ins"

# Create IR files directory
mkdir -p "${PKG_DIR}/Users/Shared/King Studios/The Kings Cab/IR Collections"

# Copy VST3 plugin
if [ -d "${BUILD_DIR}/TheKingsCab_artefacts/Release/VST3/${PLUGIN_NAME}.vst3" ]; then
    echo "✅ Copying VST3 plugin..."
    cp -R "${BUILD_DIR}/TheKingsCab_artefacts/Release/VST3/${PLUGIN_NAME}.vst3" \
          "${PKG_DIR}/Library/Audio/Plug-Ins/VST3/"
else
    echo "❌ VST3 plugin not found. Building first..."
    exit 1
fi

# Copy AU component
if [ -d "${BUILD_DIR}/TheKingsCab_artefacts/Release/AU/${PLUGIN_NAME}.component" ]; then
    echo "✅ Copying AU component..."
    cp -R "${BUILD_DIR}/TheKingsCab_artefacts/Release/AU/${PLUGIN_NAME}.component" \
          "${PKG_DIR}/Library/Audio/Plug-Ins/Components/"
else
    echo "❌ AU component not found. Building first..."
    exit 1
fi

# Copy AAX plugin (if available)
if [ -d "${BUILD_DIR}/TheKingsCab_artefacts/Release/AAX/${PLUGIN_NAME}.aaxplugin" ]; then
    echo "✅ Copying AAX plugin..."
    cp -R "${BUILD_DIR}/TheKingsCab_artefacts/Release/AAX/${PLUGIN_NAME}.aaxplugin" \
          "${PKG_DIR}/Library/Application Support/Avid/Audio/Plug-Ins/"
else
    echo "⚠️  AAX plugin not found. Pro Tools support will be limited."
fi

# Copy IR Collections
IR_SOURCE="/Users/justinmitchell/Desktop/KINGS CAB"
if [ -d "${IR_SOURCE}" ]; then
    echo "🎸 Copying IR Collections..."
    cp -R "${IR_SOURCE}/"* "${PKG_DIR}/Users/Shared/King Studios/The Kings Cab/IR Collections/"
    echo "✅ IR Collections copied successfully!"
    echo "   Collections included:"
    ls "${IR_SOURCE}" | sed 's/^/   • /'
else
    echo "❌ IR Collections not found at: ${IR_SOURCE}"
    echo "   Plugin will work but won't have IR files loaded."
fi

# Create preinstall script
cat > "${RESOURCES_DIR}/preinstall" << 'EOF'
#!/bin/bash
# Remove any existing installations
rm -rf "/Library/Audio/Plug-Ins/VST3/The King's Cab.vst3"
rm -rf "/Library/Audio/Plug-Ins/Components/The King's Cab.component"
rm -rf "/Library/Application Support/Avid/Audio/Plug-Ins/The King's Cab.aaxplugin"
exit 0
EOF

# Create postinstall script
cat > "${RESOURCES_DIR}/postinstall" << 'EOF'
#!/bin/bash
echo "The King's Cab installation completed successfully!"

# Set proper permissions for plugins
chmod -R 755 "/Library/Audio/Plug-Ins/VST3/The King's Cab.vst3"
chmod -R 755 "/Library/Audio/Plug-Ins/Components/The King's Cab.component"

if [ -d "/Library/Application Support/Avid/Audio/Plug-Ins/The King's Cab.aaxplugin" ]; then
    chmod -R 755 "/Library/Application Support/Avid/Audio/Plug-Ins/The King's Cab.aaxplugin"
fi

# Set proper permissions for IR Collections
if [ -d "/Users/Shared/King Studios/The Kings Cab/IR Collections" ]; then
    chmod -R 755 "/Users/Shared/King Studios/The Kings Cab"
    echo "✅ IR Collections installed and configured!"
fi

# Kill any running DAWs to ensure plugin registration
killall "Logic Pro" 2>/dev/null || true
killall "Pro Tools" 2>/dev/null || true
killall "Reaper" 2>/dev/null || true
killall "Ableton Live" 2>/dev/null || true

echo "✅ The King's Cab is ready to use in your DAW!"
echo "🎸 Visit https://www.kingstudiospa.com/store for premium IR collections"

exit 0
EOF

chmod +x "${RESOURCES_DIR}/preinstall"
chmod +x "${RESOURCES_DIR}/postinstall"

# Create installer welcome text
cat > "${RESOURCES_DIR}/welcome.txt" << EOF
Welcome to The King's Cab Installer

This installer will install The King's Cab professional impulse response loader plugin.

Compatible with:
• Logic Pro (AU Component)
• Pro Tools (AAX Plugin)
• All major DAWs (VST3 Plugin)

Version: ${VERSION}
Company: ${COMPANY_NAME}

The King's Cab provides premium cabinet simulation with 6 IR slots for professional guitar and bass recording.
EOF

# Create README
cat > "${RESOURCES_DIR}/readme.txt" << EOF
The King's Cab - Professional Impulse Response Loader

INSTALLATION LOCATIONS:
• VST3: /Library/Audio/Plug-Ins/VST3/
• AU: /Library/Audio/Plug-Ins/Components/
• AAX: /Library/Application Support/Avid/Audio/Plug-Ins/

COMPATIBILITY:
• macOS 10.13 or later
• Intel and Apple Silicon Macs
• 64-bit DAWs only

AFTER INSTALLATION:
1. Restart your DAW
2. Look for "The King's Cab" in your plugin list
3. Load your premium IR collection from the designated folder

For support and premium IR collections:
https://www.kingstudiospa.com/store

© ${COMPANY_NAME} - All rights reserved
EOF

echo "🔨 Building installer package..."

# Build the package
pkgbuild \
    --root "${PKG_DIR}" \
    --identifier "${BUNDLE_ID}" \
    --version "${VERSION}" \
    --scripts "${RESOURCES_DIR}" \
    --install-location "/" \
    "${INSTALLER_DIR}/TheKingsCab-Core.pkg"

# Create distribution XML
cat > "${INSTALLER_DIR}/distribution.xml" << EOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="1">
    <title>The King's Cab ${VERSION}</title>
    <organization>${COMPANY_NAME}</organization>
    <domains enable_anywhere="false" enable_currentUserHome="false" enable_localSystem="true"/>
    <options customize="never" require-scripts="true" rootVolumeOnly="true" hostArchitectures="x86_64,arm64"/>
    
    <welcome file="welcome.txt"/>
    <readme file="readme.txt"/>
    
    <pkg-ref id="${BUNDLE_ID}"/>
    
    <choices-outline>
        <line choice="default">
            <line choice="${BUNDLE_ID}"/>
        </line>
    </choices-outline>
    
    <choice id="default"/>
    <choice id="${BUNDLE_ID}" visible="false">
        <pkg-ref id="${BUNDLE_ID}"/>
    </choice>
    
    <pkg-ref id="${BUNDLE_ID}" version="${VERSION}" onConclusion="none">TheKingsCab-Core.pkg</pkg-ref>
</installer-gui-script>
EOF

# Build the final installer
productbuild \
    --distribution "${INSTALLER_DIR}/distribution.xml" \
    --resources "${RESOURCES_DIR}" \
    --package-path "${INSTALLER_DIR}" \
    "${INSTALLER_DIR}/TheKingsCab-${VERSION}-macOS.pkg"

echo ""
echo "✅ macOS Installer created successfully!"
echo "📦 Location: ${INSTALLER_DIR}/TheKingsCab-${VERSION}-macOS.pkg"
echo ""
echo "🎛️  Installation includes:"
echo "   • VST3 plugin for all DAWs"
echo "   • AU component for Logic Pro"
echo "   • AAX plugin for Pro Tools (if built)"
echo ""
echo "🚀 Ready for distribution!"
