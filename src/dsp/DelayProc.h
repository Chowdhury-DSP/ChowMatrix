#pragma once

#include <JuceHeader.h>

class DelayProc
{
public:
    DelayProc() = default;

    void prepare (const dsp::ProcessSpec& spec)
    {
        delay.prepare (spec);
        fs = (float) spec.sampleRate;
    }

    void reset()
    {
        delay.reset();
    }

    template<typename ProcessContext>
    void process (const ProcessContext& context)
    {
        delay.process (context);
    }

    void setDelay (float delayMs)
    {
        delay.setDelay ((delayMs / 1000.0f) * fs);
    }

private:
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Lagrange3rd> delay { 1 << 21 };

    float fs = 44100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProc)
};
