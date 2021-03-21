#include "VariableDelay.h"

VariableDelay::VariableDelay (size_t size) : l0Delay (static_cast<int> (size)),
                                             l1Delay (static_cast<int> (size)),
                                             l3Delay (static_cast<int> (size)),
                                             l5Delay (static_cast<int> (size)),
                                             sinc16Delay (static_cast<int> (size)),
                                             sinc32Delay (static_cast<int> (size))
{
}

void VariableDelay::setDelayForce (float newDelayInSamples) noexcept
{
    delaySmooth.setCurrentAndTargetValue (newDelayInSamples);

    for (auto* delay : delays)
        delay->setDelay (newDelayInSamples);
}

void VariableDelay::setDelayType (DelayType newType)
{
    auto oldType = type;

    // copy state and parameters first...
    delays[newType]->setDelay (delays[oldType]->getDelay());
    delays[newType]->copyState (*delays[oldType]);

    // then set new type
    type = newType;

    switch (type)
    {
    case BBDShort:
        makeupGain = 0.85f;
        delays[type]->reset();
        break;
    case BBDLong:
        makeupGain = 0.65f;
        delays[type]->reset();
        break;
    default:
        makeupGain = 1.0f;
    };
}

void VariableDelay::delayBlockStart() noexcept
{
    delays[type]->setDelay (delaySmooth.getCurrentValue());

    if (type == BBDShort)
        bbdShortDelay.setFilterFreq (2000.0f);
        
    if (type == BBDLong)
        bbdLongDelay.setFilterFreq (9000.0f);
}

void VariableDelay::prepare (const juce::dsp::ProcessSpec& spec)
{
    delaySmooth.reset (spec.sampleRate, 0.1);

    for (auto* delay : delays)
        delay->prepare (spec);
}

void VariableDelay::reset()
{
    for (auto* delay : delays)
        delay->reset();
}
