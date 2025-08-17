# The King's Cab - Professional Installers

Professional installer packages for The King's Cab impulse response loader plugin.

## 📦 Installer Formats

### macOS Installer (.pkg)
- **VST3**: Universal DAW support (Reaper, Cubase, Studio One, etc.)
- **AU Component**: Logic Pro, GarageBand, MainStage
- **AAX Plugin**: Pro Tools (requires Pro Tools AAX SDK)

### Windows Installer (.exe)
- **VST3**: Universal DAW support (Reaper, Cubase, Studio One, FL Studio, etc.)
- **AAX Plugin**: Pro Tools (requires Pro Tools AAX SDK)

## 🛠️ Building Installers

### Quick Build (Current Platform)
```bash
# macOS/Linux
./build_all_installers.sh

# Windows
build_windows_installer.bat
```

### macOS Only
```bash
./create_mac_installer.sh
```

### Windows Only
```bash
# Run on Windows with Visual Studio installed
build_windows_installer.bat
```

## 📋 Prerequisites

### macOS
- Xcode or Xcode Command Line Tools
- CMake 3.18+
- JUCE framework (auto-downloaded)
- **Optional**: Pro Tools AAX SDK for AAX format

### Windows
- Visual Studio 2019/2022 with C++ tools
- CMake 3.18+
- NSIS (Nullsoft Scriptable Install System)
- **Optional**: Pro Tools AAX SDK for AAX format

## 🎯 Installation Targets

### macOS
- VST3: `/Library/Audio/Plug-Ins/VST3/`
- AU: `/Library/Audio/Plug-Ins/Components/`
- AAX: `/Library/Application Support/Avid/Audio/Plug-Ins/`

### Windows
- VST3: `C:\Program Files\Common Files\VST3\`
- AAX: `C:\Program Files\Common Files\Avid\Audio\Plug-Ins\`

## 🔐 Code Signing (Production)

### macOS
```bash
# Sign the plugins before packaging
codesign --force --sign "Developer ID Application: Your Name" \
    "build/TheKingsCab_artefacts/Release/VST3/The King's Cab.vst3"

# Sign the installer
productsign --sign "Developer ID Installer: Your Name" \
    "TheKingsCab-1.0.0-macOS.pkg" \
    "TheKingsCab-1.0.0-macOS-signed.pkg"
```

### Windows
```bash
# Sign with Authenticode certificate
signtool sign /f "certificate.p12" /p "password" \
    "TheKingsCab-1.0.0-Windows.exe"
```

## 🧪 Testing Installation

### macOS
```bash
# Install locally for testing
sudo installer -pkg "TheKingsCab-1.0.0-macOS.pkg" -target /
```

### Windows
```bash
# Run installer with logging
TheKingsCab-1.0.0-Windows.exe /S /D="C:\Program Files\King Studios\The King's Cab"
```

## 📁 Installer Contents

Each installer includes:
- ✅ Plugin binaries (VST3, AU, AAX)
- ✅ Proper installation paths
- ✅ Registry entries (Windows)
- ✅ Uninstaller
- ✅ License agreement
- ✅ Installation scripts
- ✅ DAW restart recommendations

## 🎸 Distribution

The generated installers are production-ready for:
- Direct distribution to customers
- Upload to plugin marketplaces
- Integration with online stores
- Professional music production environments

## 📞 Support

For technical support and premium IR collections:
- Website: https://www.kingstudiospa.com
- Store: https://www.kingstudiospa.com/store

---

© King Studios - Professional Audio Software
