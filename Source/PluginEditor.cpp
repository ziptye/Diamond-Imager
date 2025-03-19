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
    
    background = juce::ImageCache::getFromMemory(BinaryData::FDImager4_png, BinaryData::FDImager4_pngSize);
    
}

VectorScopeAudioProcessorEditor::~VectorScopeAudioProcessorEditor()
{
}

//==============================================================================
void VectorScopeAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto font = juce::Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize);
    
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    g.drawImageAt(background, 0, 0);
    
//    g.setColour(juce::Colours::red);
//    g.drawRect(493, 68, 50, 26); // ROTATION COORDS.
    
//    g.setColour(juce::Colours::red);
//    g.drawRect(593, 172, 50, 26); // WIDTH COORDS,
    
//    g.setColour(juce::Colours::red);
//    g.drawEllipse(93, 330, 31, 31, 2.f); // LEFT COORDS.
//    
//    g.setColour(juce::Colours::red);
//    g.drawEllipse(206, 330, 31, 31, 2.f); // CENTER COORDS.
//    
//    g.setColour(juce::Colours::red);
//    g.drawEllipse(319, 330, 31, 31, 2.f); // RIGHT COORDS.
    
    auto resultRotation = displayValues(width); // Width Value
    g.setFont(juce::Font(font).withHeight(20.0f));
    g.setColour(juce::Colours::black);
    g.drawText(resultRotation, 593, 172, 50, 26, juce::Justification::centred);
    
    
    auto resultWidth = displayValues(rotation); // Rotation Value
    g.setFont(juce::Font(font).withHeight(20.0f));
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
    // Define clickable areas
    juce::Rectangle<int> area1(93, 330, 31, 31);  // L
    juce::Rectangle<int> area2(206, 330, 31, 31); // C
    juce::Rectangle<int> area3(319, 330, 31, 31); // R

    // Get click position
    juce::Point<int> clickPos = event.getPosition();

    // Check if the click is inside one of the defined areas
    if (area1.contains(clickPos))
    {
        { DBG("Area 1 Clicked!"); /* Call another function here */ }
    }
    else if (area2.contains(clickPos))
    {
        { DBG("Area 2 Clicked!"); /* Call another function here */ }
    }
    else if (area3.contains(clickPos))
    {
        { DBG("Area 3 Clicked!"); /* Call another function here */ }
    }
}
