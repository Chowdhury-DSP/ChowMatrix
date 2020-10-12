#pragma once

#include <JuceHeader.h>
#include "dsp/InputNode.h"

class ChowMatrix : public chowdsp::PluginBase<ChowMatrix>
{
public:
    ChowMatrix();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    AudioProcessorEditor* createEditor() override;

    // @TODO: don't make this public
    InputNode inputNodes[2];

private:
    AudioBuffer<float> chBuffers[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowMatrix)
};
