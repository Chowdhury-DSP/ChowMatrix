#pragma once

#include <JuceHeader.h>
#include "NodeManager.h"
#include "dsp/InputNode.h"
#include "dsp/Parameters/InsanityControl.h"
#include "dsp/Parameters/DelayTypeControl.h"
#include "dsp/Distortion/LookupTables.h"

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

    NodeManager& getManager() { return manager; }
    std::atomic<float>* getInsanityParam() const noexcept { return insanityControl.getParameter(); }
    std::array<InputNode, 2>* getNodes() { return &inputNodes; }

private:
    std::array<InputNode, 2> inputNodes;
    NodeManager manager;

    std::atomic<float>* dryParamDB = nullptr;
    std::atomic<float>* wetParamDB = nullptr;

    AudioBuffer<float> chBuffers[2];

    AudioBuffer<float> dryBuffer;
    dsp::Gain<float> dryGain;
    dsp::Gain<float> wetGain;

    InsanityControl insanityControl;
    DelayTypeControl delayTypeControl;

    // create this here so loading new nodes is always fast
    SharedResourcePointer<LookupTables> luts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowMatrix)
};
