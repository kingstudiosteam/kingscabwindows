# The King's Cab - Build Commands

## Quick Commands

### 🚀 Build & Auto-Launch (Recommended)
```bash
./build_and_run.sh
```
**or**
```bash
./run.sh
```

**What it does:**
- Builds The King's Cab with parallel compilation (`make -j4`)
- Automatically opens the standalone plugin in a new window
- Perfect for instant testing of your changes!

---

### 🔨 Manual Build Only
```bash
cd build
make -j4
```

**What it does:**
- Just builds the plugin without launching
- Use when you only want to compile

---

### 🎛️ Manual Launch (After Build)
```bash
open "./build/TheKingsCab_artefacts/Release/Standalone/The King's Cab.app"
```

**What it does:**
- Opens the standalone plugin manually
- Use if you want to launch without rebuilding

---

## Plugin Formats Built

- **🎛️ Standalone App:** `./build/TheKingsCab_artefacts/Release/Standalone/The King's Cab.app`
- **🎚️ VST3 Plugin:** Installed to `~/Library/Audio/Plug-Ins/VST3/The King's Cab.vst3`
- **🎵 AU Plugin:** Installed to `~/Library/Audio/Plug-Ins/Components/The King's Cab.component`

---

## Development Workflow

1. **Make code changes**
2. **Run:** `./build_and_run.sh`
3. **Test your changes** in the auto-opened plugin window
4. **Repeat!**

The auto-launch feature saves you time by instantly opening the plugin for testing after each successful build! 🎸

