/*
  ==============================================================================

    BasicDelay.cpp
    Created: 6 Oct 2019 3:21:22pm
    Author:  Francesco Fucci

  ==============================================================================
*/

#include "BasicDelay.h"


void BasicDelay::getFromDelayBuffer(AudioBuffer<float>& buffer, int channel, const float* bufferData, const float* delayData, const int delayBufferLength, const int bufferLength, const float delayTime) noexcept{
    const int readPosition = static_cast<int>(delayBufferLength + mWritePosition - (mSampleRate*delayTime/1000)) % delayBufferLength;
    
    if(delayBufferLength > readPosition + bufferLength){
        buffer.copyFrom(channel, 0, delayData + readPosition, bufferLength);
    }else{
        const int remaining = delayBufferLength - readPosition;
        buffer.copyFrom(channel, 0 , delayData + readPosition, remaining);
        buffer.copyFrom(channel, remaining, delayData, bufferLength - remaining);
    }

}


void BasicDelay::feedbackDelay(int channel,
                   const int bufferLenght, const int delayBufferLength,
                   float* dryBuffer,float feedBackAmount) noexcept{
    if(delayBufferLength > bufferLenght + mWritePosition){
        mDelayBuffer.addFromWithRamp(channel, mWritePosition, dryBuffer, bufferLenght, feedBackAmount, feedBackAmount);
    }else{
        const int remaining = delayBufferLength - mWritePosition;
        mDelayBuffer.addFromWithRamp(channel, remaining, dryBuffer, remaining, feedBackAmount, feedBackAmount);
        mDelayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLenght - remaining, feedBackAmount, feedBackAmount);
    }
    
    //Save the old value for the interpolation
    mOldFeedBack = feedBackAmount;
}

void BasicDelay::fillDelayBufferData(int bufferLength, int channel, int delayBufferLength, const float *readPointer) {
    if(delayBufferLength > bufferLength + mWritePosition){
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, readPointer, bufferLength, 0.3, 0.3);
    }else{
        const int bufferRemaining = delayBufferLength - mWritePosition;
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, readPointer, bufferRemaining, 0.3, 0.3);
        mDelayBuffer.copyFromWithRamp(channel, 0, readPointer , bufferLength- bufferRemaining, 0.3, 0.3);
    }
}

template <class T>
void BasicDelay::processBlock(int channel, AudioBuffer<float> &buffer, float* timeAmount, float* feedbackAmount) noexcept{
    return;
}


template<>
void BasicDelay::processBlock<LinearInterpolation>(int channel, AudioBuffer<float> &buffer, float* timeAmount, float* feedbackAmount) noexcept{
    //const auto* bufferPointer = buffer.getReadPointer(channel);
    //const auto* delayPointer = mDelayBuffer.getReadPointer(channel);

    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = mDelayBuffer.getNumSamples();
    delaySmoothed[channel].setTargetValue(*timeAmount);
    feedbackSmoothed[channel].setTargetValue(*feedbackAmount);
    
    auto* dryBuffer = buffer.getWritePointer(channel);
    auto* delayWritePointer = mDelayBuffer.getWritePointer(channel);
    
    //float delayTime = delaySmoothed.getNextValue();
    //float feedbackCurrentAmount = feedbackSmoothed.getNextValue();
    
    //const float nextValue = feedbackSmoothed.getNextValue();
    //String feedbackAm(nextValue);
    //Logger::outputDebugString(feedbackAm);
    float currentMix = 0.5f;
    int localWritePosition = mWritePosition;

    
    for(auto sample = 0; sample < bufferLength; ++sample){
        const float in = dryBuffer[sample];
        float feedbackCurrentAmount = feedbackSmoothed[channel].getNextValue();
        float delayTime = delaySmoothed[channel].getNextValue();
        float fullPosition = delayBufferLength + localWritePosition - (mSampleRate*(delayTime)/1000);
        float readPosition = fmodf(fullPosition, delayBufferLength);
        float out = 0.0f;

        int approxReadPosition = floorf(readPosition);
        int approxReadPosition_1 = (approxReadPosition + 1);
        
        if(approxReadPosition_1 > delayBufferLength){
            approxReadPosition_1 -= delayBufferLength;
        }
        
        // Calculate the fractional delay line (approxReadPosition + 1) % delayBufferLength
        // If the read position equals the write position then the fractional delay is zero 
        if(approxReadPosition != localWritePosition){
            float fraction = readPosition - static_cast<float>(approxReadPosition);
            float delayP1 = delayWritePointer[approxReadPosition];
            float delayP2 = delayWritePointer[approxReadPosition_1];
            out = delayP1 + fraction*(delayP2- delayP1);
            //Write back to the input buffer
            dryBuffer[sample] = in + (out - in) * currentMix;
            //delayWritePointer[localWritePosition] = in + out*feedbackAmount;
            delayWritePointer[localWritePosition] = in + out*(feedbackCurrentAmount);
        }
        
        if(++localWritePosition >= delayBufferLength){
            localWritePosition -= delayBufferLength;
        }
        
        if(++readPosition >= delayBufferLength){
            readPosition -= delayBufferLength;
        }
    }
    
    //delayTime = delaySmoothed[channel].getNextValue();
    //fullPosition = delayBufferLength + mWritePosition - (mSampleRate*(delayTime)/1000);
    //readPosition = fmodf(fullPosition, delayBufferLength);
}

