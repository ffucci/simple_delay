/*
  ==============================================================================

    SpectroscopeComponent.cpp
    Created: 1 Oct 2019 10:26:09pm
    Author:  Francesco Fucci

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectroscopeComponent.h"

//==============================================================================
SpectroscopeComponent::SpectroscopeComponent()
: m_fifoIndex(0),
m_fftBlockReady(false),
m_forwardFFT(kFFTOrder),
m_window(kFFTSize, juce::dsp::WindowingFunction<float>::hann),
m_strokeColour(Colours::white),
m_fillStartColour(Colours::white.withAlpha(0.2f)),
m_fillStopColour(Colours::white.withAlpha(0.8f))
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    //zeromem(m_outputData, sizeof(m_outputData));
    m_outputData.fill(0);
    setSize(700,200);
    startTimerHz(30);
}

SpectroscopeComponent::~SpectroscopeComponent()
{
    stopTimer();
}

void SpectroscopeComponent::paint (Graphics& g)
{
    const float width = (float) getWidth();
    const float height = (float) getHeight();
    
    g.setColour (Colours::transparentBlack);

    g.fillAll ();   // clear the background

    auto maxBin = FloatVectorOperations::findMinAndMax(m_outputData.data(), kOutputSize);
    
    const float scale = 1.0f / jmax((float) kFFTSize, maxBin.getEnd());
    
    g.setColour(m_fillStartColour);
    
    for(int i = 0; i < kOutputSize; ++i){
        float x = std::log10(1 + 39* ((i + 1.0f)/ kOutputSize)) / std::log10(40.0f) * width;
        const float yMag = scale * m_outputData[i];
        const float yDecibel = Decibels::gainToDecibels(yMag);
        const float y = jmap(yDecibel,-90.0f,-12.0f,height,0.0f);
        
        g.drawVerticalLine((int) x, y, height);
    }
    
}

void SpectroscopeComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void SpectroscopeComponent::timerCallback() {
    //Logger::outputDebugString("TIMER");

    if(m_fftBlockReady){
        m_window.multiplyWithWindowingTable(m_fftData.data(), kFFTSize);
        
    m_forwardFFT.performFrequencyOnlyForwardTransform(m_fftData.data());
        
    FloatVectorOperations::max(m_outputData.data(),m_outputData.data() ,m_fftData.data(),kOutputSize);
        m_fftBlockReady = false;
            
    }
    
    for(int i = 0; i < kOutputSize; ++i){
        m_outputData[i] *= 0.707f;
    }
    
    repaint();
}

void SpectroscopeComponent::pushBuffer(AudioSampleBuffer &buffer)
{
    if(buffer.getNumChannels() > 0){
        const int numSamples = buffer.getNumSamples();
        auto* channelData = buffer.getReadPointer(0);
        
        for(int i = 0; i< numSamples; ++i){
            pushSample(channelData[i]);
        }
    }
    
}

void SpectroscopeComponent::pushSample(float sample){
    if(m_fifoIndex == kFFTSize){
        if(!m_fftBlockReady){
            //Logger::outputDebugString("Push sample");
            
            m_fftData.fill(0);
            std::copy(std::begin(m_fifo),std::end(m_fifo), std::begin(m_fftData));
            m_fftBlockReady = true;
        }
        
        m_fifoIndex = 0;
    }
    
    m_fifo[m_fifoIndex++] = sample;
}

void SpectroscopeComponent::setColours(Colour strokeColour, Colour fillStartColour, Colour fillStopColour)
{
    m_strokeColour = strokeColour;
    m_fillStartColour = fillStartColour;
    m_fillStopColour = fillStopColour;
}
