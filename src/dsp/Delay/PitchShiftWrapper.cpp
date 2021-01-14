#include "PitchShiftWrapper.h"

PitchShiftWrapper::PitchShiftWrapper()
{
    processFunc = &PitchShiftWrapper::processSampleInternal;
}

void PitchShiftWrapper::prepare (const dsp::ProcessSpec& spec)
{
    jassert (spec.numChannels == 1);
    shifter.prepare (spec);
    pitchStSmooth.reset (spec.sampleRate, 0.05);
    crossfade.reset (spec.sampleRate, 0.05);
}

void PitchShiftWrapper::reset()
{
    shifter.reset();
}

void PitchShiftWrapper::setPitchSemitones (float pitch, bool force)
{
    if (force)
    {
        shifter.setShiftSemitones (pitch);
        pitchStSmooth.setCurrentAndTargetValue (pitch);
    }
    else
    {
        pitchStSmooth.setTargetValue (pitch);
    }

    const auto current = pitchStSmooth.getCurrentValue();
    const auto target = pitchStSmooth.getTargetValue();
    if (target == 0.0f && current == 0.0f) // bypass
    {
        shifter.reset();
        crossfade.setCurrentAndTargetValue (0.0f);
        processFunc = &PitchShiftWrapper::processSampleInternalBypass;
    }
    else if (target == 0.0f && current != 0.0f) // fade to bypass
    {
        crossfade.setTargetValue (0.0f);
        processFunc = &PitchShiftWrapper::processSampleInternalFade;
    }
    else if (target != 0.0f && current == 0.0f) // fade from bypass
    {
        crossfade.setTargetValue (1.0f);
        processFunc = &PitchShiftWrapper::processSampleInternalFade;
    }
    else if (pitchStSmooth.isSmoothing()) // smooth pitch change
    {
        crossfade.setCurrentAndTargetValue (1.0f);
        processFunc = &PitchShiftWrapper::processSampleInternalSmooth;
    }
    else // normal processing!
    {
        crossfade.setCurrentAndTargetValue (1.0f);
        processFunc = &PitchShiftWrapper::processSampleInternal;
    }
}

inline float PitchShiftWrapper::processSampleInternal (float x) noexcept
{
    return shifter.processSample (0, x);
}

inline float PitchShiftWrapper::processSampleInternalSmooth (float x) noexcept
{
    shifter.setShiftSemitones (pitchStSmooth.getNextValue());
    return shifter.processSample (0, x);
}

inline float PitchShiftWrapper::processSampleInternalFade (float x) noexcept
{
    shifter.setShiftSemitones (pitchStSmooth.getNextValue());
    auto wetGain = crossfade.getNextValue();
    return wetGain * shifter.processSample (0, x) + (1.0f - wetGain) * x;
}

inline float PitchShiftWrapper::processSampleInternalBypass (float x) noexcept
{
    return x;
}
