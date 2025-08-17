#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheKingsCabAudioProcessor::TheKingsCabAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
      valueTreeState(*this, nullptr, "PARAMETERS", createParameterLayout()),
      convolutionEngine(kNumIRSlots, kMaxIRLength),
      irManager()
{
    // Initialize IR manager with robust Windows/macOS paths
    // Preferred location: %APPDATA%/King Studios/The King's Cab/IR_Collections (Windows)
    // Fallbacks: relative to plugin/executable, then current working directory
    juce::File appDataIR = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                               .getChildFile("King Studios")
                               .getChildFile("The King's Cab")
                               .getChildFile("IR_Collections");

    // Try to locate IRs relative to the plugin binary as a fallback
    juce::File exeDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
    juce::File pluginRoot = exeDir;
    // Climb a few levels in case of bundle-like layout (safe on Windows too)
    for (int i = 0; i < 3; ++i)
        pluginRoot = pluginRoot.getParentDirectory();
    juce::File exeRelativeIR = pluginRoot.getChildFile("IR_Collections");

    juce::File cwdIR = juce::File::getCurrentWorkingDirectory().getChildFile("IR_Collections");

    juce::File chosenIRDir;
    if (appDataIR.exists() && appDataIR.isDirectory())
        chosenIRDir = appDataIR;
    else if (exeRelativeIR.exists() && exeRelativeIR.isDirectory())
        chosenIRDir = exeRelativeIR;
    else if (cwdIR.exists() && cwdIR.isDirectory())
        chosenIRDir = cwdIR;
    else
        chosenIRDir = appDataIR; // default target where installer places IRs

    irManager.setIRDirectory(chosenIRDir);
}

TheKingsCabAudioProcessor::~TheKingsCabAudioProcessor()
{
}

//==============================================================================
const juce::String TheKingsCabAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TheKingsCabAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TheKingsCabAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TheKingsCabAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TheKingsCabAudioProcessor::getTailLengthSeconds() const
{
    return 4.0; // Maximum IR length
}

int TheKingsCabAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TheKingsCabAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TheKingsCabAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String TheKingsCabAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void TheKingsCabAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void TheKingsCabAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Prepare the convolution engine with current audio settings
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    convolutionEngine.prepare(spec);
}

void TheKingsCabAudioProcessor::releaseResources()
{
    convolutionEngine.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TheKingsCabAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TheKingsCabAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Create audio block for DSP processing
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Update DSP engine with current parameter values
    // Master controls
    if (auto* masterGainParam = valueTreeState.getRawParameterValue("master_gain"))
        convolutionEngine.setMasterGain(masterGainParam->load());
    if (auto* masterMixParam = valueTreeState.getRawParameterValue("master_mix"))
        convolutionEngine.setMasterMix(masterMixParam->load());

    // Per-slot controls
    for (int i = 0; i < kNumIRSlots; ++i)
    {
        auto slotPrefix = juce::String("slot") + juce::String(i) + "_";

        if (auto* gainParam = valueTreeState.getRawParameterValue(slotPrefix + "gain"))
            convolutionEngine.setSlotGain(i, gainParam->load());

        if (auto* muteParam = valueTreeState.getRawParameterValue(slotPrefix + "mute"))
            convolutionEngine.setSlotMute(i, muteParam->load() >= 0.5f);

        if (auto* soloParam = valueTreeState.getRawParameterValue(slotPrefix + "solo"))
            convolutionEngine.setSlotSolo(i, soloParam->load() >= 0.5f);

        if (auto* phaseParam = valueTreeState.getRawParameterValue(slotPrefix + "phase"))
            convolutionEngine.setSlotPhaseInvert(i, phaseParam->load() >= 0.5f);
    }

    // Process through convolution engine
    convolutionEngine.process(context);
}

//==============================================================================
bool TheKingsCabAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TheKingsCabAudioProcessor::createEditor()
{
    return new TheKingsCabAudioProcessorEditor (*this);
}

