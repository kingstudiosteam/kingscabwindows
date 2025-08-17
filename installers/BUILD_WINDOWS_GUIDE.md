# Building The King's Cab for Windows

## 🎯 Problem Identified
The current Windows installer contains macOS binaries, which won't work on Windows. We need to build proper Windows VST3/AAX binaries.

## 🛠️ Windows Build Requirements

### Software Needed:
1. **Visual Studio 2019 or 2022** (Community Edition is free)
2. **Git for Windows**
3. **CMake 3.18+**

### Optional (for AAX):
- **Pro Tools AAX SDK** (requires Avid Developer account)

## 📋 Build Steps for Windows

### 1. Clone and Setup
```cmd
git clone [repository-url]
cd cursorfiles
```

### 2. Create Build Directory
```cmd
mkdir build_windows
cd build_windows
```

### 3. Configure with CMake
```cmd
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DJUCE_BUILD_EXTRAS=OFF ^
    -DJUCE_BUILD_EXAMPLES=OFF
```

### 4. Build the Plugin
```cmd
cmake --build . --config Release --parallel 4
```

### 5. Locate Built Files
After successful build, files will be in:
```
build_windows/TheKingsCab_artefacts/Release/VST3/The King's Cab.vst3/
build_windows/TheKingsCab_artefacts/Release/AAX/The King's Cab.aaxplugin/
```

### 6. Create Installer Package
```cmd
cd installers/windows
copy "../../build_windows/TheKingsCab_artefacts/Release/VST3/The King's Cab.vst3" plugins/VST3/
copy "../../build_windows/TheKingsCab_artefacts/Release/AAX/The King's Cab.aaxplugin" plugins/AAX/
```

## 🎸 IR Collections
The IR collections are already included in the installer package at:
```
installers/windows/ir_collections/
```

## 🚀 Final Installer
After building Windows binaries, the installer will include:
- ✅ Windows VST3 plugin
- ✅ Windows AAX plugin
- ✅ Complete IR collection (500+ files)
- ✅ Professional installation script

## 💡 Alternative: GitHub Actions
We could set up GitHub Actions to automatically build Windows binaries on every commit.

## 🔧 Troubleshooting

### Build Errors:
1. Ensure Visual Studio C++ tools are installed
2. Check CMake can find Visual Studio
3. Verify Git is in PATH

### Missing AAX:
AAX requires Pro Tools SDK. VST3 will work without it.

### Plugin Not Loading:
1. Check Windows version (64-bit required)
2. Verify DAW supports VST3
3. Check plugin was copied to correct location
