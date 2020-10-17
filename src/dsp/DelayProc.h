#pragma once

#include <JuceHeader.h>

class DelayProc
{
public:
    DelayProc() = default;

    void prepare (const dsp::ProcessSpec& spec);
    void reset();

    template<typename ProcessContext>
    void process (const ProcessContext& context);

    void setDelay (float delayMs);
    void setFeedback (float newFeedback);

private:
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Lagrange3rd> delay { 1 << 21 };

    float fs = 44100.0f;
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> feedback;
    std::vector<float> state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProc)
};
