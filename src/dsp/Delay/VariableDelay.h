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
    inline void setDelay (float newDelayInSamples) noexcept
    {
        delaySmooth.setTargetValue (newDelayInSamples);
    }

    void setDelayForce (float newDelayInSamples) noexcept;
    void setDelayType (DelayType newType);
    bool isDelaySmoothing() const { return delaySmooth.isSmoothing(); }

    // delegate everything else to dsp::DelayLine
    void prepare (const juce::dsp::ProcessSpec& spec);
    void reset();

    inline void pushSample (int channel, float sample)
    {
        delays[type]->pushSample (channel, sample);
    }

    inline void pushSampleSmooth (int channel, float sample)
    {
        delays[type]->setDelay (delaySmooth.getNextValue());
        delays[type]->pushSample (channel, sample);
    }

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

    SmoothedValue<float, ValueSmoothingTypes::Linear> delaySmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariableDelay)
};
