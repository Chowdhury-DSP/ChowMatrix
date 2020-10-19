#pragma once

#include <JuceHeader.h>
#include "Distortion.h"
#include "ProcessorChain.h"

class DelayProc
{
public:
    DelayProc() = default;

    void prepare (const dsp::ProcessSpec& spec);
    void reset();

    template<typename ProcessContext>
    void process (const ProcessContext& context);

    struct Parameters
    {
        float delayMs;
        float feedback;
        float lpfFreq;
        float hpfFreq;
        float distortion;
    };

    void setParameters (const Parameters& params);

private:
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Lagrange3rd> delay { 1 << 21 };

    float fs = 44100.0f;
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> feedback;
    std::vector<float> state;

    enum
    {
        lpfIdx,
        hpfIdx,
        distortionIdx,
    };

    MyProcessorChain<
        dsp::IIR::Filter<float>,
        dsp::IIR::Filter<float>,
        Distortion
        > procs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProc)
};
