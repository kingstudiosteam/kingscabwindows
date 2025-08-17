#include "IRSlot.h"
#include "../DSP/IRManager.h"

//==============================================================================
// Custom Navigation Button Component - bypasses JUCE button state issues
class NavigationButton : public juce::Component
{
public:
    NavigationButton(const juce::String& text, std::function<void()> onClick)
        : buttonText(text), clickCallback(onClick)
    {
        // Tooltip not needed for now - just immediate visual feedback
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Background
        auto bgColor = isMouseOver ? juce::Colours::lightgrey.withAlpha(0.3f) : juce::Colours::darkgrey.withAlpha(0.2f);
        g.setColour(bgColor);
        g.fillRoundedRectangle(bounds, 2.0f);
        
        // Border
        g.setColour(juce::Colours::grey);
        g.drawRoundedRectangle(bounds, 2.0f, 1.0f);
        
        // Text
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawText(buttonText, bounds.toNearestInt(), juce::Justification::centred);
    }
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        isMouseOver = true;
        repaint();
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        isMouseOver = false;
        repaint();
    }
    
    void mouseDown(const juce::MouseEvent&) override
    {
        DBG("=== NavigationButton mouseDown START - " << buttonText << " ===");
        if (clickCallback)
        {
            DBG("Calling clickCallback for " << buttonText);
            clickCallback();
            DBG("clickCallback completed for " << buttonText);
        }
        else
        {
            DBG("ERROR: No clickCallback for " << buttonText);
        }
        DBG("=== NavigationButton mouseDown END - " << buttonText << " ===");
    }
    
private:
    juce::String buttonText;
    std::function<void()> clickCallback;
    bool isMouseOver = false;
};

//==============================================================================
IRSlot::IRSlot(int slotIndex, juce::AudioProcessorValueTreeState& valueTreeState)
    : slotIndex(slotIndex), valueTreeState(valueTreeState)
{
    setupComponents();
    juce::Component::setLookAndFeel(&kingsCabLookAndFeel);
}

IRSlot::~IRSlot()
{
    juce::Component::setLookAndFeel(nullptr);
}

//==============================================================================
void IRSlot::setupComponents()
{
    auto paramPrefix = getParameterPrefix();

    // Compact slot label
    slotLabel = std::make_unique<juce::Label>("SlotLabel", "IR " + juce::String(slotIndex + 1));
    slotLabel->setFont(juce::Font(10.0f).boldened()); // Much smaller font
    slotLabel->setJustificationType(juce::Justification::centred);
    slotLabel->setColour(juce::Label::textColourId, 
                        kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::primaryTextColourId));
    addAndMakeVisible(slotLabel.get());

    // Folder selection
    folderComboBox = std::make_unique<juce::ComboBox>("FolderCombo");
    folderComboBox->setTextWhenNothingSelected("Select Folder...");
    folderComboBox->addListener(this);
    addAndMakeVisible(folderComboBox.get());

    // IR selection
    irComboBox = std::make_unique<juce::ComboBox>("IRCombo");
    irComboBox->setTextWhenNothingSelected("Select IR...");
    irComboBox->addListener(this);
    irComboBox->setEnabled(false);
    addAndMakeVisible(irComboBox.get());

    // Gain control
    gainSlider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, 
                                               juce::Slider::NoTextBox);
    gainSlider->setRange(0.0, 2.0, 0.01);
    gainSlider->setValue(1.0);
    gainSlider->addListener(this);
    addAndMakeVisible(gainSlider.get());



    // Control buttons
    muteButton = std::make_unique<juce::TextButton>("M");
    muteButton->setClickingTogglesState(true);
    muteButton->addListener(this);
    addAndMakeVisible(muteButton.get());

    soloButton = std::make_unique<juce::TextButton>("S");
    soloButton->setClickingTogglesState(true);
    soloButton->addListener(this);
    addAndMakeVisible(soloButton.get());

    // Navigation buttons for seamless IR browsing - using custom components
    prevIRButton = std::make_unique<NavigationButton>("^", [this]() {
        DBG("Previous IR navigation clicked");
        navigateToIR(-1);
    });
    addAndMakeVisible(prevIRButton.get());
    
    nextIRButton = std::make_unique<NavigationButton>("v", [this]() {
        DBG("Next IR navigation clicked");
        navigateToIR(1);
    });
    addAndMakeVisible(nextIRButton.get());

    // Phase and clear buttons removed - not needed for this implementation

    // Create parameter attachments
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        valueTreeState, paramPrefix + "gain", *gainSlider);
    

    
    muteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        valueTreeState, paramPrefix + "mute", *muteButton);
    
    soloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        valueTreeState, paramPrefix + "solo", *soloButton);
}

