#include "ConvolutionEngine.h"

//==============================================================================
ConvolutionEngine::ConvolutionEngine(int numSlots, int maxIRLength)
{
    juce::ignoreUnused(maxIRLength); // For future zero-latency mode implementation
    
    // Initialize IR slots
    irSlots.reserve(numSlots);
    for (int i = 0; i < numSlots; ++i)
    {
        irSlots.push_back(std::make_unique<IRSlot>());
        irSlots[i]->convolution = std::make_unique<juce::dsp::Convolution>();
    }

    // Initialize master smoothers
    masterGainSmoother.setTargetValue(1.0f);
    masterMixSmoother.setTargetValue(1.0f);
}

ConvolutionEngine::~ConvolutionEngine()
{
}

//==============================================================================
void ConvolutionEngine::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;
    currentBlockSize = static_cast<int>(spec.maximumBlockSize);
    numChannels = static_cast<int>(spec.numChannels);

    // Prepare all convolution processors
    for (auto& slot : irSlots)
    {
        slot->convolution->prepare(spec);
        
        // Setup parameter smoothing
        slot->gainSmoother.reset(currentSampleRate, kSmoothingTimeMs / 1000.0);
    }

    // Setup master parameter smoothing
    masterGainSmoother.reset(currentSampleRate, kSmoothingTimeMs / 1000.0);
    masterMixSmoother.reset(currentSampleRate, kSmoothingTimeMs / 1000.0);

    // Prepare processing buffers
    dryBuffer.setSize(numChannels, currentBlockSize);
    wetBuffer.setSize(numChannels, currentBlockSize);
    slotBuffer.setSize(numChannels, currentBlockSize);
}

void ConvolutionEngine::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    auto numSamples = static_cast<int>(inputBlock.getNumSamples());

    // Store dry signal for mixing
    dryBuffer.setSize(numChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        dryBuffer.copyFrom(ch, 0, inputBlock.getChannelPointer(ch), numSamples);
    }

    // Clear wet buffer
    wetBuffer.setSize(numChannels, numSamples, false, false, true);
    wetBuffer.clear();

    // Check if any slots are soloed
    bool hasAnySolo = hasAnySoloedSlots();

    // Process each IR slot
    for (size_t i = 0; i < irSlots.size(); ++i)
    {
        auto& slot = *irSlots[i];
        
        // Skip if no IR loaded
        if (!slot.hasIR.load())
            continue;

        // Skip if muted or if other slots are soloed (and this isn't)
        bool shouldPlay = !slot.muted.load() && (!hasAnySolo || slot.soloed.load());
        if (!shouldPlay)
            continue;

        // Process this slot
        processSlot(static_cast<int>(i), context);
    }

    // Apply master controls
    updateSmoothers();
    
    auto currentMasterGain = masterGainSmoother.getNextValue();
    auto currentMasterMix = masterMixSmoother.getNextValue();

    // Check if any IRs are loaded
    bool hasAnyLoadedIR = false;
    for (const auto& slot : irSlots)
    {
        if (slot->hasIR.load())
        {
            hasAnyLoadedIR = true;
            break;
        }
    }

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* outputData = outputBlock.getChannelPointer(ch);
        auto* dryData = dryBuffer.getReadPointer(ch);
        auto* wetData = wetBuffer.getReadPointer(ch);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float finalSample;
            
            if (hasAnyLoadedIR)
            {
                // Cabinet mode: IRs completely replace the dry signal
                // Mix control blends between full dry (0) and full IR-processed (1)
                auto drySignal = dryData[sample] * (1.0f - currentMasterMix);
                auto irSignal = wetData[sample] * currentMasterMix;
                finalSample = (drySignal + irSignal) * currentMasterGain;
            }
            else
            {
                // Bypass mode: pass through dry signal when no IRs loaded
                finalSample = dryData[sample] * currentMasterGain;
            }
            
            outputData[sample] = finalSample;
            
            // Update smoothers for next sample
            if (sample < numSamples - 1)
            {
                currentMasterGain = masterGainSmoother.getNextValue();
                currentMasterMix = masterMixSmoother.getNextValue();
            }
        }
    }
}

void ConvolutionEngine::reset()
{
    for (auto& slot : irSlots)
    {
        slot->convolution->reset();
        slot->gainSmoother.reset(currentSampleRate, kSmoothingTimeMs / 1000.0);
    }

    masterGainSmoother.reset(currentSampleRate, kSmoothingTimeMs / 1000.0);
    masterMixSmoother.reset(currentSampleRate, kSmoothingTimeMs / 1000.0);

    dryBuffer.clear();
    wetBuffer.clear();
    slotBuffer.clear();
}

