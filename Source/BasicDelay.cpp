/*
  ==============================================================================

    BasicDelay.cpp
    Created: 6 Oct 2019 3:21:22pm
    Author:  Francesco Fucci

  ==============================================================================
*/

#include "BasicDelay.h"


template <class T>
void BasicDelay::processBlock(int channel, AudioBuffer<float> &buffer, float* timeAmount, float* currentMix,float* feedbackAmount) noexcept{
    return;
}


template<>
void BasicDelay::processBlock<LinearInterpolation>(int channel, AudioBuffer<float> &buffer, float* timeAmount,
                                                   float* currentMix,
                                                   float* feedbackAmount) noexcept{
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
    int localWritePosition = mWritePosition;
    
    for(auto sample = 0; sample < bufferLength; ++sample){
        const float in = dryBuffer[sample];
        float feedbackCurrentAmount = feedbackSmoothed[channel].getNextValue();
        const float delayTime = delaySmoothed[channel].getNextValue();
        const float fullPosition = delayBufferLength + localWritePosition - (mSampleRate*(delayTime)/1000);
        float readPosition = fmodf(fullPosition, delayBufferLength);

        float out = 0.0f;

        int approxReadPosition = floorf(readPosition);
        int approxReadPosition_1 = (approxReadPosition + 1);
        
        if(approxReadPosition_1 >= delayBufferLength){
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
            dryBuffer[sample] = in + (out - in) * (*currentMix);
            //delayWritePointer[localWritePosition] = in + out*feedbackAmount;
            delayWritePointer[localWritePosition] = in + out*(feedbackCurrentAmount);
        }
        
        if(++localWritePosition >= delayBufferLength){
            localWritePosition -= delayBufferLength;
        }
        
        /**readPosition += 1 - delta;
        
        if(readPosition >= delayBufferLength){
            readPosition -= delayBufferLength;
        }**/
    }

}