//==============================================================================
void IRSlot::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Transparent background - no frame drawing to show plugin background
    // Removed: drawSlotFrame(g, bounds); for clean transparent look
    
    // IR display removed - no gold file/waveform display needed
    juce::ignoreUnused(bounds);
}

void IRSlot::resized()
{
    auto bounds = getLocalBounds().reduced(4); // Smaller padding
    
    // Compact slot label at top
    slotLabel->setBounds(bounds.removeFromTop(12)); // Much smaller
    bounds.removeFromTop(2);
    
    // IR display area removed - no waveform display needed
    
    // Compact folder and IR selection - much narrower
    auto comboHeight = 16; // Much smaller dropdowns
    auto comboWidth = bounds.getWidth() * 0.7f; // Only 70% of available width (much narrower)
    
    auto folderArea = bounds.removeFromTop(comboHeight);
    if (rightAligned)
        folderComboBox->setBounds(folderArea.removeFromRight(comboWidth)); // Right-aligned
    else
        folderComboBox->setBounds(folderArea.removeFromLeft(comboWidth));  // Left-aligned
    bounds.removeFromTop(1);
    
    auto irArea = bounds.removeFromTop(comboHeight);
    auto navButtonWidth = 18; // Small navigation buttons
    
    if (rightAligned)
    {
        // Right-aligned: [<] [>] [IR Dropdown]
        nextIRButton->setBounds(irArea.removeFromRight(navButtonWidth));
        irArea.removeFromRight(2);
        prevIRButton->setBounds(irArea.removeFromRight(navButtonWidth));
        irArea.removeFromRight(3);
        irComboBox->setBounds(irArea.removeFromRight(comboWidth - navButtonWidth * 2 - 5));
    }
    else
    {
        // Left-aligned: [IR Dropdown] [<] [>]
        irComboBox->setBounds(irArea.removeFromLeft(comboWidth - navButtonWidth * 2 - 5));
        irArea.removeFromLeft(3);
        prevIRButton->setBounds(irArea.removeFromLeft(navButtonWidth));
        irArea.removeFromLeft(2);
        nextIRButton->setBounds(irArea.removeFromLeft(navButtonWidth));
    }
    bounds.removeFromTop(3);
    
    // Compact controls layout
    auto controlsArea = bounds.removeFromTop(40); // Much smaller controls area
    
    if (rightAligned)
    {
        // Right-aligned layout: buttons first, then knob
        auto buttonWidth = 30; // Fixed narrow width
        auto buttonHeight = 18; // Smaller buttons
        controlsArea.removeFromTop(5); // Tighter spacing
        
        soloButton->setBounds(controlsArea.removeFromRight(buttonWidth).removeFromTop(buttonHeight));
        controlsArea.removeFromRight(3);
        muteButton->setBounds(controlsArea.removeFromRight(buttonWidth).removeFromTop(buttonHeight));
        controlsArea.removeFromRight(4);
        
        // Gain knob on the right side
        auto gainArea = controlsArea.removeFromRight(35);
        auto gainKnobSquare = juce::Rectangle<int>(gainArea.getX(), gainArea.getY(), 35, 35);
        gainSlider->setBounds(gainKnobSquare);
    }
    else
    {
        // Left-aligned layout: knob first, then buttons
        auto gainArea = controlsArea.removeFromLeft(35); // Much smaller knob
        auto gainKnobSquare = juce::Rectangle<int>(gainArea.getX(), gainArea.getY(), 35, 35);
        gainSlider->setBounds(gainKnobSquare);
        
        // Compact buttons in a row - much narrower Mute and Solo
        controlsArea.removeFromLeft(4);
        controlsArea.removeFromTop(5); // Tighter spacing
        auto buttonWidth = 30; // Fixed narrow width instead of dynamic
        auto buttonHeight = 18; // Smaller buttons
        
        muteButton->setBounds(controlsArea.removeFromLeft(buttonWidth).removeFromTop(buttonHeight));
        controlsArea.removeFromLeft(3);
        soloButton->setBounds(controlsArea.removeFromLeft(buttonWidth).removeFromTop(buttonHeight));
    }
}

