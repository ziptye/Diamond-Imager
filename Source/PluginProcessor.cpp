/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VectorScopeAudioProcessor::VectorScopeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Diamond Imager Params", createParameterLayout())
#endif
{
    processorBuffer.setSize(2, bufferSize);
    processorBuffer.clear();
    
    ledOnLParam = apvts.getRawParameterValue("soloLeft");
    ledOnCParam = apvts.getRawParameterValue("soloCenter");
    ledOnRParam = apvts.getRawParameterValue("soloRight");
}

VectorScopeAudioProcessor::~VectorScopeAudioProcessor()
{
}

//==============================================================================
const juce::String VectorScopeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VectorScopeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VectorScopeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VectorScopeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VectorScopeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VectorScopeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VectorScopeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VectorScopeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VectorScopeAudioProcessor::getProgramName (int index)
{
    return {};
}

void VectorScopeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VectorScopeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    processorBuffer.clear();
    writePosition = 0;
}

void VectorScopeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VectorScopeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VectorScopeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    // Ensure we have at least 1 channel
    if (numChannels == 0)
    {
        DBG("!!! WARNING: NUMCHANNELS == 0 !!!");
        return;
    }

    auto* leftChannel = buffer.getWritePointer(0); // Always use channel 0
    auto* rightChannel = (numChannels > 1) ? buffer.getWritePointer(1) : leftChannel; // Use left for mono
    

    bool soloLeft = *ledOnLParam > 0.5f;   // Treat as bool (0.0f = false, 1.0f = true)
    bool soloCenter = *ledOnCParam > 0.5f;
    bool soloRight = *ledOnRParam > 0.5f;
    bool anySoloActive = soloLeft || soloCenter || soloRight;

    if (anySoloActive)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float leftSample = leftChannel[sample];
            float rightSample = (numChannels > 1) ? rightChannel[sample] : leftSample;
            float centerSample = (leftSample + rightSample) * 0.5f;

            float leftOutput = 0.0f;
            float rightOutput = 0.0f;

            if (soloLeft) leftOutput += leftSample;
            if (soloCenter && !soloLeft && !soloRight)
            {
                leftOutput += centerSample;
                rightOutput += centerSample;
            }
            if (soloRight) rightOutput += rightSample;

            leftChannel[sample] = leftOutput;
            if (numChannels > 1) rightChannel[sample] = rightOutput;
        }
    }
    
    pushSamplesToEditor(leftChannel, rightChannel, numSamples);
    
    // Clear unused output channels if more outputs than inputs
    for (int channel = numChannels; channel < getTotalNumOutputChannels(); ++channel)
    {
        buffer.clear(channel, 0, numSamples);
    }
    
#if JUCE_DEBUG
    protectYourEars(buffer);
#endif
}

void VectorScopeAudioProcessor::pushSamplesToEditor(const float* leftSamples, const float* rightSamples, int numSamples)
{
    int samplesToWrite = juce::jmin(numSamples, bufferSize - writePosition);

    // Copy to processorBuffer, duplicating mono to both channels
    for (int i = 0; i < samplesToWrite; ++i)
    {
        processorBuffer.setSample(0, writePosition + i, leftSamples[i]);
        processorBuffer.setSample(1, writePosition + i, (leftSamples == rightSamples) ? leftSamples[i] : rightSamples[i]);
    }

    writePosition += samplesToWrite;

    if (writePosition >= bufferSize)
    {
        if (auto* editor = dynamic_cast<VectorScopeAudioProcessorEditor*>(getActiveEditor()))
        {
            editor->pushSamples(processorBuffer.getReadPointer(0), processorBuffer.getReadPointer(1), bufferSize);
        }
        writePosition = 0;
    }
}
//==============================================================================
bool VectorScopeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VectorScopeAudioProcessor::createEditor()
{
    return new VectorScopeAudioProcessorEditor (*this);
}

//==============================================================================
void VectorScopeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VectorScopeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout VectorScopeAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("soloLeft", 1), "Solo Left", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("soloCenter", 1), "Solo Center", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("soloRight", 1), "Solo Right", false));
    
    return {    params.begin(), params.end()    };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VectorScopeAudioProcessor();
}
