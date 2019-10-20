/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayPluginAudioProcessorEditor::DelayPluginAudioProcessorEditor (DelayPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    preGain = std::make_unique<SpectroscopeComponent>();
    addAndMakeVisible(preGain.get());

    preGain->setColours(Colour::fromRGBA(255, 186, 34, 255),
                          Colour::fromRGBA(253, 174, 25, 255).withAlpha(0.7f),
                          Colour::fromRGBA(255, 126, 0, 255).withAlpha(0.7f));
    
    createSlider(feedbackAmountSlider, {0.0,1.0},0.1);
    createSlider(delayTimeSlider, {0.0,1000.0},200.0);
    createSlider(dryWetSlider, {0.0, 1.0},0.0f);

    feedbackAmountValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "feedback", feedbackAmountSlider);
    delayTimeValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree, "time", delayTimeSlider);
    dryWetValue = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.tree,"dry_wet", dryWetSlider);
    
    m_glContext.setComponentPaintingEnabled(true);
    m_glContext.attachTo(*this);
    feedbackAmountSlider.setSkewFactor(0.3f);
    delayTimeSlider.setSkewFactor(150.0f);
    dryWetSlider.setSkewFactor(0.1f);
    setSize (700, 400);

}

void DelayPluginAudioProcessorEditor::createSlider(Slider& slider,
                                                    std::pair<float,float> range,
                                                    float value) {
    
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    slider.setRange(range.first,range.second);
    slider.setValue(value);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    slider.setPopupDisplayEnabled(true , true, this);
    addAndMakeVisible(slider);
    
}

DelayPluginAudioProcessorEditor::~DelayPluginAudioProcessorEditor()
{
    m_glContext.detach();
}

//==============================================================================
void DelayPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::black);
}

void DelayPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    preGain->setBounds(194, 181, 396, 134);
    feedbackAmountSlider.setBounds (30, 90, 70, 70);
    delayTimeSlider.setBounds (30, 150, 70, 70);
    dryWetSlider.setBounds (30, 210, 70, 70);

}
