#pragma once

#include <pch.h>
#include "NodeManager.h"
#include "dsp/InputNode.h"
#include "dsp/Parameters/InsanityControl.h"
#include "dsp/Parameters/DelayTypeControl.h"

/**
 * Main class for the Matrix plugin
 */ 
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

    /** Handle to access node manager */
    NodeManager& getManager() { return manager; }

    /** Handle to read insanity parameter (used by MatrixAurora) */
    std::atomic<float>* getInsanityParam() const noexcept { return insanityControl.getParameter(); }
    
    /** Access to array of input nodes */
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
