#include "Distortion.h"

Distortion::Distortion()
{
    gain.setCurrentAndTargetValue (1.0f);
}

void Distortion::prepare (const dsp::ProcessSpec& spec)
{
    adaa.prepare();
    gain.reset (spec.sampleRate, 0.05);
}

void Distortion::reset()
{
    adaa.prepare();
    gain.setCurrentAndTargetValue (gain.getTargetValue());
}

void ADAA2::prepare()
{
    tables->prepare();

    x1 = 0.0;
    x2 = 0.0;
    ad2_x1 = 0.0;
    d2 = 0.0;
}
