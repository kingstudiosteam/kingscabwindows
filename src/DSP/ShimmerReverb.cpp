#include "ShimmerReverb.h"
#include <algorithm>
#include <cmath>

//==============================================================================
ShimmerReverb::ShimmerReverb()
{
    // Initialize modulation oscillator with sine wave
    modOscillator.initialise([](float x) { return std::sin(x); });
    
    // Initialize grains
    grains.resize(MAX_GRAINS);
    for (auto& grain : grains)
    {
        grain.buffer.resize(GRAIN_LENGTH, 0.0f);
    }
}

//==============================================================================
void ShimmerReverb::prepare(double newSampleRate, int newBlockSize)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;

    // Prepare JUCE DSP modules
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = 2; // Stereo

    lowCutFilter.prepare(spec);
    highCutFilter.prepare(spec);
    modOscillator.prepare(spec);

    // Setup delay lines based on current mode
    setupDelayLines();

    // Prepare buffers
    feedbackBuffer.setSize(2, blockSize);
    pitchBuffer.setSize(2, blockSize);
    tempBuffer.setSize(2, blockSize);
    grainBuffer.setSize(2, blockSize);

    // Update filters and modulation
    updateFilters();
    updateModulation();

    // Reset state
    reset();
}

void ShimmerReverb::reset()
{
    // Clear delay lines
    for (auto& delayLine : delayLines)
    {
        std::fill(delayLine.buffer.begin(), delayLine.buffer.end(), 0.0f);
        delayLine.writeIndex = 0;
        delayLine.readIndex = 0;
        delayLine.modPhase = 0.0f;
    }

    // Clear grains
    for (auto& grain : grains)
    {
        std::fill(grain.buffer.begin(), grain.buffer.end(), 0.0f);
        grain.position = 0;
        grain.length = 0;
        grain.envelope = 0.0f;
        grain.envelopeStep = 0.0f;
        grain.active = false;
    }

    // Clear buffers
    feedbackBuffer.clear();
    pitchBuffer.clear();
    tempBuffer.clear();
    grainBuffer.clear();

    // Reset filters and oscillators
    lowCutFilter.reset();
    highCutFilter.reset();
    modOscillator.reset();
    modPhase = 0.0f;
}

void ShimmerReverb::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels < 1 || numSamples <= 0)
        return;

    // Store input for feedback processing
    tempBuffer.makeCopyOf(buffer);

    // Apply tone shaping to input
    applyToneShaping(tempBuffer);

    // Process pitch shifting in feedback loop
    if (pitchMode.load() != PitchMode::Bypass)
    {
        processPitchShifting(tempBuffer);
    }

    // Process through reverb
    processReverb(tempBuffer);

    // Copy result back to input buffer
    buffer.makeCopyOf(tempBuffer);
}

//==============================================================================
void ShimmerReverb::setFeedback(float newFeedback) noexcept
{
    feedback.store(juce::jlimit(MIN_FEEDBACK, MAX_FEEDBACK, newFeedback));
}

void ShimmerReverb::setSize(float newSize) noexcept
{
    size.store(juce::jlimit(MIN_SIZE, MAX_SIZE, newSize));
    setupDelayLines();
}

void ShimmerReverb::setDiffusion(float newDiffusion) noexcept
{
    diffusion.store(juce::jlimit(MIN_DIFFUSION, MAX_DIFFUSION, newDiffusion));
}

void ShimmerReverb::setModulationRate(float rate) noexcept
{
    modRate.store(juce::jlimit(MIN_MOD_RATE, MAX_MOD_RATE, rate));
    updateModulation();
}

void ShimmerReverb::setModulationDepth(float depth) noexcept
{
    modDepth.store(juce::jlimit(MIN_MOD_DEPTH, MAX_MOD_DEPTH, depth));
}

void ShimmerReverb::setPitchShift(float semitones) noexcept
{
    pitchShift.store(juce::jlimit(MIN_SHIFT, MAX_SHIFT, semitones));
}

