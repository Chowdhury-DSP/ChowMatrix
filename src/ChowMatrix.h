#pragma once

#include <JuceHeader.h>

class ChowMatrix : public chowdsp::PluginBase<ChowMatrix>
{
public:
    ChowMatrix() {}

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowMatrix)
};
