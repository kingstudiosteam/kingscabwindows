#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Components/IRSlot.h"

//==============================================================================
/**
 * The King's Cab Plugin Editor
 * 
 * Features:
 * - Premium 3D metallic interface design
 * - 6 IR slots with individual controls
 * - Integrated folder browser for easy IR selection
 * - Master controls for global settings
 * - Optimized layout for professional workflow
 */
class TheKingsCabAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        public juce::Timer,
                                        public juce::Slider::Listener
{
public:
    //==============================================================================
    TheKingsCabAudioProcessorEditor(TheKingsCabAudioProcessor&);
    ~TheKingsCabAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // Timer callback for UI updates
    void timerCallback() override;

    //==============================================================================
    // Slider listener for master controls
    void sliderValueChanged(juce::Slider* slider) override;

private:
    //==============================================================================
    // Reference to processor
    TheKingsCabAudioProcessor& audioProcessor;

    //==============================================================================
    // Look and feel
    KingsCabLookAndFeel kingsCabLookAndFeel;

    //==============================================================================
    // UI Components
    
    // Header section (logo removed)
    
    // Custom background
    juce::Image customBackgroundImage;
    juce::Image headerBackgroundImage;
    juce::Image mainBodyBackgroundImage;
    juce::Image footerBackgroundImage;
    
    // Master controls
    std::unique_ptr<juce::Slider> masterGainSlider;
    std::unique_ptr<juce::Slider> masterMixSlider;
    std::unique_ptr<juce::Label> masterGainLabel;
    std::unique_ptr<juce::Label> masterMixLabel;
    
    // IR Slots (6 slots arranged in 2 columns)
    std::array<std::unique_ptr<IRSlot>, TheKingsCabAudioProcessor::kNumIRSlots> irSlots;
    
    // Folder browser removed - using only your IR collection
    
    // Footer
    std::unique_ptr<juce::Label> statusLabel;
    std::unique_ptr<juce::HyperlinkButton> storeLink;

    //==============================================================================
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterMixAttachment;

    //==============================================================================
    // Layout constants
    static constexpr int kWindowWidth = 840;
    static constexpr int kWindowHeight = 620;  // Original size for perfect background fit
    static constexpr int kHeaderHeight = 80;
    static constexpr int kFooterHeight = 40;
    static constexpr int kMasterControlsHeight = 120;
    static constexpr int kSlotSpacing = 8;  // Tighter spacing

    //==============================================================================
    // Helper methods
    void setupComponents();
    void setupIRSlots();
    void initializeIRData();
    void loadCustomBackground();
    void loadHeaderBackground();
    void loadMainBodyBackground();
    void loadFooterBackground();

    void updateStatusDisplay();
    void drawBackground(juce::Graphics& g);
    void drawHeader(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawFooter(juce::Graphics& g, const juce::Rectangle<int>& bounds);

    //==============================================================================
    // IR slot callbacks
    void onIRSelected(int slotIndex, const juce::File& irFile);
    void onIRCleared(int slotIndex);
    void onIRPreview(const IRManager::IRInfo& irInfo);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheKingsCabAudioProcessorEditor)
};