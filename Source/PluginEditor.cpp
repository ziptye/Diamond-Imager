/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VectorScopeAudioProcessorEditor::VectorScopeAudioProcessorEditor (VectorScopeAudioProcessor& p, std::atomic<float>& correlationRef)
: AudioProcessorEditor (&p), audioProcessor (p), correlationValue(correlationRef)
{
    addAndMakeVisible(vectorscope);
    setSize (700, 395);
    
    background = juce::ImageCache::getFromMemory(BinaryData::FDImager8_png, BinaryData::FDImager8_pngSize);
    
    // Register this editor as a listener to the APVTS
    audioProcessor.apvts.addParameterListener("soloLeft", this);
    audioProcessor.apvts.addParameterListener("soloCenter", this);
    audioProcessor.apvts.addParameterListener("soloRight", this);

    // Ensure editor is repaintable when parameters change
    setRepaintsOnMouseActivity(true);
    
    smoothedCorrelation.reset(60.0, 0.07);
    smoothedCorrelation.setCurrentAndTargetValue(0.0f);
    startTimer(30);
}

VectorScopeAudioProcessorEditor::~VectorScopeAudioProcessorEditor()
{
    // Remove listener on destruction
    audioProcessor.apvts.removeParameterListener("soloLeft", this);
    audioProcessor.apvts.removeParameterListener("soloCenter", this);
    audioProcessor.apvts.removeParameterListener("soloRight", this);
    
    stopTimer();
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
    
    
    // Calculate number of LEDs to light
    int numLit = juce::jlimit(0, 6, static_cast<int>(std::round(std::abs(displayVal) * 6.0f)));
    
    // Handle negative correlation
    if (displayVal < 0.0f)
    {
        for (int i = 0; i < 6; ++i)
        {
            int ledIndex = 5 - i; // Reverse index: i=0 -> ledIndex=5, i=1 -> ledIndex=4, etc.
            if (i < numLit)
            {
                if (ledIndex < 3)
                {
                    g.setColour(juce::Colours::red); // LEDs 0–2
                }
                else if (ledIndex < 5)
                {
                    g.setColour(juce::Colours::orange); // LEDs 3–4
                }
                else
                {
                    g.setColour(juce::Colours::yellow); // LEDs 5–6
                }
            }
            else
            {
                g.setColour(juce::Colours::black.withAlpha(0.2f));
            }
            g.fillEllipse(ledsL[ledIndex].toFloat());
            g.fillEllipse(ledsR[ledIndex].toFloat());
        }

        // Turn off positive side LEDs (6–11)
        for (int i = 6; i < 12; ++i)
        {
            g.setColour(juce::Colours::black.withAlpha(0.2f));
            g.fillEllipse(ledsL[i].toFloat());
            g.fillEllipse(ledsR[i].toFloat());
        }
    }
    // Handle positive correlation
    else if (displayVal > 0.0f)
    {
        for (int i = 6; i < 12; ++i)
        {
            if (i - 6 < numLit)
            {
                g.setColour(juce::Colours::limegreen); // LEDs 6–11
            }
            else
            {
                g.setColour(juce::Colours::black.withAlpha(0.2f));
            }
            g.fillEllipse(ledsL[i].toFloat());
            g.fillEllipse(ledsR[i].toFloat());
        }

        // Turn off negative side LEDs (0–5)
        for (int i = 0; i < 6; ++i)
        {
            g.setColour(juce::Colours::black.withAlpha(0.2f));
            g.fillEllipse(ledsL[i].toFloat());
            g.fillEllipse(ledsR[i].toFloat());
        }
    }
    // Handle zero correlation: turn off all LEDs
    else
    {
        for (int i = 0; i < 12; ++i)
        {
            g.setColour(juce::Colours::black.withAlpha(0.2f));
            g.fillEllipse(ledsL[i].toFloat());
            g.fillEllipse(ledsR[i].toFloat());
        }
    }
        
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

void VectorScopeAudioProcessorEditor::timerCallback()
{
    float rawCorrelation = audioProcessor.correlationValue.load();
    smoothedCorrelation.setTargetValue(rawCorrelation);
    displayVal = smoothedCorrelation.getNextValue();
    repaint();
}

void VectorScopeAudioProcessorEditor::parameterChanged(const juce::String &parameterID, float newValue)
{
    repaint();
}
