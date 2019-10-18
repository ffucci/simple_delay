/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayPluginAudioProcessor::DelayPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), tree(*this, nullptr, Identifier ("DelayPlugin"),{ getParameterLayout()})
#endif
{
    //tree.addParameterListener("time",this);
    timeAmount = tree.getRawParameterValue("time");

}

DelayPluginAudioProcessor::~DelayPluginAudioProcessor()
{
}

//==============================================================================
const String DelayPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String DelayPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DelayPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    //const double smoothTime = 0.2;
    const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = 2* (sampleRate + samplesPerBlock);
        
    //timeSmoothed.reset (sampleRate, smoothTime);
    //.reset (sampleRate, smoothTime);
    mSampleRate = sampleRate;
    mDelayBuffer.setSize(numInputChannels, delayBufferSize, false, true ,true);
    mDelayBuffer.clear();
    
    float feedbackAmount = *tree.getRawParameterValue("feedback");
    mSimpleDelay.setParameters(sampleRate, numInputChannels, delayBufferSize,feedbackAmount);
}


void DelayPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void DelayPluginAudioProcessor::parameterChanged(const String &parameterID, float newValue){
    
}

AudioProcessorValueTreeState::ParameterLayout DelayPluginAudioProcessor::getParameterLayout(){
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    auto feedBackAmountParamter = std::make_unique<AudioParameterFloat>("feedback","Feedback",0.0, 1.0f,0.3f);
    auto delayTimeParameter = std::make_unique<AudioParameterFloat>("time","Time",0.0, 1000.0f,300.0f);
    params.push_back(std::move(feedBackAmountParamter));
    params.push_back(std::move(delayTimeParameter));

    return {params.begin(),params.end()};
}

void DelayPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    DelayPluginAudioProcessorEditor* editor = static_cast<DelayPluginAudioProcessorEditor*>(getActiveEditor());
    
    if(editor) { editor->preGain->pushBuffer(buffer); }
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    const int bufferLength = buffer.getNumSamples();
    //const int delayBufferLength = mDelayBuffer.getNumSamples();
    float* feedbackAmount = tree.getRawParameterValue("feedback");
    //float timeAmount = *tree.getRawParameterValue("time");
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //String amount(timeAmount);
        //Logger::outputDebugString(amount);
        mSimpleDelay.processBlock<LinearInterpolation>(channel, buffer, timeAmount, feedbackAmount);
    }
    
    mSimpleDelay.updatePosition(bufferLength);

    // copy to the delay data
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
}



//==============================================================================
bool DelayPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayPluginAudioProcessor::createEditor()
{
    return new DelayPluginAudioProcessorEditor (*this);
}

//==============================================================================
void DelayPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayPluginAudioProcessor();
}
