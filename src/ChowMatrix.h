#pragma once

#include <JuceHeader.h>
#include "NodeManager.h"
#include "dsp/InputNode.h"
#include "dsp/InsanityControl.h"

class ChowMatrix : public chowdsp::PluginBase<ChowMatrix>
{
public:
    ChowMatrix();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    AudioProcessorEditor* createEditor() override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // @TODO: don't make this public
    std::array<InputNode, 2> inputNodes;
    // InputNode inputNodes[2];

    NodeManager& getManager() { return manager; }
    std::atomic<float>* getInsanityParam() const noexcept { return insanityControl.getParameter(); }

private:
    NodeManager manager;

    std::atomic<float>* dryParamDB = nullptr;
    std::atomic<float>* wetParamDB = nullptr;

    AudioBuffer<float> chBuffers[2];

    AudioBuffer<float> dryBuffer;
    dsp::Gain<float> dryGain;
    dsp::Gain<float> wetGain;

    InsanityControl insanityControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowMatrix)
};
