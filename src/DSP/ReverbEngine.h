#pragma once

#include <JuceHeader.h>

/**
 * @brief High-quality algorithmic reverb engine for shimmer effects
 * 
 * Implements a professional reverb algorithm with multiple room modes,
 * modulation, and tone shaping capabilities inspired by classic hardware reverbs.
 */
class ReverbEngine
{
public:
    //==============================================================================
    // Reverb modes matching Valhalla Shimmer
    enum class ReverbMode
    {
        Mono = 0,           // Large cathedral spaces
        SmallStereo = 1,    // Small rooms
        MediumStereo = 2,   // Medium halls
        BigStereo = 3       // Large halls
    };

    // Color modes for tone shaping
    enum class ColorMode
    {
        Bright = 0,         // Full bandwidth, modern
        Dark = 1            // Vintage, high frequency loss
    };

    //==============================================================================
    ReverbEngine();
    ~ReverbEngine() = default;

    //==============================================================================
    /** Prepare the reverb engine for audio processing */
    void prepare(double sampleRate, int samplesPerBlock);

    /** Reset all internal state */
    void reset();

    /** Process audio through the reverb */
    void process(juce::AudioBuffer<float>& buffer);

    //==============================================================================
    // Parameter setters
    void setFeedback(float feedback) noexcept;
    void setSize(float size) noexcept;
    void setDiffusion(float diffusion) noexcept;
    void setModulationRate(float rate) noexcept;
    void setModulationDepth(float depth) noexcept;
    void setReverbMode(int mode) noexcept;
    void setColorMode(int mode) noexcept;

    //==============================================================================
    // Parameter getters
    float getFeedback() const noexcept { return feedback; }
    float getSize() const noexcept { return size; }
    float getDiffusion() const noexcept { return diffusion; }
    float getModulationRate() const noexcept { return modRate; }
    float getModulationDepth() const noexcept { return modDepth; }
    ReverbMode getReverbMode() const noexcept { return reverbMode; }
    ColorMode getColorMode() const noexcept { return colorMode; }

private:
    //==============================================================================
    // Delay line structure for reverb
    struct DelayLine
    {
        std::vector<float> buffer;
        size_t writeIndex{0};
        size_t readIndex{0};
        size_t delayTime{0};
        float feedback{0.0f};
        float diffusion{0.0f};
        float modulation{0.0f};
        float modPhase{0.0f};
        float modRate{1.0f};
    };

    //==============================================================================
    // Reverb parameters
    std::atomic<float> feedback{0.5f};
    std::atomic<float> size{0.7f};
    std::atomic<float> diffusion{0.5f};
    std::atomic<float> modRate{1.0f};
    std::atomic<float> modDepth{0.3f};
    std::atomic<ReverbMode> reverbMode{ReverbMode::BigStereo};
    std::atomic<ColorMode> colorMode{ColorMode::Bright};

    //==============================================================================
    // Internal state
    double sampleRate{44100.0};
    int blockSize{512};
    
    // Delay lines for different reverb modes
    std::vector<DelayLine> delayLines;
    
    // Modulation oscillators
    std::vector<juce::dsp::Oscillator<float>> modOscillators;
    
    // Filters for tone shaping
    juce::dsp::IIR::Filter<float> toneFilter;
    
    // Buffers for processing
    juce::AudioBuffer<float> tempBuffer;
    juce::AudioBuffer<float> outputBuffer;

    //==============================================================================
    // Constants for different reverb modes
    static constexpr size_t MAX_DELAY_LINES = 16;
    static constexpr size_t MAX_DELAY_TIME = 65536; // Maximum delay time in samples
    
    // Delay times for different modes (in samples)
    static constexpr size_t MONO_DELAY_TIMES[] = { 1764, 2114, 2352, 2646, 3136, 3528, 3969, 4410 };
    static constexpr size_t SMALL_STEREO_DELAY_TIMES[] = { 882, 1057, 1176, 1323, 1568, 1764, 1984, 2205 };
    static constexpr size_t MEDIUM_STEREO_DELAY_TIMES[] = { 1323, 1587, 1764, 1984, 2352, 2646, 2977, 3307 };
    static constexpr size_t BIG_STEREO_DELAY_TIMES[] = { 2205, 2646, 2940, 3307, 3920, 4410, 4961, 5512 };

    //==============================================================================
    // Helper methods
    void setupDelayLines();
    void updateModulation();
    void updateToneFilter();
    void processDelayLine(DelayLine& delayLine, float input, float& output);
    void applyDiffusion(juce::AudioBuffer<float>& buffer);
    void applyModulation(juce::AudioBuffer<float>& buffer);
    void applyToneShaping(juce::AudioBuffer<float>& buffer);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbEngine)
};
