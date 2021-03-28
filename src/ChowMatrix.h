#pragma once

#include "NodeManager.h"
#include "dsp/InputNode.h"
#include "dsp/Parameters/DelayTypeControl.h"
#include "dsp/Parameters/InsanityControl.h"
#include "dsp/Parameters/SyncControl.h"
#include "gui/AutoUpdating.h"
#include "state/StateManager.h"
#include <pch.h>

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
    void processAudioBlock (AudioBuffer<float>& buffer) override;

    AudioProcessorEditor* createEditor() override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void updateHostPrograms();

    /** Handle to access node manager */
    NodeManager& getManager() { return manager; }

    /** Handle to read insanity parameter (used by MatrixAurora) */
    std::atomic<float>* getInsanityParam() const noexcept { return insanityControl.getParameter(); }

    /** Access to array of input nodes */
    std::array<InputNode, 2>* getNodes() { return &inputNodes; }
    StateManager& getStateManager() { return stateManager; }

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
    SyncControl syncControl;

    // create this here so loading new nodes is always fast
    SharedResourcePointer<LookupTables> luts;

    AutoUpdater updater;
    StateManager stateManager;
    std::atomic_bool prepared = false; // so we don't try to change presets before the processor is prepared

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowMatrix)
};
