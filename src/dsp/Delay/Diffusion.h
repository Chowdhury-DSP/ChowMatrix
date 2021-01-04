#pragma once

/**
 * Class for delay line diffusion processing
 * 
 * The DSP is made up of many stages of first-order allpass
 * filters, with linear fading between stages.
 */ 
class Diffusion
{
public:
    Diffusion();

    void prepare (const dsp::ProcessSpec& spec);
    void reset();

    inline float processSample (float x)
    {
        const auto numStages = depthSmooth.getNextValue() * 50.0f;
        const auto numStagesInt = static_cast<size_t> (numStages);

        // process integer stages
        for (size_t stage = 0; stage < numStagesInt; ++stage)
            x = processStage (x, stage);

        // process fractional stage
        float stageFrac = numStages - numStagesInt;
        x = stageFrac * processStage (x, numStagesInt) + (1.0f - stageFrac) * x;

        return x;
    }

    inline float processStage (float x, size_t stage)
    {
        float y = z[stage] + x * b[0];
        z[stage] = x * b[1] - y * a[1];

        return y;
    }

private:
    void calcCoefs (float fc);

    static constexpr size_t maxNumStages = 52;
    static constexpr float fc = 1000.0f;

    float a[2] = {1.0f, 0.0f};
    float b[2] = {1.0f, 0.0f};
    float z[maxNumStages];

    float fs = 44100.0f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> depthSmooth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Diffusion)
};
