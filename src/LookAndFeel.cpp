#include "LookAndFeel.h"

//==============================================================================
KingsCabLookAndFeel::KingsCabLookAndFeel()
{
    initializeColors();
}

KingsCabLookAndFeel::~KingsCabLookAndFeel()
{
}

//==============================================================================
void KingsCabLookAndFeel::initializeColors()
{
    // Premium metallic color palette for studio-grade appearance
    darkBackground = juce::Colour(0xff000000); // Pure black background
    
    // Brushed aluminum/steel look
    metallicBase = juce::Colour(0xff505050);
    metallicHighlight = juce::Colour(0xff808080);
    metallicShadow = juce::Colour(0xff2a2a2a);
    
    // Gold accents for premium feel
    goldBase = juce::Colour(0xffb8860b);
    goldHighlight = juce::Colour(0xffd4af37);
    goldShadow = juce::Colour(0xff8b6914);
    
    // Text colors for readability
    lightText = juce::Colour(0xfff0f0f0);
    dimText = juce::Colour(0xffa0a0a0);

    // Set JUCE color scheme
    setColour(backgroundColourId, darkBackground);
    setColour(panelBackgroundColourId, juce::Colour(0xff2d2d2d));
    
    setColour(metallicBaseColourId, metallicBase);
    setColour(metallicHighlightColourId, metallicHighlight);
    setColour(metallicShadowColourId, metallicShadow);
    
    setColour(goldBaseColourId, goldBase);
    setColour(goldHighlightColourId, goldHighlight);
    setColour(goldShadowColourId, goldShadow);
    
    setColour(primaryTextColourId, lightText);
    setColour(secondaryTextColourId, dimText);
    
    // Override default JUCE colors for consistency
    setColour(juce::ResizableWindow::backgroundColourId, darkBackground);
    setColour(juce::Label::textColourId, lightText);
    setColour(juce::ComboBox::backgroundColourId, metallicBase);
    setColour(juce::ComboBox::textColourId, lightText);
    setColour(juce::TextButton::buttonColourId, metallicBase);
    setColour(juce::TextButton::textColourOffId, lightText);
}

//==============================================================================
void KingsCabLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider& slider)
{
    // Use the new vintage knob style for all rotary sliders
    drawVintageKnob(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
}

//==============================================================================
void KingsCabLookAndFeel::drawVintageKnob(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider&)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto center = bounds.getCentre();
    
    // Classic Neve/API knob colors - brushed aluminum with darker rim
    auto knobBody = juce::Colour(0xff6a6a6a);        // Brushed aluminum body
    auto knobRim = juce::Colour(0xff3a3a3a);         // Darker rim 
    auto knobHighlight = juce::Colour(0xff9a9a9a);   // Bright highlight
    auto knobShadow = juce::Colour(0xff2a2a2a);      // Deep shadow
    auto pointerColor = juce::Colour(0xfff0f0f0);    // White pointer line
    
    // Draw drop shadow for depth
    auto shadowBounds = bounds.expanded(1);
    g.setColour(knobShadow.withAlpha(0.3f));
    g.fillEllipse(shadowBounds);
    
    // Draw outer rim (darker)
    g.setColour(knobRim);
    g.fillEllipse(bounds);
    
    // Draw main knob body with brushed metal gradient
    auto knobBounds = bounds.reduced(radius * 0.15f);
    g.setGradientFill(juce::ColourGradient(
        knobHighlight, knobBounds.getTopLeft(),
        knobBody, knobBounds.getBottomRight(), false));
    g.fillEllipse(knobBounds);
    
    // Add circular brushed metal texture
    auto textureRadius = knobBounds.getWidth() / 2.0f;
    for (int i = 0; i < 24; ++i)
    {
        auto angle = (i / 24.0f) * juce::MathConstants<float>::twoPi;
        auto startRadius = textureRadius * 0.3f;
        auto endRadius = textureRadius * 0.85f;
        
        auto startX = center.x + startRadius * std::cos(angle);
        auto startY = center.y + startRadius * std::sin(angle);
        auto endX = center.x + endRadius * std::cos(angle);
        auto endY = center.y + endRadius * std::sin(angle);
        
        g.setColour(knobHighlight.withAlpha(0.15f));
        g.drawLine(startX, startY, endX, endY, 0.5f);
    }
    
    // Draw inner rim highlight
    g.setColour(knobHighlight.withAlpha(0.8f));
    g.drawEllipse(knobBounds.reduced(1), 1.0f);
    
    // Draw center indicator dot
    auto centerDot = juce::Rectangle<float>(4, 4).withCentre(center);
    g.setColour(knobShadow);
    g.fillEllipse(centerDot);
    
    // Draw pointer line (classic style)
    auto pointerLength = radius * 0.7f;
    auto pointerThickness = 2.5f;
    
    auto pointerX = center.x + pointerLength * std::cos(toAngle - juce::MathConstants<float>::halfPi);
    auto pointerY = center.y + pointerLength * std::sin(toAngle - juce::MathConstants<float>::halfPi);
    
    // Pointer shadow
    g.setColour(knobShadow.withAlpha(0.6f));
    g.drawLine(center.x + 1, center.y + 1, pointerX + 1, pointerY + 1, pointerThickness);
    
    // Main pointer
    g.setColour(pointerColor);
    g.drawLine(center.x, center.y, pointerX, pointerY, pointerThickness);
    
    // Add small highlight at pointer tip
    auto tipHighlight = juce::Rectangle<float>(2, 2).withCentre(juce::Point<float>(pointerX, pointerY));
    g.setColour(pointerColor.brighter(0.3f));
    g.fillEllipse(tipHighlight);
}

void KingsCabLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float minSliderPos, float maxSliderPos,
                                          const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    juce::ignoreUnused(minSliderPos, maxSliderPos);
    
    auto trackBounds = juce::Rectangle<int>(x, y, width, height);
    auto isVertical = style == juce::Slider::LinearVertical;
    
    // Draw track background with inset 3D effect
    auto trackArea = isVertical ? trackBounds.reduced(width / 3, 4) : trackBounds.reduced(4, height / 3);
    
    g.setColour(metallicShadow);
    g.fillRoundedRectangle(trackArea.toFloat(), 2.0f);
    
    g.setGradientFill(juce::ColourGradient(metallicShadow, trackArea.getTopLeft().toFloat(),
                                          metallicBase, trackArea.getBottomRight().toFloat(), false));
    g.fillRoundedRectangle(trackArea.reduced(1).toFloat(), 2.0f);
    
    // Draw value track with gold gradient
    auto valueTrack = trackArea;
    if (isVertical)
    {
        auto valueHeight = static_cast<int>((1.0f - sliderPos) * trackArea.getHeight());
        valueTrack = valueTrack.removeFromBottom(trackArea.getHeight() - valueHeight);
    }
    else
    {
        auto valueWidth = static_cast<int>(sliderPos * trackArea.getWidth());
        valueTrack = valueTrack.removeFromLeft(valueWidth);
    }
    
    if (!valueTrack.isEmpty())
    {
        g.setGradientFill(juce::ColourGradient(goldHighlight, valueTrack.getTopLeft().toFloat(),
                                              goldBase, valueTrack.getBottomRight().toFloat(), false));
        g.fillRoundedRectangle(valueTrack.reduced(1).toFloat(), 2.0f);
    }
    
    // Draw thumb with 3D metallic effect
    auto thumbSize = isVertical ? width - 4 : height - 4;
    auto thumbBounds = juce::Rectangle<int>(thumbSize, thumbSize);
    
    if (isVertical)
    {
        auto thumbY = static_cast<int>(y + (1.0f - sliderPos) * (height - thumbSize));
        thumbBounds = thumbBounds.withCentre(juce::Point<int>(x + width / 2, thumbY + thumbSize / 2));
    }
    else
    {
        auto thumbX = static_cast<int>(x + sliderPos * (width - thumbSize));
        thumbBounds = thumbBounds.withCentre(juce::Point<int>(thumbX + thumbSize / 2, y + height / 2));
    }
    
    drawMetallicGradient(g, thumbBounds, false, slider.isMouseOverOrDragging());
}

//==============================================================================
void KingsCabLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& backgroundColour,
                                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(backgroundColour);
    
    auto bounds = button.getLocalBounds();
    drawMetallicGradient(g, bounds, shouldDrawButtonAsDown, shouldDrawButtonAsHighlighted);
    
    // Add gold accent for active states
    if (button.getToggleState())
    {
        drawGoldAccent(g, bounds);
    }
}

void KingsCabLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    
    auto font = getTextButtonFont(button, button.getHeight());
    g.setFont(font);
    
    auto textColour = button.getToggleState() ? goldHighlight : lightText;
    if (!button.isEnabled())
        textColour = dimText;
        
    g.setColour(textColour);
    
    auto yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    auto cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;
    auto fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
    auto leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    auto rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    auto textWidth = button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
    {
        g.drawFittedText(button.getButtonText(),
                        leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                        juce::Justification::centred, 2);
    }
}

