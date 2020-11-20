#pragma once

#include "Preset.h"

class ChowMatrix;
class PresetManager
{
public:
    PresetManager (ChowMatrix* plugin, AudioProcessorValueTreeState& vts);

    StringArray getPresetChoices();
    void loadPresets();
    static void addParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& params);

    int getNumPresets() const { return presets.size(); }
    String getPresetName (int idx);
    bool setPreset (int idx);
    int getSelectedPresetIdx() const noexcept { return presetParam->get(); }

    void presetUpdated() { listeners.call (&Listener::presetUpdated); }

    File getUserPresetFolder() { return userPresetFolder; }
    void chooseUserPresetFolder();
    bool saveUserPreset (const String& name);
    const PopupMenu& getUserPresetMenu() const { return userPresetMenu; }

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

    ChowMatrix* plugin;
    AudioProcessorValueTreeState& vts;
    AudioParameterInt* presetParam = nullptr;

    std::unordered_map<int, Preset*> presetMap;
    OwnedArray<Preset> presets;
    int maxIdx;

    std::future<void> loadingFuture;
    ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
