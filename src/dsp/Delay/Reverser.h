#pragma once

#include <pch.h>

/**
 * Time reversal effect, based on this paper: https://ccrma.stanford.edu/~hskim08/files/pdf/DAFx14_sttr.pdf
 */
class Reverser
{
public:
    Reverser() = default;

    void prepare (const dsp::ProcessSpec& spec);
    void reset();
    void setReverseTime (float revTimeMs);

    inline float processSample (float x)
    {
        if (bypass)
            return x;

        // read output sample from reverse buffer
        float y = 0.0f;
        y += revBuffPtr[bufferRead1] * window[bufferRead1 % windowSize];
        y += revBuffPtr[bufferRead2] * window[bufferRead2 % windowSize];

        // update read pointers
        bufferRead1--;
        bufferRead1 = bufferRead1 > 0 ? bufferRead1 : doubleWindowSize;

        bufferRead2--;
        bufferRead2 = bufferRead2 > halfWindowSize ? bufferRead2 : doubleWindowSize + halfWindowSize;

        // push new sample into reverse buffer
        revBuffPtr[bufferWrite] = x;
        revBuffPtr[bufferWrite + doubleWindowSize] = x;

        // update write pointers
        bufferWrite++;
        bufferWrite = bufferWrite >= doubleWindowSize ? 0 : bufferWrite;

        return y;
    }

private:
    bool bypass = false;

    AudioBuffer<float> reverseBuffer;
    float* revBuffPtr = nullptr;

    float fs = 48000.0f;
    int maxWindowSize = 8192;

    AudioBuffer<float> windowBuffer;
    float* window = nullptr;

    int bufferWrite, bufferRead1, bufferRead2;
    int windowSize, halfWindowSize, doubleWindowSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Reverser)
};
