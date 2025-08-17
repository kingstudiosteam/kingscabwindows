#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
PLUGIN_NAME="Multi-Blend"
AU_DST="/Library/Audio/Plug-Ins/Components/${PLUGIN_NAME}.component"
VST3_DST="/Library/Audio/Plug-Ins/VST3/${PLUGIN_NAME}.vst3"

echo "[Install] Configure (Release)"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_BUILD_EXAMPLES=OFF -DJUCE_BUILD_EXTRAS=OFF -DJUCE_FETCH_VST3_SDK=ON

echo "[Install] Build"
cmake --build "$BUILD_DIR" --config Release --parallel

AU_SRC="$BUILD_DIR/CrossFX_artefacts/Release/AU/${PLUGIN_NAME}.component"
VST3_SRC="$BUILD_DIR/CrossFX_artefacts/Release/VST3/${PLUGIN_NAME}.vst3"

if [[ ! -d "$AU_SRC" ]]; then echo "AU not found: $AU_SRC" >&2; exit 1; fi
if [[ ! -d "$VST3_SRC" ]]; then echo "VST3 not found: $VST3_SRC" >&2; exit 1; fi

echo "[Install] Copy AU and VST3 to system folders (will prompt for admin)"
osascript -e "do shell script \"mkdir -p '/Library/Audio/Plug-Ins/Components' '/Library/Audio/Plug-Ins/VST3' && cp -R '$AU_SRC' '$AU_DST' && cp -R '$VST3_SRC' '$VST3_DST' && xattr -dr com.apple.quarantine '$AU_DST' '$VST3_DST' && codesign -s - --deep -f '$AU_DST' '$VST3_DST'\" with administrator privileges"

echo "[Install] Refresh AU cache"
killall -9 AudioComponentRegistrar 2>/dev/null || true
sleep 1

echo "[Install] Validate AU (aufx Mlbd MltB)"
auval -v aufx Mlbd MltB | tail -n 25 || true

echo "[Install] Done. AU at: $AU_DST, VST3 at: $VST3_DST"


