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
        Sinc16,
        Sinc32,
        BBDShort,
        BBDLong,
        BBDAlien,
    };

    // manage parameters
    inline void setDelay (float newDelayInSamples) noexcept
    {
        delaySmooth.setTargetValue (newDelayInSamples);
    }

    // call this at the start of processinf a block of data
    void delayBlockStart() noexcept;

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

    inline float popSample (int channel)
    {
        return makeupGain * delays[type]->popSample (channel);
    }

private:
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::None> l0Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Linear> l1Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Lagrange3rd> l3Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Lagrange5th> l5Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Sinc<float, 16>> sinc16Delay;
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Sinc<float, 32>> sinc32Delay;
    chowdsp::BBD::BBDDelayWrapper<4096> bbdShortDelay;
    chowdsp::BBD::BBDDelayWrapper<16384> bbdLongDelay;
    chowdsp::BBD::BBDDelayWrapper<8192, true> bbdAlienDelay;

    std::array<chowdsp::DelayLineBase<float>*, 9> delays { &l0Delay, &l1Delay, &l3Delay, &l5Delay,
        &sinc16Delay, &sinc32Delay, &bbdShortDelay, &bbdLongDelay, &bbdAlienDelay };
    DelayType type = ThirdInterp;

    SmoothedValue<float, ValueSmoothingTypes::Linear> delaySmooth;
    float makeupGain = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariableDelay)
};
