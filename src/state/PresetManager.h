#pragma once

#include "StateManager.h"

class PresetManager : public chowdsp::PresetManager
{
public:
    PresetManager (AudioProcessorValueTreeState& vts, StateManager& stateMgr);

    std::unique_ptr<XmlElement> savePresetState() override;
    void loadPresetState (const XmlElement* xml) override;

    chowdsp::Preset loadUserPresetFromFile (const File& file) override;

private:
    StateManager& stateManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
