#pragma once

#include <JuceHeader.h>

/**
 * @brief High-quality pitch shifter for shimmer reverb effects
 * 
 * Implements various pitch shifting algorithms including grain-based shifting,
 * reverse processing, and dual shifting modes inspired by classic shimmer effects.
 */
class PitchShifter
{
public:
    //==============================================================================
    // Pitch shift modes matching Valhalla Shimmer
    enum class PitchMode
    {
        Single = 0,        // Classic shimmer sound
        Dual = 1,          // Rich harmonic texture
        SingleReverse = 2, // Smooth, organ-like
        DualReverse = 3,   // Pipe organ sound
        Bypass = 4         // Standard reverb
    };

    //==============================================================================
    // Grain-based pitch shifting parameters
    static constexpr size_t MIN_GRAIN_SIZE = 256;
    static constexpr size_t MAX_GRAIN_SIZE = 2048;
    static constexpr size_t DEFAULT_GRAIN_SIZE = 512;
    static constexpr float MIN_PITCH_SHIFT = -24.0f;
    static constexpr float MAX_PITCH_SHIFT = 24.0f;
    static constexpr float CROSSFADE_RATIO = 0.5f;

    //==============================================================================
    PitchShifter();
    ~PitchShifter() = default;

    //==============================================================================
    /** Prepare the pitch shifter for audio processing */
    void prepare(double sampleRate, int samplesPerBlock);

    /** Reset all internal state */
    void reset();

    /** Process audio through the pitch shifter */
    void process(juce::AudioBuffer<float>& buffer);

    //==============================================================================
    // Parameter setters
    void setPitchShift(float semitones) noexcept;
    void setMode(int mode) noexcept;
    void setGrainSize(size_t samples) noexcept;

    //==============================================================================
    // Parameter getters
    float getPitchShift() const noexcept { return pitchShift; }
    PitchMode getMode() const noexcept { return pitchMode; }
    size_t getGrainSize() const noexcept { return grainSize; }

private:
    //==============================================================================
    // Grain structure for pitch shifting
    struct Grain
    {
        std::vector<float> buffer;
        size_t position{0};
        size_t length{0};
        float envelope{0.0f};
        bool active{false};
        float pitchRatio{1.0f};
        bool reversed{false};
    };

    //==============================================================================
    // Pitch shifter parameters
    std::atomic<float> pitchShift{12.0f};
    std::atomic<PitchMode> pitchMode{PitchMode::Single};
    std::atomic<size_t> grainSize{DEFAULT_GRAIN_SIZE};

    //==============================================================================
    // Internal state
    double sampleRate{44100.0};
    int blockSize{512};
    
    // Grain buffers and management
    std::vector<Grain> grains;
    juce::AudioBuffer<float> inputBuffer;
    juce::AudioBuffer<float> outputBuffer;
    juce::AudioBuffer<float> tempBuffer;
    
    // Grain scheduling
    size_t grainCounter{0};
    size_t grainInterval{0};
    
    // Envelope generation
    std::vector<float> envelopeTable;
    
    //==============================================================================
    // Helper methods
    void setupGrains();
    void updateGrainParameters();
    void processGrains(juce::AudioBuffer<float>& buffer);
    void generateEnvelopeTable();
    void scheduleGrains();
    void processGrain(Grain& grain, const juce::AudioBuffer<float>& input, juce::AudioBuffer<float>& output);
    float getEnvelopeValue(float position) const;
    void reverseGrain(Grain& grain);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchShifter)
};