//==============================================================================
bool ConvolutionEngine::loadImpulseResponse(int slotIndex, const juce::AudioBuffer<float>& irBuffer)
{
    DBG("=== CONVOLUTION ENGINE loadImpulseResponse START ===");
    DBG("Loading IR for slot " << slotIndex << ", buffer channels: " << irBuffer.getNumChannels() << ", samples: " << irBuffer.getNumSamples());
    
    if (slotIndex < 0 || slotIndex >= static_cast<int>(irSlots.size()))
    {
        DBG("ERROR: Invalid slot index " << slotIndex << " (slots available: " << irSlots.size() << ")");
        return false;
    }

    auto& slot = *irSlots[slotIndex];

    try
    {
        DBG("Creating IR copy for JUCE convolution...");
        // Load IR into convolution processor
        // Create a copy that can be moved (JUCE 7 requires move semantics)
        juce::AudioBuffer<float> irCopy = irBuffer;
        
        DBG("Calling JUCE convolution->loadImpulseResponse...");
        slot.convolution->loadImpulseResponse(
            std::move(irCopy),
            currentSampleRate,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::yes,
            juce::dsp::Convolution::Normalise::yes
        );

        DBG("JUCE convolution loadImpulseResponse completed successfully");
        
        // Force immediate convolution activation by resetting and preparing
        DBG("Forcing convolution reset and prepare...");
        slot.convolution->reset();
        slot.convolution->prepare(juce::dsp::ProcessSpec{currentSampleRate,
                                                         static_cast<juce::uint32>(currentBlockSize),
                                                         static_cast<juce::uint32>(numChannels)});
        
        DBG("Convolution forced to active state");
        slot.hasIR.store(true);
        DBG("=== CONVOLUTION ENGINE loadImpulseResponse SUCCESS ===");
        return true;
    }
    catch (const std::exception& e)
    {
        DBG("ERROR: Exception in convolution->loadImpulseResponse: " << e.what());
        slot.hasIR.store(false);
        DBG("=== CONVOLUTION ENGINE loadImpulseResponse FAILED ===");
        return false;
    }
}

void ConvolutionEngine::clearImpulseResponse(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(irSlots.size()))
        return;

    auto& slot = *irSlots[slotIndex];
    slot.hasIR.store(false);
    slot.convolution->reset();
}

bool ConvolutionEngine::isIRLoaded(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(irSlots.size()))
        return false;

    return irSlots[slotIndex]->hasIR.load();
}

//==============================================================================
void ConvolutionEngine::setSlotGain(int slotIndex, float gain)
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(irSlots.size()))
        return;

    auto& slot = *irSlots[slotIndex];
    slot.gain.store(juce::jmax(kMinGain, gain));
    slot.gainSmoother.setTargetValue(slot.gain.load());
}

void ConvolutionEngine::setSlotMute(int slotIndex, bool muted)
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(irSlots.size()))
        return;

    irSlots[slotIndex]->muted.store(muted);
}

void ConvolutionEngine::setSlotSolo(int slotIndex, bool soloed)
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(irSlots.size()))
        return;

    irSlots[slotIndex]->soloed.store(soloed);
}

void ConvolutionEngine::setSlotPhaseInvert(int slotIndex, bool inverted)
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(irSlots.size()))
        return;

    irSlots[slotIndex]->phaseInverted.store(inverted);
}



void ConvolutionEngine::setMasterGain(float gain)
{
    masterGain.store(juce::jmax(kMinGain, gain));
    masterGainSmoother.setTargetValue(masterGain.load());
}

void ConvolutionEngine::setMasterMix(float mix)
{
    masterMix.store(juce::jlimit(0.0f, 1.0f, mix));
    masterMixSmoother.setTargetValue(masterMix.load());
}

//==============================================================================
void ConvolutionEngine::updateSmoothers()
{
    // Update master smoothers if they're not at target
    if (!masterGainSmoother.isSmoothing())
        masterGainSmoother.setTargetValue(masterGain.load());
    
    if (!masterMixSmoother.isSmoothing())
        masterMixSmoother.setTargetValue(masterMix.load());

    // Update slot smoothers
    for (auto& slot : irSlots)
    {
        if (!slot->gainSmoother.isSmoothing())
            slot->gainSmoother.setTargetValue(slot->gain.load());
    }
}

bool ConvolutionEngine::hasAnySoloedSlots() const
{
    for (const auto& slot : irSlots)
    {
        if (slot->soloed.load() && slot->hasIR.load())
            return true;
    }
    return false;
}

void ConvolutionEngine::processSlot(int slotIndex, const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& slot = *irSlots[slotIndex];
    auto& inputBlock = context.getInputBlock();
    auto numSamples = static_cast<int>(inputBlock.getNumSamples());

    // Copy input to slot buffer
    slotBuffer.setSize(numChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        slotBuffer.copyFrom(ch, 0, inputBlock.getChannelPointer(ch), numSamples);
    }

    // Process through convolution
    juce::dsp::AudioBlock<float> slotBlock(slotBuffer);
    juce::dsp::ProcessContextReplacing<float> slotContext(slotBlock);
    slot.convolution->process(slotContext);

    // Apply slot controls
    auto currentGain = slot.gainSmoother.getNextValue();
    bool phaseInvert = slot.phaseInverted.load();

    // Apply gain and phase
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float processedSample0 = slotBuffer.getSample(0, sample) * currentGain;
        if (phaseInvert) processedSample0 = -processedSample0;
        
        wetBuffer.addSample(0, sample, processedSample0);

        if (numChannels >= 2)
        {
            float processedSample1 = slotBuffer.getSample(1, sample) * currentGain;
            if (phaseInvert) processedSample1 = -processedSample1;
            
            wetBuffer.addSample(1, sample, processedSample1);
        }

        // Update smoothers for next sample
        if (sample < numSamples - 1)
        {
            currentGain = slot.gainSmoother.getNextValue();
        }
    }
}
