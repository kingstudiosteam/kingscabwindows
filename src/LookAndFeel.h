#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 * The King's Cab Premium Look and Feel
 * 
 * Features:
 * - Stunning 3D metallic appearance with depth and shadows
 * - Premium gold/silver gradient styling
 * - Glossy, reflective surfaces for professional studio look
 * - Optimized for cabinet simulation interface
 */
class KingsCabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //==============================================================================
    KingsCabLookAndFeel();
    ~KingsCabLookAndFeel() override;

    //==============================================================================
    // Slider customization for premium feel
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    // Vintage Neve/API style knobs
    void drawVintageKnob(juce::Graphics& g, int x, int y, int width, int height,
                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                        juce::Slider& slider);

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    //==============================================================================
    // Button styling for 3D effect
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    //==============================================================================
    // ComboBox for folder selection
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& comboBox) override;

    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;

    //==============================================================================
    // Label styling for clean text
    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    //==============================================================================
    // Window and panel backgrounds
    void fillResizableWindowBackground(juce::Graphics& g, int w, int h,
                                      const juce::BorderSize<int>& border,
                                      juce::ResizableWindow& window) override;

    //==============================================================================
    // Color scheme for premium metallic look
    enum ColourIds
    {
        // Main background and panels
        backgroundColourId = 0x2000001,
        panelBackgroundColourId = 0x2000002,
        
        // Metallic elements
        metallicHighlightColourId = 0x2000003,
        metallicShadowColourId = 0x2000004,
        metallicBaseColourId = 0x2000005,
        
        // Gold accents
        goldHighlightColourId = 0x2000006,
        goldBaseColourId = 0x2000007,
        goldShadowColourId = 0x2000008,
        
        // Text and labels
        primaryTextColourId = 0x2000009,
        secondaryTextColourId = 0x2000010,
        
        // Control elements
        knobBaseColourId = 0x2000011,
        knobHighlightColourId = 0x2000012,
        buttonActiveColourId = 0x2000013,
        buttonInactiveColourId = 0x2000014,
        
        // Vintage knob colors (Neve/API style)
        vintageKnobBodyColourId = 0x2000015,
        vintageKnobRimColourId = 0x2000016,
        vintageKnobPointerColourId = 0x2000017,
        vintageKnobShadowColourId = 0x2000018
    };

private:
    //==============================================================================
    // Gradient and effect helpers
    void drawMetallicGradient(juce::Graphics& g, const juce::Rectangle<int>& bounds, 
                             bool isPressed = false, bool isHighlighted = false);
    
    void drawGoldAccent(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    
    void draw3DFrame(juce::Graphics& g, const juce::Rectangle<int>& bounds, 
                     bool isInset = false, float cornerSize = 4.0f);
    
    void drawGlossyOverlay(juce::Graphics& g, const juce::Rectangle<int>& bounds, 
                          float cornerSize = 4.0f);

    //==============================================================================
    // Premium color palette
    void initializeColors();
    
    juce::Colour metallicBase;
    juce::Colour metallicHighlight;
    juce::Colour metallicShadow;
    juce::Colour goldBase;
    juce::Colour goldHighlight;
    juce::Colour goldShadow;
    juce::Colour darkBackground;
    juce::Colour lightText;
    juce::Colour dimText;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KingsCabLookAndFeel)
};