void ShimmerReverb::setPitchMode(int mode) noexcept
{
    if (mode >= 0 && mode <= 4)
    {
        pitchMode.store(static_cast<PitchMode>(mode));
    }
}

void ShimmerReverb::setReverbMode(int mode) noexcept
{
    if (mode >= 0 && mode <= 3)
    {
        reverbMode.store(static_cast<ReverbMode>(mode));
        setupDelayLines();
    }
}

void ShimmerReverb::setColorMode(int mode) noexcept
{
    if (mode >= 0 && mode <= 1)
    {
        colorMode.store(static_cast<ColorMode>(mode));
        updateFilters();
    }
}

void ShimmerReverb::setHighCut(float frequency) noexcept
{
    highCut.store(juce::jlimit(MIN_HIGH_CUT, MAX_HIGH_CUT, frequency));
    updateFilters();
}

void ShimmerReverb::setLowCut(float frequency) noexcept
{
    lowCut.store(juce::jlimit(MIN_LOW_CUT, MAX_LOW_CUT, frequency));
    updateFilters();
}

//==============================================================================
void ShimmerReverb::setupDelayLines()
{
    delayLines.clear();

    // Get delay times based on current mode
    const size_t* delayTimes = nullptr;
    size_t numDelays = 0;

    switch (reverbMode.load())
    {
        case ReverbMode::Mono:
            delayTimes = MONO_DELAY_TIMES;
            numDelays = sizeof(MONO_DELAY_TIMES) / sizeof(MONO_DELAY_TIMES[0]);
            break;
        case ReverbMode::SmallStereo:
            delayTimes = SMALL_STEREO_DELAY_TIMES;
            numDelays = sizeof(SMALL_STEREO_DELAY_TIMES) / sizeof(SMALL_STEREO_DELAY_TIMES[0]);
            break;
        case ReverbMode::MediumStereo:
            delayTimes = MEDIUM_STEREO_DELAY_TIMES;
            numDelays = sizeof(MEDIUM_STEREO_DELAY_TIMES) / sizeof(MEDIUM_STEREO_DELAY_TIMES[0]);
            break;
        case ReverbMode::BigStereo:
            delayTimes = BIG_STEREO_DELAY_TIMES;
            numDelays = sizeof(BIG_STEREO_DELAY_TIMES) / sizeof(BIG_STEREO_DELAY_TIMES[0]);
            break;
    }

    // Create delay lines
    for (size_t i = 0; i < numDelays; ++i)
    {
        DelayLine delayLine;
        
        // Scale delay time by size parameter
        size_t scaledDelayTime = static_cast<size_t>(delayTimes[i] * size.load());
        delayLine.delayTime = juce::jlimit(static_cast<size_t>(1), MAX_DELAY_TIME, scaledDelayTime);
        
        // Initialize buffer
        delayLine.buffer.resize(delayLine.delayTime, 0.0f);
        delayLine.writeIndex = 0;
        delayLine.readIndex = 0;
        delayLine.feedback = feedback.load();
        delayLine.diffusion = diffusion.load();
        delayLine.modRate = modRate.load();
        delayLine.modPhase = 0.0f;

        delayLines.push_back(delayLine);
    }
}

void ShimmerReverb::updateModulation()
{
    modOscillator.setFrequency(modRate.load());
}

void ShimmerReverb::updateFilters()
{
    // Update low cut filter
    auto lowCutCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, lowCut.load());
    lowCutFilter.coefficients = lowCutCoeffs;

    // Update high cut filter
    auto highCutCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, highCut.load());
    highCutFilter.coefficients = highCutCoeffs;
}

