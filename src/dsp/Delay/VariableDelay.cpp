#include "VariableDelay.h"

VariableDelay::VariableDelay (size_t size) :
    l0Delay (size),
    l1Delay (size),
    l3Delay (size),
    l5Delay (size)
{}

void VariableDelay::setDelay (float newDelayInSamples)
{ 
    delays[type]->setDelay (newDelayInSamples);
}

float VariableDelay::getDelay() const
{ 
    return delays[type]->getDelay();
}

void VariableDelay::setDelayType (DelayType newType)
{
    auto oldType = type;

    delays[newType]->setDelay (delays[oldType]->getDelay());
    delays[newType]->copyState (*delays[oldType]);

    type = newType;
}

void VariableDelay::prepare (const juce::dsp::ProcessSpec& spec)
{
    for (auto* delay : delays)
        delay->prepare (spec);
}

void VariableDelay::reset()
{
    for (auto* delay : delays)
        delay->reset();
}
