#include "Diffusion.h"

namespace
{
    constexpr double smoothTime = 0.01;
    constexpr float allpassFreq = 200.0f;
}

Diffusion::Diffusion()
{
    depthSmooth.setCurrentAndTargetValue (0.0f);
    reset();
}

void Diffusion::setDepth (float depth, bool force)
{
    if (force)
        depthSmooth.setCurrentAndTargetValue (depth);
    else
        depthSmooth.setTargetValue (depth);
}

void Diffusion::prepare (const dsp::ProcessSpec& spec)
{
    fs = (float) spec.sampleRate;
    depthSmooth.reset (spec.sampleRate, smoothTime);

    calcCoefs (allpassFreq);
    reset();
}

void Diffusion::reset()
{
    std::fill (z, &z[maxNumStages+1], 0.0f);
}

void Diffusion::calcCoefs (float freq)
{
    const float RC = 1.0f / (MathConstants<float>::twoPi * freq);
    const float b0s = RC;
    const float b1s = -1.0f;
    const float a0s = b0s;
    const float a1s = 1.0f;
    const auto K = 2.0f * fs;

    const auto a0 = a0s * K + a1s;
    b[0] = ( b0s * K + b1s) / a0;
    b[1] = (-b0s * K + b1s) / a0;
    a[0] = 1.0f;
    a[1] = (-a0s * K + a1s) / a0;
}
