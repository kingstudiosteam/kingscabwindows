#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>
#include <cmath>

/**
 * @brief Professional shimmer reverb with pitch shifting in feedback loop
 * 
 * Implements a high-quality shimmer reverb inspired by Valhalla Shimmer,
 * featuring pitch shifting in the feedback loop, multiple delay lines,
 * modulation, and tone shaping for ethereal, atmospheric effects.
 */
class ShimmerReverb
{
public:
    ShimmerReverb();
    ~ShimmerReverb() = default;

    //==============================================================================
    /** Prepare the reverb for processing */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Process audio block */
    void processBlock(juce::AudioBuffer<float>& buffer);

    /** Reset all internal state */
    void reset();

    //==============================================================================
    // Parameter setters
    void setDryWet(float dryWet) { dryWetParam.store(dryWet); }
    void setFeedback(float feedback) { feedbackParam.store(juce::jlimit(0.0f, 0.98f, feedback)); }
    void setSize(float size) { sizeParam.store(size); updateDelayTimes(); }
    void setDiffusion(float diffusion) { diffusionParam.store(diffusion); }
    void setModRate(float modRate) { modRateParam.store(modRate); }
    void setModDepth(float modDepth) { modDepthParam.store(modDepth); }
    void setPitchShift(float semitones) { pitchShiftParam.store(semitones); }
    void setHighCut(float freq) { highCutParam.store(freq); updateFilters(); }
    void setLowCut(float freq) { lowCutParam.store(freq); updateFilters(); }
    void setPitchMode(int mode) { pitchModeParam.store(mode); }
    void setReverbMode(int mode) { reverbModeParam.store(mode); updateDelayTimes(); }
    void setColorMode(int mode) { colorModeParam.store(mode); updateFilters(); }

private:
    //==============================================================================
    // Internal processing methods
    void updateDelayTimes();
    void updateFilters();
    void updateModulation();
    void processPitchShifting(float* channelData, int numSamples);
    void processReverb(float* channelData, int numSamples);
    void applyToneShaping(float* channelData, int numSamples);

    //==============================================================================
    // Delay line structure
    struct DelayLine
    {
        std::vector<float> buffer;
        int writeIndex = 0;
        int readIndex = 0;
        int delayTime = 0;
        float feedback = 0.0f;
        float modulation = 0.0f;
        float modPhase = 0.0f;
        float modIncrement = 0.0f;
    };

    // Pitch shifter structure
    struct PitchShifter
    {
        std::vector<float> buffer;
        int writeIndex = 0;
        float readIndex = 0.0f;
        float pitchRatio = 1.0f;
        float windowSize = 1024.0f;
        float overlap = 4.0f;
    };

    //==============================================================================
    // Parameters (atomic for thread safety)
    std::atomic<float> dryWetParam{0.5f};
    std::atomic<float> feedbackParam{0.7f};
    std::atomic<float> sizeParam{0.5f};
    std::atomic<float> diffusionParam{0.5f};
    std::atomic<float> modRateParam{0.5f};
    std::atomic<float> modDepthParam{0.3f};
    std::atomic<float> pitchShiftParam{12.0f};
    std::atomic<float> highCutParam{8000.0f};
    std::atomic<float> lowCutParam{100.0f};
    std::atomic<int> pitchModeParam{1};
    std::atomic<int> reverbModeParam{2};
    std::atomic<int> colorModeParam{1};

    //==============================================================================
    // Internal state
    double sampleRate = 44100.0;
    int maxBlockSize = 512;
    int numChannels = 2;

    // Delay lines for reverb
    std::vector<DelayLine> delayLines;
    std::vector<DelayLine> pitchShiftDelayLines;

    // Pitch shifters
    std::vector<PitchShifter> pitchShifters;

    // Filters
    juce::dsp::IIR::Filter<float> lowCutFilter;
    juce::dsp::IIR::Filter<float> highCutFilter;
    juce::dsp::IIR::Filter<float> preFilter;
    juce::dsp::IIR::Filter<float> postFilter;

    // Modulation
    float modPhase = 0.0f;
    float modIncrement = 0.0f;

    // Buffers
    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> wetBuffer;
    juce::AudioBuffer<float> tempBuffer;

    //==============================================================================
    // Constants
    static constexpr int MAX_DELAY_LINES = 8;
    static constexpr int MAX_DELAY_TIME_MS = 2000;
    static constexpr int PITCH_BUFFER_SIZE = 8192;
    static constexpr float MAX_FEEDBACK = 0.98f;

    // Delay time arrays for different reverb modes
    static constexpr float MONO_DELAY_TIMES[] = {29.7f, 37.1f, 41.1f, 43.7f, 47.3f, 53.1f, 59.3f, 61.7f};
    static constexpr float SMALL_STEREO_DELAY_TIMES[] = {29.7f, 37.1f, 41.1f, 43.7f, 47.3f, 53.1f, 59.3f, 61.7f};
    static constexpr float MEDIUM_STEREO_DELAY_TIMES[] = {31.7f, 37.9f, 41.9f, 43.9f, 47.9f, 53.9f, 59.9f, 61.9f};
    static constexpr float BIG_STEREO_DELAY_TIMES[] = {33.7f, 38.9f, 42.9f, 44.9f, 48.9f, 54.9f, 60.9f, 62.9f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShimmerReverb)
};
