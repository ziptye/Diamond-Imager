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
    
    background = juce::ImageCache::getFromMemory(BinaryData::FDImager2_png, BinaryData::FDImager2_pngSize);
    
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
//    g.drawRect(593, 172, 50, 26);
 
    // Insert spaces between each character
    juce::String modifiedR = juce::String(rotation);
    juce::String spacedString;
    for (int i = 0; i < modifiedR.length(); ++i)
    {
        spacedString += modifiedR[i];
        if (i < modifiedR.length() - 1)
            spacedString += " "; // Append space between numbers
    }
    g.setFont(juce::Font(font).withHeight(20.0f));
    g.setColour(juce::Colours::black);
    g.drawText(spacedString, 593, 172, 50, 26, juce::Justification::centred);
    
    
    juce::String modifiedW = juce::String(width);
    juce::String spacedString2;
    for (int i = 0; i < modifiedW.length(); ++i)
    {
        spacedString2 += modifiedW[i];
        if (i < modifiedW.length() - 1)
            spacedString2 += " "; // Append space between numbers
    }
    g.setFont(juce::Font(font).withHeight(20.0f));
    g.setColour(juce::Colours::black);
    g.drawText(spacedString2, 493, 68, 50, 26, juce::Justification::centred);
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
