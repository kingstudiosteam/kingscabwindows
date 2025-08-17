# The King's Cab - Premium IR Loader

**Professional-grade impulse response loader plugin for guitar cabinet simulation**

*King Studios - Professional Audio Software*

---

## Overview

The King's Cab is a premium VST3/AU plugin designed specifically for guitar players who demand pristine cabinet simulation. Load up to 6 impulse responses simultaneously with individual controls for ultimate tonal flexibility.

### Key Features

- **6 IR Slots** - Load multiple impulse responses simultaneously
- **Premium 3D Interface** - Stunning metallic UI with gold accents
- **Advanced Blending** - Individual volume, pan, solo, mute, and phase controls per slot
- **Folder Browser** - Intuitive IR library management with tree view
- **Pristine Audio Quality** - Optimized convolution engine for professional results
- **Low CPU Usage** - Highly optimized for real-time performance
- **Cross-Platform** - VST3 and AU formats for macOS, Windows, and Linux

## Quick Start

### Building the Plugin

1. **Prerequisites:**
   - CMake 3.24+
   - Xcode (macOS) or Visual Studio (Windows)
   - Git

2. **Build:**
   ```bash
   chmod +x scripts/build.sh
   ./scripts/build.sh
   ```

3. **Install:**
   - Copy the generated `.vst3` file to your VST3 plugins folder
   - Copy the `.component` file to your AU plugins folder

### Using the Plugin

1. **Set IR Directory:**
   - The plugin automatically scans `/Users/justinmitchell/Desktop/KINGS CAB`
   - Or use the folder browser to navigate to your IR collection

2. **Load IRs:**
   - Select a folder from the dropdown
   - Choose specific IR files from the list
   - Up to 6 IRs can be loaded simultaneously

3. **Blend and Shape:**
   - Adjust individual volume levels for each IR
   - Use solo/mute for A/B comparisons
   - Pan IRs for stereo imaging
   - Phase invert for tone shaping

## Technical Specifications

### Audio Processing
- **Sample Rates:** 44.1kHz - 192kHz
- **Bit Depth:** 32-bit floating point
- **Latency:** Ultra-low (buffer-dependent)
- **IR Length:** Up to 4 seconds (192,000 samples at 48kHz)

### Performance
- **CPU Optimized:** SSE/AVX instruction sets where available
- **Memory Efficient:** Smart IR loading and caching
- **Real-time Safe:** Lock-free audio processing

### Supported Formats
- **Plugin Formats:** VST3, AU, Standalone
- **Platforms:** macOS (10.15+), Windows (10+), Linux
- **Architectures:** Intel x64, Apple Silicon (M1/M2)

## IR Collection Compatibility

The King's Cab works with your existing IR collection:

### Supported Formats
- **WAV files** (16-bit, 24-bit, 32-bit)
- **Sample rates:** 44.1kHz - 192kHz
- **Channels:** Mono and Stereo IRs

### Folder Structure
```
KINGS CAB/
├── CUSTOM BLENDS/
│   ├── Excalibur.wav
│   ├── Narsil.wav
│   └── ...
├── MIC BLENDS/
│   ├── 57 2011 CAP EDGE/
│   │   ├── 0.wav
│   │   ├── 5.wav
│   │   └── ...
│   └── C12 2011 CENTER CONE/
└── SINGLE MICS/
    ├── 57 LEFT SPEAKER/
    └── 57 RIGHT SPEAKER/
```

## Interface Overview

### Main Sections

1. **Header** - Plugin title and version
2. **Master Controls** - Global gain and dry/wet mix
3. **IR Slots (6x)** - Individual IR loading and control
4. **Folder Browser** - Navigate IR library
5. **Status Bar** - CPU usage and IR count

### IR Slot Controls

- **Folder Dropdown** - Select IR category
- **IR Dropdown** - Choose specific impulse response
- **Gain Knob** - Volume control (0-200%)
- **Pan Slider** - Stereo positioning
- **Mute Button** - Silence slot
- **Solo Button** - Isolate slot
- **Phase Button** - Invert polarity
- **Clear Button** - Remove IR

## Development

### Architecture

```
src/
├── PluginProcessor.cpp/h     # Main audio processing
├── PluginEditor.cpp/h        # User interface
├── LookAndFeel.cpp/h         # 3D visual styling
├── DSP/
│   ├── ConvolutionEngine.cpp/h   # High-performance convolution
│   └── IRManager.cpp/h           # IR file management
└── Components/
    ├── IRSlot.cpp/h             # Individual IR controls
    └── FolderBrowser.cpp/h      # File navigation
```

### Key Design Principles

- **Performance First** - Lock-free audio processing
- **Professional Quality** - Pristine audio with zero artifacts
- **Intuitive Workflow** - Streamlined interface for live and studio use
- **Scalable Architecture** - Easy to extend and maintain

## License

© 2024 King Studios. All rights reserved.

---

**Ready to experience the ultimate in cabinet simulation?**

*Load your IRs, blend your tones, and discover the sound of royalty.*