//==============================================================================
void IRSlot::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    DBG("===== COMBO_BOX_CHANGED START =====");
    DBG("Slot " << slotIndex << " comboBoxChanged called");
    
    if (comboBoxThatHasChanged == folderComboBox.get())
    {
        DBG("Folder ComboBox changed");
        // Update IR list based on selected folder
        updateIRComboBox();
    }
    else if (comboBoxThatHasChanged == irComboBox.get())
    {
        DBG("IR ComboBox changed");
        DBG("IR ComboBox selected ID: " << irComboBox->getSelectedId());
        DBG("IR ComboBox selected item index: " << irComboBox->getSelectedItemIndex());
        DBG("Available IRs count: " << displayData.availableIRs.size());
        
        // Load selected IR
        auto selectedIRIndex = irComboBox->getSelectedItemIndex();
        if (selectedIRIndex >= 0 && selectedIRIndex < static_cast<int>(displayData.availableIRs.size()))
        {
            auto& selectedIR = displayData.availableIRs[selectedIRIndex];
            DBG("Loading IR at index " << selectedIRIndex << ": " << selectedIR.name);
            DBG("IR file path: " << selectedIR.file.getFullPathName());
            
            if (onIRSelected)
            {
                DBG("CALLING onIRSelected callback from comboBoxChanged");
                onIRSelected(slotIndex, selectedIR.file);
                DBG("onIRSelected callback COMPLETED from comboBoxChanged");
            }
            else
            {
                DBG("ERROR - No onIRSelected callback in comboBoxChanged");
            }
            
            // Update display data
            displayData.irName = selectedIR.name;
            displayData.hasValidIR = true;
            setActive(true);
            repaint();
            
            DBG("Display data updated and repaint called");
        }
        else
        {
            DBG("ERROR - Invalid IR selection index: " << selectedIRIndex << " (available: " << displayData.availableIRs.size() << ")");
        }
    }
    else
    {
        DBG("Unknown ComboBox changed");
    }
    
    DBG("===== COMBO_BOX_CHANGED END =====");
}

void IRSlot::sliderValueChanged(juce::Slider* slider)
{
    juce::ignoreUnused(slider);
    // Parameter attachments handle the actual parameter updates
    repaint();
}

void IRSlot::buttonClicked(juce::Button* button)
{
    // Navigation is now handled by custom NavigationButton components with lambda callbacks
    // Mute and Solo buttons are handled by parameter attachments
    juce::ignoreUnused(button);
    repaint();
}



//==============================================================================
void IRSlot::updateFolderList(const std::vector<IRManager::FolderInfo>& folders)
{
    availableFolders = folders;
    
    folderComboBox->clear();
    folderComboBox->addItem("Select Folder...", -1);
    
    for (int i = 0; i < static_cast<int>(folders.size()); ++i)
    {
        folderComboBox->addItem(folders[i].name, i + 1);
    }
}

