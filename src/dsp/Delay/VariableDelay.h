#pragma once

#include <pch.h>

/**
 * Delay class with variable interpolation type
 */ 
class VariableDelay
{
public:
    VariableDelay (size_t size);

    enum DelayType
    {
        NoInterp,
        LinearInterp,
        ThirdInterp,
        FifthInterp,
    };

    // manage parameters
    void setDelay (float newDelayInSamples);
    float getDelay() const;
    void setDelayType (DelayType newType);

    // delegate everything else to dsp::DelayLine
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    inline void pushSample (int channel, float sample) { delays[type]->pushSample (channel, sample); }

    inline float popSample (int channel, float delayInSamples = -1, bool updateReadPointer = true)
    {
        return delays[type]->popSample (channel, delayInSamples, updateReadPointer);
    }

private:
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::None> l0Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Linear> l1Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Lagrange3rd> l3Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Lagrange5th> l5Delay;

    std::array<chowdsp::DelayLineBase<float>*, 4> delays { &l0Delay, &l1Delay, &l3Delay, &l5Delay };
    DelayType type = ThirdInterp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariableDelay)
};
