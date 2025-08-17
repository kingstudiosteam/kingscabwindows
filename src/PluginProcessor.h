#pragma once

#include <JuceHeader.h>
#include "DSP/ConvolutionEngine.h"
#include "DSP/IRManager.h"

//==============================================================================
/**
 * The King's Cab - Professional Impulse Response Loader
 * 
 * High-performance VST3 plugin for guitar cabinet simulation with 6 IR slots.
 * Optimized for low CPU usage and professional audio quality.
 */
class TheKingsCabAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    TheKingsCabAudioProcessor();
    ~TheKingsCabAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // IR Management
    void loadImpulseResponse(int slotIndex, const juce::File& irFile);
    void clearImpulseResponse(int slotIndex);
    IRManager& getIRManager() { return irManager; }
    ConvolutionEngine& getConvolutionEngine() { return convolutionEngine; }
    
    // Parameter access
    juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }

    // Constants
    static constexpr int kNumIRSlots = 6;
    static constexpr int kMaxIRLength = 192000; // 4 seconds at 48kHz

private:
    //==============================================================================
    // Parameter creation helper
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Core components
    juce::AudioProcessorValueTreeState valueTreeState;
    ConvolutionEngine convolutionEngine;
    IRManager irManager;

    // Performance monitoring
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheKingsCabAudioProcessor)
};