#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>

//==============================================================================
/**
 * High-performance convolution engine optimized for guitar IR processing.
 * 
 * Features:
 * - Lock-free processing for real-time performance
 * - Multiple IR slot management with individual controls
 * - Optimized for low CPU usage and minimal latency
 * - Thread-safe IR loading and unloading
 */
class ConvolutionEngine
{
public:
    //==============================================================================
    ConvolutionEngine(int numSlots, int maxIRLength);
    ~ConvolutionEngine();

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(const juce::dsp::ProcessContextReplacing<float>& context);
    void reset();

    //==============================================================================
    // IR Management
    bool loadImpulseResponse(int slotIndex, const juce::AudioBuffer<float>& irBuffer);
    void clearImpulseResponse(int slotIndex);
    bool isIRLoaded(int slotIndex) const;

    //==============================================================================
    // Real-time parameter control (thread-safe)
    void setSlotGain(int slotIndex, float gain);
    void setSlotMute(int slotIndex, bool muted);
    void setSlotSolo(int slotIndex, bool soloed);
    void setSlotPhaseInvert(int slotIndex, bool inverted);


    void setMasterGain(float gain);
    void setMasterMix(float mix);

private:
    //==============================================================================
    struct IRSlot
    {
        std::unique_ptr<juce::dsp::Convolution> convolution;
        std::atomic<float> gain{ 1.0f };
        std::atomic<bool> muted{ false };
        std::atomic<bool> soloed{ false };
        std::atomic<bool> phaseInverted{ false };
        std::atomic<bool> hasIR{ false };
        
        // Smoothed parameters for click-free operation
        juce::LinearSmoothedValue<float> gainSmoother;
        
        IRSlot() 
        {
            gainSmoother.setTargetValue(1.0f);
        }
    };

    //==============================================================================
    // Core components
    std::vector<std::unique_ptr<IRSlot>> irSlots;
    
    // Master controls
    std::atomic<float> masterGain{ 1.0f };
    std::atomic<float> masterMix{ 1.0f };
    juce::LinearSmoothedValue<float> masterGainSmoother;
    juce::LinearSmoothedValue<float> masterMixSmoother;

    // Processing buffers for efficiency
    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> wetBuffer;
    juce::AudioBuffer<float> slotBuffer;

    // Audio format settings
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    int numChannels = 2;

    // Performance constants
    static constexpr float kSmoothingTimeMs = 20.0f;
    static constexpr float kMinGain = 0.0001f; // -80dB
    
    //==============================================================================
    // Helper methods
    void updateSmoothers();
    bool hasAnySoloedSlots() const;
    void processSlot(int slotIndex, const juce::dsp::ProcessContextReplacing<float>& context);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvolutionEngine)
};
