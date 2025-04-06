/*
  ==============================================================================

    VectorscopeComponent.cpp
    Created: 15 Mar 2025 1:06:23pm
    Author:  Zachary Pennington

  ==============================================================================
*/

#include "VectorscopeComponent.h"

VectorscopeComponent::VectorscopeComponent()
{
    sampleBuffer.setSize(2, bufferSize);
    sampleBuffer.clear();
    writePosition = 0;
    startTimerHz(30);
}

VectorscopeComponent::~VectorscopeComponent()
{
    stopTimer();
}

void VectorscopeComponent::pushSamples(const float* leftSamples, const float* rightSamples, int numSamples)
{
//    // Ensure we don't exceed buffer size
    int samplesToWrite = juce::jmin(numSamples, bufferSize - writePosition);

    // Copy samples into the buffer
    sampleBuffer.copyFrom(0, writePosition, leftSamples, samplesToWrite); // Left channel
    sampleBuffer.copyFrom(1, writePosition, rightSamples, samplesToWrite); // Right channel

    writePosition += samplesToWrite;
    if (writePosition >= bufferSize)
        writePosition = 0; // Wrap around
    
    // USE FOR MONO SIGNAL GEN.
//    for (int i = 0; i < samplesToWrite; ++i)
//    {
//        float monoValue = std::sin(2.0f * juce::MathConstants<float>::pi * i / samplesToWrite);
//        sampleBuffer.setSample(0, writePosition + i, monoValue);
//        sampleBuffer.setSample(1, writePosition + i, monoValue);
//    }
}


void VectorscopeComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack); // Background

    // Center of the component
    float centerX = getWidth() / 2.0f;
    float centerY = getHeight() / 2.0f;
    float scale = juce::jmin(getWidth(), getHeight()) * 0.45f; // Scale to fit

    // Draw grid or axes
//    g.setColour(juce::Colours::red);
//    g.drawLine(centerX, 0, centerX, getHeight(), 1.0f); // Vertical axis (mono reference)
//    g.drawLine(0, centerY, getWidth(), centerY, 1.0f);  // Horizontal axis

    // Plot the vectorscope
    juce::Path path;
    bool firstPoint = true;

    for (int i = 0; i < bufferSize; ++i)
    {
        
        /*
         ===============================================================================================
         TODO: For some reason, switching the left and right channels below fixes the issue where
         the channels were being drawn backwards when soloed. Need to figure out why this was happenning.
         Also need to check to make sure this issue isn't present in the processBlock().
         ===============================================================================================
        */
        
        float right = sampleBuffer.getSample(0, i);  // Left channel
        float left = sampleBuffer.getSample(1, i); // Right channel

        // Rotated coordinates (45-degree turn)
        float stereoDiff = (left - right) * 0.7071f; // ≈ 1/√2, horizontal spread
        float monoSum = (left + right) * 0.7071f;    // ≈ 1/√2, vertical movement

        float x = centerX + (stereoDiff * scale);    // X varies with stereo difference
        float y = centerY - (monoSum * scale);       // Y varies with mono sum

        if (firstPoint) { path.startNewSubPath(x, y); firstPoint = false; }
        else { path.lineTo(x, y); }
    }
    
    // Radial gradient from center
    juce::ColourGradient gradient(juce::Colours::white, centerX, centerY,
                                 juce::Colours::cyan, centerX + scale, centerY, true);
    g.setGradientFill(gradient);

    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void VectorscopeComponent::resized()
{
}

void VectorscopeComponent::timerCallback()
{
    repaint();
}