//==============================================================================
void KingsCabLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                      int buttonX, int buttonY, int buttonW, int buttonH,
                                      juce::ComboBox& comboBox)
{
    juce::ignoreUnused(buttonX, buttonY, buttonW, buttonH);
    
    auto bounds = juce::Rectangle<int>(0, 0, width, height);
    drawMetallicGradient(g, bounds, isButtonDown, comboBox.hasKeyboardFocus(true));
    
    // Draw dropdown arrow
    auto arrowBounds = bounds.removeFromRight(height).reduced(height / 4);
    juce::Path arrow;
    arrow.addTriangle(arrowBounds.getX(), arrowBounds.getY(),
                     arrowBounds.getRight(), arrowBounds.getY(),
                     arrowBounds.getCentreX(), arrowBounds.getBottom());
    
    g.setColour(lightText);
    g.fillPath(arrow);
}

void KingsCabLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(1, 1, box.getWidth() - box.getHeight(), box.getHeight() - 2);
    label.setFont(getComboBoxFont(box));
}

//==============================================================================
void KingsCabLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const juce::Font font(getLabelFont(label));

        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);

        auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                        juce::jmax(1, (int)(textArea.getHeight() / font.getHeight())),
                        label.getMinimumHorizontalScale());

        g.setColour(label.findColour(juce::Label::outlineColourId).withMultipliedAlpha(alpha));
    }
    else if (label.isEnabled())
    {
        g.setColour(label.findColour(juce::Label::outlineColourId));
    }

    g.drawRect(label.getLocalBounds());
}

//==============================================================================
void KingsCabLookAndFeel::fillResizableWindowBackground(juce::Graphics& g, int w, int h,
                                                        const juce::BorderSize<int>& border,
                                                        juce::ResizableWindow& window)
{
    juce::ignoreUnused(border, window);
    
    // Premium gradient background
    g.setGradientFill(juce::ColourGradient(
        darkBackground.brighter(0.1f), 0, 0,
        darkBackground.darker(0.2f), 0, static_cast<float>(h), false));
    g.fillAll();
    
    // Subtle texture overlay
    g.setColour(metallicBase.withAlpha(0.1f));
    for (int y = 0; y < h; y += 2)
    {
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(w));
    }
}

//==============================================================================
void KingsCabLookAndFeel::drawMetallicGradient(juce::Graphics& g, const juce::Rectangle<int>& bounds, 
                                              bool isPressed, bool isHighlighted)
{
    auto floatBounds = bounds.toFloat();
    
    // Base shadow
    g.setColour(metallicShadow);
    g.fillRoundedRectangle(floatBounds, 4.0f);
    
    // Main gradient
    auto topColour = isPressed ? metallicBase : metallicHighlight;
    auto bottomColour = isPressed ? metallicShadow : metallicBase;
    
    if (isHighlighted)
    {
        topColour = topColour.brighter(0.2f);
        bottomColour = bottomColour.brighter(0.1f);
    }
    
    g.setGradientFill(juce::ColourGradient(topColour, floatBounds.getTopLeft(),
                                          bottomColour, floatBounds.getBottomRight(), false));
    g.fillRoundedRectangle(floatBounds.reduced(1), 3.0f);
    
    // Glossy overlay
    drawGlossyOverlay(g, bounds, 3.0f);
}

void KingsCabLookAndFeel::drawGoldAccent(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    auto floatBounds = bounds.toFloat();
    
    // Gold border
    g.setColour(goldBase);
    g.drawRoundedRectangle(floatBounds.reduced(1), 3.0f, 1.5f);
    
    // Inner gold highlight
    g.setColour(goldHighlight.withAlpha(0.3f));
    g.fillRoundedRectangle(floatBounds.reduced(2), 3.0f);
}

void KingsCabLookAndFeel::draw3DFrame(juce::Graphics& g, const juce::Rectangle<int>& bounds, 
                                     bool isInset, float cornerSize)
{
    auto floatBounds = bounds.toFloat();
    
    auto topLeftColour = isInset ? metallicShadow : metallicHighlight;
    auto bottomRightColour = isInset ? metallicHighlight : metallicShadow;
    
    // Draw frame with 3D effect
    g.setColour(topLeftColour);
    g.drawRoundedRectangle(floatBounds, cornerSize, 1.0f);
    
    g.setColour(bottomRightColour);
    g.drawRoundedRectangle(floatBounds.reduced(1), cornerSize, 1.0f);
}

void KingsCabLookAndFeel::drawGlossyOverlay(juce::Graphics& g, const juce::Rectangle<int>& bounds, 
                                           float cornerSize)
{
    auto floatBounds = bounds.toFloat();
    auto glossyArea = floatBounds.removeFromTop(floatBounds.getHeight() * 0.4f);
    
    g.setGradientFill(juce::ColourGradient(
        juce::Colours::white.withAlpha(0.15f), glossyArea.getTopLeft(),
        juce::Colours::white.withAlpha(0.05f), glossyArea.getBottomLeft(), false));
    
    g.fillRoundedRectangle(glossyArea.reduced(2), cornerSize);
}