#pragma once

#include <pch.h>

/** Wrapper for chowdsp::PitchShift */
class PitchShiftWrapper
{
public:
    PitchShiftWrapper();

    void prepare (const dsp::ProcessSpec& spec);
    void reset();
    void setPitchSemitones (float pitch, bool force = false);

    inline float processSample (float x) noexcept
    {
        return (this->*processFunc) (x);
    }

private:
    inline float processSampleInternal (float x) noexcept;
    inline float processSampleInternalSmooth (float x) noexcept;
    inline float processSampleInternalFade (float x) noexcept;
    inline float processSampleInternalBypass (float x) noexcept;

    using Processor = float (PitchShiftWrapper::*) (float);
    Processor processFunc;

    SmoothedValue<float, ValueSmoothingTypes::Linear> pitchStSmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> crossfade;
    chowdsp::PitchShifter<float, chowdsp::DelayLineInterpolationTypes::Lagrange3rd> shifter { 4096, 256 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchShiftWrapper)
};
