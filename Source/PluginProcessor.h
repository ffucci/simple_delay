/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "BasicDelay.h"

//==============================================================================
/**
*/
class DelayPluginAudioProcessor  : public AudioProcessor, public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    DelayPluginAudioProcessor();
    ~DelayPluginAudioProcessor();

   
    void parameterChanged (const String &parameterID, float newValue) override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void extracted(int bufferLength, int channel, int delayBufferLength, const float *readPointer);
    
void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void fillDelayBufferData(int bufferLength, int channel, int delayBufferLength, const float *readPointer);
    
    void getFromDelayBuffer(AudioBuffer<float>& buffer, int channel, const float* bufferData, const float* delayData, const int delayBufferLength, const int bufferLength);
    
    void feedbackDelay(int channel,
                       const int bufferLenght, const int delayBufferLength,
                       float* dryBuffer);
    
    AudioProcessorValueTreeState tree;
    
private:
   
    AudioProcessorValueTreeState::ParameterLayout getParameterLayout();

    //==============================================================================
    AudioBuffer<float> mDelayBuffer;
    int mWritePosition {0};
    int mSampleRate;
    
    BasicDelay mSimpleDelay;
    LinearSmoothedValue<float> timeSmoothed;
    float* timeAmount;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayPluginAudioProcessor)
};