void IRSlot::setLoadedIR(const juce::String& folderName, const juce::String& irName)
{
    displayData.folderName = folderName;
    displayData.irName = irName;
    displayData.hasValidIR = true;
    setActive(true);
    repaint();
}

void IRSlot::clearIR()
{
    displayData.folderName.clear();
    displayData.irName.clear();
    displayData.hasValidIR = false;
    displayData.availableIRs.clear();
    
    folderComboBox->setSelectedItemIndex(0);
    irComboBox->clear();
    irComboBox->setEnabled(false);
    
    setActive(false);
    repaint();
}

//==============================================================================
void IRSlot::setActive(bool isActive)
{
    active = isActive;
    
    // Update visual state
    auto alpha = active ? 1.0f : 0.7f;
    setAlpha(alpha);
    
    // Enable/disable controls based on active state
    gainSlider->setEnabled(active);
    muteButton->setEnabled(active);
    soloButton->setEnabled(active);
}

//==============================================================================
juce::String IRSlot::getParameterPrefix() const
{
    return "slot" + juce::String(slotIndex) + "_";
}

void IRSlot::updateIRComboBox()
{
    irComboBox->clear();
    irComboBox->setEnabled(false);
    displayData.availableIRs.clear();
    preloadedIRBuffers.clear(); // Clear any existing preloaded buffers
    
    auto selectedFolderIndex = folderComboBox->getSelectedItemIndex() - 1; // Adjust for "Select Folder..." item
    
    if (selectedFolderIndex >= 0 && selectedFolderIndex < static_cast<int>(availableFolders.size()))
    {
        irComboBox->addItem("Select IR...", -1);
        
        // Get IRs from the selected folder
        const auto& folder = availableFolders[selectedFolderIndex];
        displayData.availableIRs = folder.irFiles;
        
        DBG("Pre-loading all IRs in folder: " << folder.name);
        
        // Populate the combo box with proper IDs AND pre-load buffers
        for (int i = 0; i < static_cast<int>(folder.irFiles.size()); ++i)
        {
            irComboBox->addItem(folder.irFiles[i].name, i + 1); // IDs start from 1 (0 is reserved for "Select IR...")
            
            // Pre-load this IR buffer for instant navigation
            preloadIRBuffer(folder.irFiles[i].file);
        }
        
        DBG("Pre-loaded " << preloadedIRBuffers.size() << " IR buffers for instant navigation");
        
        irComboBox->setEnabled(true);
        displayData.folderName = folder.name;
    }
}

void IRSlot::drawSlotFrame(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    auto floatBounds = bounds.toFloat();
    
    // Main panel background with inset effect
    g.setColour(kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::panelBackgroundColourId));
    g.fillRoundedRectangle(floatBounds, 6.0f);
    
    // 3D frame effect
    auto frameColour = active ? 
        kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::goldBaseColourId) :
        kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::metallicBaseColourId);
    
    g.setColour(frameColour.withAlpha(0.8f));
    g.drawRoundedRectangle(floatBounds.reduced(1), 6.0f, 2.0f);
    
    // Inner shadow for depth
    g.setColour(kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::metallicShadowColourId).withAlpha(0.5f));
    g.drawRoundedRectangle(floatBounds.reduced(3), 4.0f, 1.0f);
}

