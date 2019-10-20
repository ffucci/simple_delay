/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "SpectroscopeComponent.h"

//==============================================================================
/**
*/
class DelayPluginAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DelayPluginAudioProcessorEditor (DelayPluginAudioProcessor&);
    ~DelayPluginAudioProcessorEditor();

    void createSlider(Slider& slider, std::pair<float,float> range, float value);
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    std::unique_ptr<SpectroscopeComponent> preGain;
    
    using slider_attachment_t = std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>;
    
    slider_attachment_t feedbackAmountValue, delayTimeValue, dryWetValue;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayPluginAudioProcessor& processor;

    Slider feedbackAmountSlider, delayTimeSlider, dryWetSlider;
    
    OpenGLContext m_glContext;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayPluginAudioProcessorEditor)
};
