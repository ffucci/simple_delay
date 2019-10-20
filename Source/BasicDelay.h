/*
  ==============================================================================

    BasicDelay.h
    Created: 6 Oct 2019 3:21:22pm
    Author:  Francesco Fucci

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LinearInterpolation{};

class BasicDelay {
    
public:
    BasicDelay() = default;
    
    void setParameters(int sampleRate, int numInputChannels, int delayBufferSize, float initialFeedback){
        mSampleRate = sampleRate;
        mDelayBuffer.setSize(numInputChannels, delayBufferSize, false, true ,true);
        mDelayBuffer.clear();
        for(int i = 0; i< numInputChannels; ++i){
            delaySmoothed[i].reset(sampleRate, 0.3);
            feedbackSmoothed[i].reset(sampleRate, 0.3);
        }
        
        mWritePosition = 0;
    }
    
    template<class T>
    void processBlock(int channel, AudioBuffer<float> &buffer,
                      float* timeAmount, float* currentMix, float* feedbackAmount) noexcept;
    
    inline void updatePosition(int size) noexcept{
        mWritePosition += size;
        if(mWritePosition >= mDelayBuffer.getNumSamples()){
            mWritePosition -= mDelayBuffer.getNumSamples();
        }
    }
    

    
private:
    
    AudioBuffer<float> mDelayBuffer;
    int mSampleRate;
    int mWritePosition {0};
    const float delta = 0.3f;
    
    LinearSmoothedValue<float> delaySmoothed[2];
    LinearSmoothedValue<float> feedbackSmoothed[2];



};
