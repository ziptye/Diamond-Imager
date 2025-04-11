/*
  ==============================================================================

    VectorscopeComponent.h
    Created: 15 Mar 2025 1:06:23pm
    Author:  Zachary Pennington

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class VectorscopeComponent : public juce::Component, public juce::Timer
{
public:
    VectorscopeComponent();
    ~VectorscopeComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Called by the audio thread to push new samples
    void pushSamples(const float* leftSamples, const float* rightSamples, int numSamples);

private:
    void timerCallback() override;

    // Buffer to store audio samples
    juce::AudioBuffer<float> sampleBuffer;
    int writePosition = 0;
    static constexpr int bufferSize = 1024; // Adjust based on needs

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VectorscopeComponent)
};
