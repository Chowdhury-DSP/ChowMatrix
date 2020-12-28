#pragma once

#include <pch.h>

/** Wrapper for chowdsp::PitchShift */
class PitchShiftWrapper
{
public:
    PitchShiftWrapper() {}

    void prepare (const dsp::ProcessSpec& spec)
    {
        jassert (spec.numChannels == 1);
        shifter.prepare (spec);
    }

    void reset()
    {
        shifter.reset();
    }

    void setPitchSemitones (float pitch)
    {
        shifter.setShiftSemitones (pitch);
    }

    inline float processSample (float x) noexcept
    {
        return shifter.processSample (0, x);
    }

private:
    chowdsp::PitchShifter<float, chowdsp::DelayLineInterpolationTypes::Lagrange3rd> shifter { 4096, 256 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchShiftWrapper)
};
