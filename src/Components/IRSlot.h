#pragma once

#include <JuceHeader.h>
#include "../LookAndFeel.h"
#include "../DSP/IRManager.h"

//==============================================================================
/**
 * Individual IR Slot Component for The King's Cab
 * 
 * Features:
 * - Folder dropdown with IR selection
 * - Volume, solo, mute, phase controls
 * - Premium 3D styling to match cabinet aesthetic
 * - Real-time waveform display
 */
class IRSlot : public juce::Component,
               public juce::ComboBox::Listener,
               public juce::Slider::Listener,
               public juce::Button::Listener
{
public:
    //==============================================================================
    IRSlot(int slotIndex, juce::AudioProcessorValueTreeState& valueTreeState);
    ~IRSlot() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // Component callbacks
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    //==============================================================================
    // Layout configuration
    void setRightAligned(bool isRightAligned) { rightAligned = isRightAligned; }
    
    //==============================================================================
    // IR Management
    void updateFolderList(const std::vector<struct IRManager::FolderInfo>& folders);
    void setLoadedIR(const juce::String& folderName, const juce::String& irName);
    void clearIR();

    //==============================================================================
    // Callbacks for parent component
    std::function<void(int, const juce::File&)> onIRSelected;
    std::function<void(int)> onIRCleared;

    //==============================================================================
    // Visual state
    void setActive(bool isActive);
    bool isActive() const { return active; }

private:
    //==============================================================================
    // Core data
    int slotIndex;
    bool active = false;
    bool rightAligned = false;
    juce::AudioProcessorValueTreeState& valueTreeState;

    //==============================================================================
    // UI Components
    std::unique_ptr<juce::Label> slotLabel;
    std::unique_ptr<juce::ComboBox> folderComboBox;
    std::unique_ptr<juce::ComboBox> irComboBox;
    
    // Controls
    std::unique_ptr<juce::Slider> gainSlider;
    std::unique_ptr<juce::TextButton> muteButton;
    std::unique_ptr<juce::TextButton> soloButton;
    
    // Navigation buttons for seamless IR browsing - using standard components
    std::unique_ptr<juce::Component> prevIRButton;
    std::unique_ptr<juce::Component> nextIRButton;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> soloAttachment;

    //==============================================================================
    // IR data for display
    struct IRDisplayData
    {
        juce::String folderName;
        juce::String irName;
        std::vector<struct IRManager::IRInfo> availableIRs;
        bool hasValidIR = false;
    };
    IRDisplayData displayData;
    
    // Available folders from IR Manager
    std::vector<IRManager::FolderInfo> availableFolders;
    
    // Pre-loaded IR buffers for instant navigation
    std::vector<std::unique_ptr<juce::AudioBuffer<float>>> preloadedIRBuffers;

    //==============================================================================
    // Look and feel
    KingsCabLookAndFeel kingsCabLookAndFeel;

    //==============================================================================
    // Helper methods
    void setupComponents();
    void updateIRComboBox();
    void navigateToIR(int direction); // Navigate through IRs in current folder (-1 = prev, +1 = next)
    void preloadIRBuffer(const juce::File& irFile); // Pre-load IR buffer for instant navigation
    void usePreloadedIR(int irIndex); // Use pre-loaded IR buffer instead of loading from disk
    juce::String getParameterPrefix() const;
    void drawSlotFrame(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void drawIRDisplay(juce::Graphics& g, const juce::Rectangle<int>& bounds);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRSlot)
};
