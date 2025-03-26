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
    
    void mouseDown(const juce::MouseEvent& event) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VectorScopeAudioProcessor& audioProcessor;
    VectorscopeComponent vectorscope;
    
    int rotation = 100;
    int width = 100;
    
    juce::Image background;
    
    bool ledOnL = true;
    bool ledOnC = true;
    bool ledOnR = true;
    
    // Define clickable areas
    juce::Rectangle<int> area1 {93, 330, 31, 31};  // L
    juce::Rectangle<int> area2 {206, 330, 31, 31}; // C
    juce::Rectangle<int> area3 {319, 330, 31, 31}; // R
    
    // Define LED lights
    juce::Rectangle<int> ledBoundsL {105, 341, 8, 8}; // L
    juce::Rectangle<int> ledBoundsC {218, 341, 8, 8}; // C
    juce::Rectangle<int> ledBoundsR {331, 341, 8, 8}; // R

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VectorScopeAudioProcessorEditor)
};
