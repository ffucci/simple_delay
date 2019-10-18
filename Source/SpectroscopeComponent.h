/*
  ==============================================================================
    SpectroscopeComponent.h
    Created: 8 Apr 2017 12:46:51pm
    Author:  Nick Thompson
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <array>

//==============================================================================
/*
*/
class SpectroscopeComponent : public Component , Timer
{
public:
    SpectroscopeComponent();
    ~SpectroscopeComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    
    void pushBuffer(AudioSampleBuffer& buffer);
    inline void pushSample(float sample);
    
    void setColours (Colour strokeColour, Colour fillStartColour, Colour fillStopColour);

    enum {
        kFFTOrder = 11,
        kFFTSize = 1 << kFFTOrder,
        kOutputSize = kFFTSize/2
    };
    
private:
    std::array<float, kFFTSize> m_fifo;
    std::array<float, 2*kFFTSize> m_fftData;
    std::array<float, kOutputSize> m_outputData;
    
    /**float m_fifo[kFFTSize];
    float m_fftData[2*kFFTSize];
    float m_outputData[kOutputSize];**/
    
    unsigned int m_fifoIndex = 0;
    bool m_fftBlockReady;
    
    juce::dsp::FFT m_forwardFFT;
    juce::dsp::WindowingFunction<float> m_window;
    
    Colour m_strokeColour;
    Colour m_fillStartColour;
    Colour m_fillStopColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectroscopeComponent)
};
