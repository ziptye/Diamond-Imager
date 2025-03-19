/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VectorscopeComponent.h"

//==============================================================================
/**
*/
class VectorScopeAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    VectorScopeAudioProcessorEditor (VectorScopeAudioProcessor&);
    ~VectorScopeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void pushSamples(const float* leftSamples, const float* rightSamples, int numSamples);
    juce::String displayValues (int val);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VectorScopeAudioProcessor& audioProcessor;
    VectorscopeComponent vectorscope;
    
    int rotation = 100;
    int width = 100;
    
    juce::Image background;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VectorScopeAudioProcessorEditor)
};
