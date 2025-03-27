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
                       )
#endif
{
    processorBuffer.setSize(2, bufferSize);
    processorBuffer.clear();
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
    if (numChannels == 0) return;

    auto* leftChannel = buffer.getWritePointer(0); // Always use channel 0
    const float* rightChannel = (numChannels > 1) ? buffer.getWritePointer(1) : leftChannel; // Use left for mono

    pushSamplesToEditor(leftChannel, rightChannel, numSamples);

    // Pass-through audio (required for Logic validation)
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = channelData[sample]; // No processing, just pass-through
        }
    }

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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VectorScopeAudioProcessor();
}
