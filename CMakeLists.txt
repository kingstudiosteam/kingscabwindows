cmake_minimum_required(VERSION 3.24)
project(TheKingsCab VERSION 1.0.0 LANGUAGES C CXX)

# Modern C++20 with strict standards for maximum performance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific warning flags for zero warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# JUCE configuration for optimal performance
include(FetchContent)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)
set(JUCE_FETCH_VST3_SDK ON CACHE BOOL "" FORCE)

# Fetch JUCE 7+ for latest features and performance optimizations
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(juce)

# Plugin configuration for The King's Cab IR Loader
juce_add_plugin(TheKingsCab
  COMPANY_NAME "King Studios"
  PLUGIN_MANUFACTURER "King Studios"
  PLUGIN_MANUFACTURER_CODE KngS
  PLUGIN_CODE KCab
  BUNDLE_ID com.kingstudios.thekingscab
  PRODUCT_NAME "The Kings Cab"
  DESCRIPTION "Professional impulse response loader with 6-slot cabinet simulation"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  FORMATS VST3 AU AAX Standalone
  AAX_IDENTIFIER com.kingstudios.thekingscab
)

juce_generate_juce_header(TheKingsCab)

# Source files organized for clean, scalable architecture
target_sources(TheKingsCab PRIVATE
  src/PluginProcessor.cpp
  src/PluginEditor.cpp
  src/LookAndFeel.cpp
  src/DSP/ConvolutionEngine.cpp
  src/DSP/IRManager.cpp
  src/Components/IRSlot.cpp
)

# Performance and security optimizations for IR loading
target_compile_definitions(TheKingsCab PRIVATE
  # Disable unnecessary JUCE features for lean binary
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_PLUGINHOST_VST=0
  JUCE_PLUGINHOST_VST3=0
  JUCE_PLUGINHOST_AU=0
  JUCE_JACK=0
  JUCE_ALSA=0
  
  # Audio format support for IR files
  JUCE_USE_FLAC=1
  JUCE_USE_OGGVORBIS=0
  JUCE_USE_MP3AUDIOFORMAT=0
  JUCE_USE_LAME_AUDIO_FORMAT=0
  JUCE_USE_WINDOWS_MEDIA_FORMAT=1
  
  # Modern C++ features for performance
  JUCE_MODAL_LOOPS_PERMITTED=0
  JUCE_STRICT_REFCOUNTEDPOINTER=1
  
  # IR Loader specific optimizations
  JUCE_DSP_USE_INTEL_MKL=0
  JUCE_DSP_USE_SHARED_FFTW=0
  JUCE_DSP_ENABLE_SNAP_TO_ZERO=1
)

# Essential JUCE modules for IR processing and UI
target_link_libraries(TheKingsCab PRIVATE
  juce::juce_audio_utils
  juce::juce_audio_plugin_client
  juce::juce_dsp
  juce::juce_gui_basics
  juce::juce_core
  juce::juce_graphics
)

# Aggressive optimization flags for low CPU usage
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(TheKingsCab PRIVATE /O2 /Ob2 /Oi /Ot /GS- /fp:fast)
    else()
        target_compile_options(TheKingsCab PRIVATE -O3 -ffast-math -march=native -mtune=native)
    endif()
endif()

# Linker optimizations for smaller binary size
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(APPLE)
        target_link_options(TheKingsCab PRIVATE -Wl,-dead_strip)
    elseif(UNIX)
        target_link_options(TheKingsCab PRIVATE -Wl,--gc-sections)
    endif()
endif()