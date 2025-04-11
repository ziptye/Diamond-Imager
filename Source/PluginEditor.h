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
class VectorScopeAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::AudioProcessorValueTreeState::Listener
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
    VectorScopeAudioProcessor& audioProcessor;
    VectorscopeComponent vectorscope;
    
    int rotation = 0;
    int width = 100;
    
    juce::Image background;
    
    // Define clickable areas
    juce::Rectangle<int> area1 {93, 330, 31, 31};  // L
    juce::Rectangle<int> area2 {206, 330, 31, 31}; // C
    juce::Rectangle<int> area3 {319, 330, 31, 31}; // R
    
    // Define LED lights
    juce::Rectangle<int> ledBoundsL {105, 341, 8, 8}; // L
    juce::Rectangle<int> ledBoundsC {218, 341, 8, 8}; // C
    juce::Rectangle<int> ledBoundsR {331, 341, 8, 8}; // R
    
    // Define the clickable areas for W/R
    juce::Rectangle<int> rotationUp {510, 38, 17, 17}; // Rotation Up
    juce::Rectangle<int> rotationDown {510, 106, 17, 17}; // Rotation Down
    juce::Rectangle<int> widthUp {609, 143, 17, 17}; // Width Up
    juce::Rectangle<int> widthDown {609, 211, 17, 17}; // Width Down
    
    //==========================================================================
    // STEREO CORRLEATION METER LEDS
    // TODO: NEED TO REFACTOR THIS CODE... THIS IS JUST BAD LOL :)
    
    juce::Rectangle<int> led1L {478, 255, 8, 8};
    juce::Rectangle<int> led1R {478, 268, 8, 8};
    
    juce::Rectangle<int> led2L {493, 255, 8, 8};
    juce::Rectangle<int> led2R {493, 268, 8, 8};
    
    juce::Rectangle<int> led3L {508, 255, 8, 8};
    juce::Rectangle<int> led3R {508, 268, 8, 8};
    
    juce::Rectangle<int> led4L {523, 255, 8, 8};
    juce::Rectangle<int> led4R {523, 268, 8, 8};
    
    juce::Rectangle<int> led5L {538, 255, 8, 8};
    juce::Rectangle<int> led5R {538, 268, 8, 8};
    
    juce::Rectangle<int> led6L {553, 255, 8, 8};
    juce::Rectangle<int> led6R {553, 268, 8, 8};
    
    juce::Rectangle<int> led7L {576, 255, 8, 8};
    juce::Rectangle<int> led7R {576, 268, 8, 8};
    
    juce::Rectangle<int> led8L {591, 255, 8, 8};
    juce::Rectangle<int> led8R {591, 268, 8, 8};
    
    juce::Rectangle<int> led9L {606, 255, 8, 8};
    juce::Rectangle<int> led9R {606, 268, 8, 8};
    
    juce::Rectangle<int> led10L {621, 255, 8, 8};
    juce::Rectangle<int> led10R {621, 268, 8, 8};
    
    juce::Rectangle<int> led11L {636, 255, 8, 8};
    juce::Rectangle<int> led11R {636, 268, 8, 8};
    
    juce::Rectangle<int> led12L {651, 255, 8, 8};
    juce::Rectangle<int> led12R {651, 268, 8, 8};
    
    
    //==========================================================================
    // Listener callback
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VectorScopeAudioProcessorEditor)
};
