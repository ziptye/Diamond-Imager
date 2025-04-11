/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VectorScopeAudioProcessorEditor::VectorScopeAudioProcessorEditor (VectorScopeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(vectorscope);
    setSize (700, 395);
    
    background = juce::ImageCache::getFromMemory(BinaryData::FDImager7_png, BinaryData::FDImager7_pngSize);
    
    // Register this editor as a listener to the APVTS
    audioProcessor.apvts.addParameterListener("soloLeft", this);
    audioProcessor.apvts.addParameterListener("soloCenter", this);
    audioProcessor.apvts.addParameterListener("soloRight", this);

    // Ensure editor is repaintable when parameters change
    setRepaintsOnMouseActivity(true);
    
}

VectorScopeAudioProcessorEditor::~VectorScopeAudioProcessorEditor()
{
    // Remove listener on destruction
    audioProcessor.apvts.removeParameterListener("soloLeft", this);
    audioProcessor.apvts.removeParameterListener("soloCenter", this);
    audioProcessor.apvts.removeParameterListener("soloRight", this);
}

//==============================================================================
void VectorScopeAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto font = juce::Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize);
    
     
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    
    // Draws background image to screen
    g.drawImageAt(background, 0, 0);
    
    
    // Draws LED lights to screen
    g.setColour(audioProcessor.ledOnLParam-> load() > 0.5f ? juce::Colours::red : juce::Colours::darkred);
    g.fillEllipse(ledBoundsL.toFloat());
    
    g.setColour(audioProcessor.ledOnCParam-> load() > 0.5f? juce::Colours::red : juce::Colours::darkred);
    g.fillEllipse(ledBoundsC.toFloat());
    
    g.setColour(audioProcessor.ledOnRParam-> load() > 0.5f ? juce::Colours::red : juce::Colours::darkred);
    g.fillEllipse(ledBoundsR.toFloat());
    
    g.setColour(juce::Colours::red);
    g.fillEllipse(led1L.toFloat());
    
    g.setColour(juce::Colours::red);
    g.fillEllipse(led1R.toFloat());
    
    g.setColour(juce::Colours::red);
    g.fillEllipse(led2L.toFloat());
    
    g.setColour(juce::Colours::red);
    g.fillEllipse(led2R.toFloat());
    
    g.setColour(juce::Colours::orange);
    g.fillEllipse(led3L.toFloat());
    
    g.setColour(juce::Colours::orange);
    g.fillEllipse(led3R.toFloat());
    
    g.setColour(juce::Colours::orange);
    g.fillEllipse(led4L.toFloat());
    
    g.setColour(juce::Colours::orange);
    g.fillEllipse(led4R.toFloat());
    
    g.setColour(juce::Colours::yellow);
    g.fillEllipse(led5L.toFloat());
    
    g.setColour(juce::Colours::yellow);
    g.fillEllipse(led5R.toFloat());
    
    g.setColour(juce::Colours::yellow);
    g.fillEllipse(led6L.toFloat());
    
    g.setColour(juce::Colours::yellow);
    g.fillEllipse(led6R.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led7L.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led7R.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led8L.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led8R.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led9L.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led9R.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led10L.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led10R.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led11L.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led11R.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led12L.toFloat());
    
    g.setColour(juce::Colours::limegreen);
    g.fillEllipse(led12R.toFloat());
    
    // Draws W/R values to screen
    auto resultRotation = displayValues(width); // Width Value
    g.setFont(juce::FontOptions(font).withHeight(20.0f));
    g.setColour(juce::Colours::black);
    g.drawText(resultRotation, 593, 172, 50, 26, juce::Justification::centred);
    
    auto resultWidth = displayValues(rotation); // Rotation Value
    g.setFont(juce::FontOptions(font).withHeight(20.0f));
    g.setColour(juce::Colours::black);
    g.drawText(resultWidth, 493, 68, 50, 26, juce::Justification::centred);
}

void VectorScopeAudioProcessorEditor::resized()
{
    // Defines the area for the vectorscope (within the diamond)
    int totalWidth = getWidth();
    int totalHeight = getHeight();
    
    // Leave space for the right panel (1/4 width) and bottom buttons (1/10 height)
    int scopeWidth = totalWidth * 0.35f;    // 35% of width
    int scopeHeight = totalHeight * 0.7f;   // 70% of height
    int xOffset = (totalWidth - scopeWidth) / 2 - 128;  // Center horizontally
    int yOffset = (totalHeight - scopeHeight) / 10 + 4; // Leave space at bottom

    // Sets the vectorscope bounds
    vectorscope.setBounds(xOffset, yOffset, scopeWidth, scopeHeight);
}

void VectorScopeAudioProcessorEditor::pushSamples(const float* leftSamples, const float* rightSamples, int numSamples)
{
    vectorscope.pushSamples(leftSamples, rightSamples, numSamples);
}

juce::String VectorScopeAudioProcessorEditor::displayValues(int val)
{
    juce::String modifiedR = juce::String(val).paddedLeft('0', 3); // Prepends zeros to String.
    juce::String spacedString;
    
    for (int i = 0; i < modifiedR.length(); ++i)
    {
        spacedString += modifiedR[i];
        if (i < modifiedR.length() - 1)
            spacedString += " "; // Append space between numbers
    }
    return spacedString;
}

void VectorScopeAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    // Get click position
    juce::Point<int> clickPos = event.getPosition();

    // Check if the click is inside one of the defined areas
    if (area1.contains(clickPos))
    {
        bool currentState = audioProcessor.ledOnLParam->load() > 0.5f;
        audioProcessor.apvts.getParameter("soloLeft")->setValueNotifyingHost(currentState ? 0.0f : 1.0f);
    }
    else if (area2.contains(clickPos))
    {
        bool currentState = audioProcessor.ledOnCParam->load() > 0.5f;
        audioProcessor.apvts.getParameter("soloCenter")->setValueNotifyingHost(currentState ? 0.0f : 1.0f);
    }
    else if (area3.contains(clickPos))
    {
        bool currentState = audioProcessor.ledOnRParam->load() > 0.5f;
        audioProcessor.apvts.getParameter("soloRight")->setValueNotifyingHost(currentState ? 0.0f : 1.0f);
    }
    else if (rotationUp.contains(clickPos) && (rotation < 100))
    {
        rotation += 1;
        repaint();
    }
    else if (rotationDown.contains(clickPos) && (rotation > 0))
    {
        rotation -= 1;
        repaint();
    }
    else if (widthUp.contains(clickPos) && (width < 200))
    {
        width += 1;
        repaint();
    }
    else if (widthDown.contains(clickPos) && (width > 0))
    {
        width -= 1;
        repaint();
    }
}

void VectorScopeAudioProcessorEditor::parameterChanged(const juce::String &parameterID, float newValue)
{
    repaint();
}