void IRSlot::drawIRDisplay(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    auto floatBounds = bounds.toFloat();
    
    // Subtle transparent background - barely visible
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRoundedRectangle(floatBounds, 3.0f);
    
    // Very subtle border
    g.setColour(kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::metallicHighlightColourId).withAlpha(0.3f));
    g.drawRoundedRectangle(floatBounds, 3.0f, 1.0f);
    
    // IR name display
    if (displayData.hasValidIR)
    {
        g.setColour(kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::goldHighlightColourId));
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        
        auto textBounds = floatBounds.reduced(8, 4);
        g.drawText(displayData.irName, textBounds.toNearestInt(), juce::Justification::centredLeft);
        
        // Simple waveform representation (placeholder for future enhancement)
        auto waveformBounds = textBounds.removeFromRight(textBounds.getWidth() * 0.4f);
        g.setColour(kingsCabLookAndFeel.findColour(KingsCabLookAndFeel::goldBaseColourId).withAlpha(0.6f));
        
        // Draw simple bars to represent audio content
        auto barWidth = 2.0f;
        auto barSpacing = 3.0f;
        auto numBars = static_cast<int>(waveformBounds.getWidth() / (barWidth + barSpacing));
        
        for (int i = 0; i < numBars; ++i)
        {
            auto x = waveformBounds.getX() + i * (barWidth + barSpacing);
            auto height = waveformBounds.getHeight() * (0.3f + 0.7f * std::sin(i * 0.5f));
            auto y = waveformBounds.getCentreY() - height * 0.5f;
            
            g.fillRect(x, y, barWidth, height);
        }
    }
}

//==============================================================================
void IRSlot::navigateToIR(int direction)
{
    DBG("===== NAVIGATE_TO_IR START =====");
    DBG("Slot " << slotIndex << " navigateToIR called with direction: " << direction);
    
    // Only navigate if we have a folder selected and IRs are available
    if (folderComboBox->getSelectedId() <= 0 || displayData.availableIRs.empty())
    {
        DBG("EARLY EXIT - Cannot navigate - no folder selected or no IRs available. Folder ID: " << folderComboBox->getSelectedId() << ", IRs count: " << displayData.availableIRs.size());
        return;
    }
    
    // Get current selected IR index in the available IRs list
    int currentIndex = -1;
    int currentSelectedId = irComboBox->getSelectedId();
    int currentSelectedItemIndex = irComboBox->getSelectedItemIndex();
    
    DBG("Current IR ComboBox ID: " << currentSelectedId);
    DBG("Current IR ComboBox ItemIndex: " << currentSelectedItemIndex);
    DBG("Current IR ComboBox Text: '" << irComboBox->getText() << "'");
    
    // Find current IR index (ComboBox IDs start from 1, array indices from 0)
    if (currentSelectedId > 0)
        currentIndex = currentSelectedId - 1;
    
    DBG("Calculated currentIndex from ID: " << currentIndex);
    
    // Double-check with ItemIndex method
    if (currentSelectedItemIndex >= 0)
        DBG("ItemIndex method gives: " << currentSelectedItemIndex);
    
    // Verify current selection matches our data
    if (currentIndex >= 0 && currentIndex < static_cast<int>(displayData.availableIRs.size()))
    {
        DBG("Current IR should be: " << displayData.availableIRs[currentIndex].name);
    }
    else
    {
        DBG("WARNING: Current index " << currentIndex << " is out of range for availableIRs");
    }
    
    int numIRs = static_cast<int>(displayData.availableIRs.size());
    if (numIRs <= 0)
    {
        DBG("EARLY EXIT - No IRs available in current folder");
        return;
    }
    
    DBG("Current index: " << currentIndex << ", Total IRs: " << numIRs);
    
    // List all available IRs for debugging
    DBG("Available IRs:");
    for (int i = 0; i < numIRs; ++i)
    {
        DBG("  [" << i << "] " << displayData.availableIRs[i].name);
    }
    
    int newIndex;
    
    // If no IR is currently selected, start from the first one
    if (currentIndex < 0)
    {
        if (direction > 0)
            newIndex = 0; // First IR
        else
            newIndex = numIRs - 1; // Last IR
        DBG("No current selection, starting at index: " << newIndex);
    }
    else
    {
        // Calculate new index with wrapping
        newIndex = currentIndex + direction;
        
        if (newIndex < 0)
            newIndex = numIRs - 1; // Wrap to last IR
        else if (newIndex >= numIRs)
            newIndex = 0; // Wrap to first IR
        
        DBG("Calculated new index: " << newIndex << " (from " << currentIndex << " + " << direction << ")");
    }
    
    if (newIndex < 0 || newIndex >= numIRs)
    {
        DBG("ERROR - New index out of bounds: " << newIndex << " (should be 0-" << (numIRs-1) << ")");
        return;
    }
    
    DBG("Setting ComboBox to ID: " << (newIndex + 1) << " (index " << newIndex << ")");
    
    // Update ComboBox selection WITHOUT notification first
    irComboBox->setSelectedId(newIndex + 1, juce::dontSendNotification);
    
    DBG("Using pre-loaded IR buffer for instant navigation...");
    
    // Use pre-loaded buffer for instant navigation (no disk I/O!)
    usePreloadedIR(newIndex);
    
    DBG("ComboBox final state - ID: " << irComboBox->getSelectedId() << ", ItemIndex: " << irComboBox->getSelectedItemIndex());
    DBG("===== NAVIGATE_TO_IR END =====");
}