//==============================================================================
void TheKingsCabAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save the plugin state including loaded IRs and parameter values
    auto state = valueTreeState.copyState();
    
    // Add IR file paths to state
    auto irState = juce::ValueTree("IRFiles");
    for (int i = 0; i < kNumIRSlots; ++i)
    {
        auto irFile = irManager.getLoadedIR(i);
        if (irFile.existsAsFile())
        {
            auto irSlot = juce::ValueTree("Slot" + juce::String(i));
            irSlot.setProperty("path", irFile.getFullPathName(), nullptr);
            irState.appendChild(irSlot, nullptr);
        }
    }
    state.appendChild(irState, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TheKingsCabAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore plugin state including IR files
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(valueTreeState.state.getType()))
        {
            auto state = juce::ValueTree::fromXml(*xmlState);
            valueTreeState.replaceState(state);

            // Restore IR files
            auto irState = state.getChildWithName("IRFiles");
            if (irState.isValid())
            {
                for (int i = 0; i < kNumIRSlots; ++i)
                {
                    auto irSlot = irState.getChildWithName("Slot" + juce::String(i));
                    if (irSlot.isValid())
                    {
                        auto path = irSlot.getProperty("path").toString();
                        if (path.isNotEmpty())
                        {
                            juce::File irFile(path);
                            if (irFile.existsAsFile())
                            {
                                loadImpulseResponse(i, irFile);
                            }
                        }
                    }
                }
            }
        }
    }
}

//==============================================================================
void TheKingsCabAudioProcessor::loadImpulseResponse(int slotIndex, const juce::File& irFile)
{
    DBG("=== AUDIO PROCESSOR loadImpulseResponse START ===");
    DBG("Loading IR for slot " << slotIndex << ": " << irFile.getFullPathName());
    
    if (slotIndex >= 0 && slotIndex < kNumIRSlots)
    {
        DBG("Slot index valid, calling IRManager.loadIR...");
        
        // Load IR through manager and update convolution engine
        if (irManager.loadIR(slotIndex, irFile))
        {
            DBG("IRManager.loadIR returned SUCCESS, getting buffer...");
            auto irBuffer = irManager.getIRBuffer(slotIndex);
            if (irBuffer != nullptr)
            {
                DBG("IR buffer obtained, loading into convolution engine...");
                bool convolutionSuccess = convolutionEngine.loadImpulseResponse(slotIndex, *irBuffer);
                DBG("Convolution engine loadImpulseResponse result: " << (convolutionSuccess ? "SUCCESS" : "FAILED"));
                
                if (convolutionSuccess)
                {
                    // Force immediate audio processing update
                    DBG("Forcing immediate audio processing sync...");
                    
                    // Trigger a parameter update to force audio engine refresh
                    auto* gainParam = valueTreeState.getParameter("slot" + juce::String(slotIndex) + "_gain");
                    if (gainParam)
                    {
                        // Briefly modify and restore gain to force processing update
                        float currentGain = gainParam->getValue();
                        gainParam->setValueNotifyingHost(currentGain + 0.001f);
                        gainParam->setValueNotifyingHost(currentGain);
                        DBG("Forced parameter refresh completed");
                    }
                }
            }
            else
            {
                DBG("ERROR: IR buffer is null after successful IRManager.loadIR!");
            }
        }
        else
        {
            DBG("ERROR: IRManager.loadIR returned FAILURE for slot " << slotIndex);
        }
    }
    else
    {
        DBG("ERROR: Invalid slot index " << slotIndex << " (must be 0-" << (kNumIRSlots-1) << ")");
    }
    
    DBG("=== AUDIO PROCESSOR loadImpulseResponse END ===");
}

void TheKingsCabAudioProcessor::clearImpulseResponse(int slotIndex)
{
    if (slotIndex >= 0 && slotIndex < kNumIRSlots)
    {
        irManager.clearIR(slotIndex);
        convolutionEngine.clearImpulseResponse(slotIndex);
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout TheKingsCabAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    // Master controls
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "master_gain", "Master Gain", 
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        "master_mix", "Dry/IR Mix", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    // IR slot parameters
    for (int i = 0; i < kNumIRSlots; ++i)
    {
        auto slotPrefix = "slot" + juce::String(i) + "_";

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            slotPrefix + "gain", "Slot " + juce::String(i + 1) + " Gain",
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            slotPrefix + "mute", "Slot " + juce::String(i + 1) + " Mute", false));

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            slotPrefix + "solo", "Slot " + juce::String(i + 1) + " Solo", false));

        parameters.push_back(std::make_unique<juce::AudioParameterBool>(
            slotPrefix + "phase", "Slot " + juce::String(i + 1) + " Phase Invert", false));


    }

    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheKingsCabAudioProcessor();
}