void ShimmerReverb::processPitchShifting(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Clear pitch buffer
    pitchBuffer.clear();
    
    // Copy input to pitch buffer
    pitchBuffer.makeCopyOf(buffer);
    
    // Apply grain-based pitch shifting
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = pitchBuffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Simple pitch shifting using time stretching
            float pitchRatio = std::pow(2.0f, pitchShift.load() / 12.0f);
            
            // Calculate read position with pitch shifting
            float readPos = static_cast<float>(sample) / pitchRatio;
            int readIndex = static_cast<int>(readPos);
            
            if (readIndex >= 0 && readIndex < numSamples)
            {
                // Linear interpolation
                float frac = readPos - static_cast<float>(readIndex);
                float nextSample = (readIndex + 1 < numSamples) ? channelData[readIndex + 1] : channelData[readIndex];
                channelData[sample] = channelData[readIndex] * (1.0f - frac) + nextSample * frac;
            }
        }
    }
    
    // Mix pitch-shifted signal back to original buffer
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* originalChannel = buffer.getWritePointer(channel);
        auto* pitchChannel = pitchBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Mix original and pitch-shifted signals based on mode
            switch (pitchMode.load())
            {
                case PitchMode::Single:
                    originalChannel[sample] = 0.5f * (originalChannel[sample] + pitchChannel[sample]);
                    break;
                case PitchMode::Dual:
                    originalChannel[sample] = 0.33f * (originalChannel[sample] + pitchChannel[sample] + 
                                                      pitchChannel[sample] * 0.5f);
                    break;
                case PitchMode::SingleReverse:
                    originalChannel[sample] = 0.5f * (originalChannel[sample] + pitchChannel[sample]);
                    break;
                case PitchMode::DualReverse:
                    originalChannel[sample] = 0.33f * (originalChannel[sample] + pitchChannel[sample] + 
                                                      pitchChannel[sample] * 0.5f);
                    break;
                default:
                    break;
            }
        }
    }
}

void ShimmerReverb::applyToneShaping(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        // Apply low cut filter
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = lowCutFilter.processSample(channelData[sample]);
        }

        // Apply high cut filter
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = highCutFilter.processSample(channelData[sample]);
        }
    }
}

void ShimmerReverb::processReverb(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Clear feedback buffer
    feedbackBuffer.clear();

    // Process through delay lines
    for (auto& delayLine : delayLines)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* inputChannel = buffer.getReadPointer(channel);
            auto* feedbackChannel = feedbackBuffer.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                // Read from delay line
                float delayedSample = delayLine.buffer[delayLine.readIndex];

                // Apply modulation to read position
                if (modDepth.load() > 0.0f)
                {
                    float modValue = modDepth.load() * modOscillator.processSample(0.0f);
                    size_t modOffset = static_cast<size_t>(modValue * 10.0f);
                    size_t modulatedReadIndex = (delayLine.readIndex + modOffset) % delayLine.buffer.size();
                    delayedSample = delayLine.buffer[modulatedReadIndex];
                }

                // Mix input with feedback
                float input = inputChannel[sample];
                float feedback = delayedSample * delayLine.feedback;
                float output = input + feedback;

                // Write to delay line
                delayLine.buffer[delayLine.writeIndex] = output;

                // Add to feedback buffer
                feedbackChannel[sample] += delayedSample * (1.0f - delayLine.feedback);

                // Update indices
                delayLine.writeIndex = (delayLine.writeIndex + 1) % delayLine.buffer.size();
                delayLine.readIndex = (delayLine.readIndex + 1) % delayLine.buffer.size();
            }
        }
    }

    // Apply diffusion
    if (diffusion.load() > 0.0f)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = feedbackBuffer.getWritePointer(channel);
            
            float prevSample = 0.0f;
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float currentSample = channelData[sample];
                float diffusedSample = currentSample + prevSample * diffusion.load();
                channelData[sample] = diffusedSample;
                prevSample = currentSample;
            }
        }
    }

    // Mix feedback back to output
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* outputChannel = buffer.getWritePointer(channel);
        auto* feedbackChannel = feedbackBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            outputChannel[sample] = feedbackChannel[sample] / static_cast<float>(delayLines.size());
        }
    }
}