//==============================================================================
void IRSlot::preloadIRBuffer(const juce::File& irFile)
{
    DBG("Pre-loading IR buffer: " << irFile.getFileName());
    
    // Create audio format manager
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    // Create reader for the IR file
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(irFile));
    
    if (reader == nullptr)
    {
        DBG("Failed to create reader for IR file: " << irFile.getFullPathName());
        return;
    }
    
    // Create buffer and read the IR data
    auto buffer = std::make_unique<juce::AudioBuffer<float>>();
    int numChannels = static_cast<int>(reader->numChannels);
    int lengthInSamples = static_cast<int>(reader->lengthInSamples);
    
    buffer->setSize(numChannels, lengthInSamples, false, true, true);
    
    if (reader->read(buffer.get(), 0, lengthInSamples, 0, true, true))
    {
        preloadedIRBuffers.push_back(std::move(buffer));
        DBG("Successfully pre-loaded IR: " << irFile.getFileName() << " (" << numChannels << " channels, " << lengthInSamples << " samples)");
    }
    else
    {
        DBG("Failed to read IR data from: " << irFile.getFullPathName());
    }
}

void IRSlot::usePreloadedIR(int irIndex)
{
    DBG("===== USE_PRELOADED_IR START =====");
    DBG("Using pre-loaded IR at index: " << irIndex);
    
    if (irIndex < 0 || irIndex >= static_cast<int>(preloadedIRBuffers.size()))
    {
        DBG("ERROR: Invalid preloaded IR index: " << irIndex << " (available: " << preloadedIRBuffers.size() << ")");
        return;
    }
    
    if (irIndex >= static_cast<int>(displayData.availableIRs.size()))
    {
        DBG("ERROR: IR index out of bounds for availableIRs: " << irIndex);
        return;
    }
    
    auto& buffer = *preloadedIRBuffers[irIndex];
    const auto& irInfo = displayData.availableIRs[irIndex];
    
    DBG("Pre-loaded buffer: " << buffer.getNumChannels() << " channels, " << buffer.getNumSamples() << " samples");
    
    // Directly load the pre-loaded buffer into the convolution engine via callback
    if (onIRSelected)
    {
        // We'll use the existing IR loading infrastructure but with pre-loaded data
        // For now, we still need to use the file path for the callback
        const auto& selectedIR = displayData.availableIRs[irIndex];
        DBG("Using existing callback with file: " << selectedIR.file.getFullPathName());
        onIRSelected(slotIndex, selectedIR.file);
        
        DBG("Successfully triggered IR loading via callback");
        
        // Update display data
        displayData.irName = irInfo.name;
        displayData.hasValidIR = true;
        setActive(true);
        repaint();
        
        DBG("UI updated for pre-loaded IR: " << irInfo.name);
    }
    else
    {
        DBG("ERROR: No onIRSelected callback available");
    }
    
    DBG("===== USE_PRELOADED_IR END =====");
}


