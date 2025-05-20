/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProtectYourEars.h"

//==============================================================================
/**
*/
class VectorScopeAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    VectorScopeAudioProcessor();
    ~VectorScopeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //================================
    void pushSamplesToEditor(const float* leftSamples, const float* rightSamples, int numSamples);
    
    juce::AudioProcessorValueTreeState apvts;
    
    std::atomic<float>* ledOnLParam = nullptr;
    std::atomic<float>* ledOnCParam = nullptr;
    std::atomic<float>* ledOnRParam = nullptr;
    
    std::atomic<float> correlationValue { 0.0f };
    
    float calculateStereoCorrelation (const float* left, const float* right, int numSamples);

private:
    juce::AudioBuffer<float> processorBuffer;
    int writePosition = 0;
    static constexpr int bufferSize = 1024;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VectorScopeAudioProcessor)
};
