#pragma once

#include "Preset.h"

class StateManager;
class PresetComp;
class PresetManager
{
public:
    PresetManager (StateManager* stateManager, AudioProcessorValueTreeState& vts);

    StringArray getPresetChoices();
    void loadPresets();
    static void addParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params);

    int getNumPresets() const noexcept { return presets.size(); }
    int getNumFactoryPresets() const noexcept { return numFactoryPresets; }
    String getPresetName (int idx);
    bool setPreset (int idx);
    int getSelectedPresetIdx() const noexcept { return presetParam->get(); }

    void presetUpdated() { listeners.call (&Listener::presetUpdated); }

    File getUserPresetFolder() { return userPresetFolder; }
    void chooseUserPresetFolder();
    bool saveUserPreset (const String& name, int& newPresetIdx);
    const PopupMenu& getUserPresetMenu (const PresetComp* comp) const;

    struct Listener
    {
        virtual ~Listener() {}
        virtual void presetUpdated() {}
    };

    void addListener (Listener* l) { listeners.add (l); }
    void removeListener (Listener* l) { listeners.remove (l); }

private:
    File getUserPresetConfigFile() const;
    void updateUserPresets();
    void loadPresetFolder (PopupMenu& menu, File& directory);

    File userPresetFolder;
    int numFactoryPresets = 0;
    PopupMenu userPresetMenu;

    StateManager* stateManager;
    AudioParameterInt* presetParam = nullptr;

    std::unordered_map<int, Preset*> presetMap;
    OwnedArray<Preset> presets;
    int maxIdx;

    std::future<void> loadingFuture;
    ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
