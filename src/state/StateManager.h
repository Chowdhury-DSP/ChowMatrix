#pragma once

#include "../dsp/InputNode.h"
#include "dsp/Parameters/HostParamControl.h"
#include "presets/PresetManager.h"

/** Class to manage the plugin state */
class StateManager
{
public:
    StateManager (AudioProcessorValueTreeState& vts,
                  HostParamControl& paramControl,
                  std::array<InputNode, 2>& nodes);

    /** Call this from the main plugin once the default states have been
     *  loaded, so that the initial A/B states will be the default state.
     */
    void loadDefaultABStates();

    /** Copies the current processor state to the non-active state */
    void copyABState();

    /** Returns 0 for A state, 1 for B state */
    int getCurrentABState() const noexcept { return static_cast<int> (currentState); }

    /** Loads a new state */
    void setCurrentABState (int newState);

    /** Save the current plugin state as an Xml object */
    std::unique_ptr<XmlElement> saveState();

    /** Load a new plugin state from an Xml object */
    void loadState (XmlElement* xml);

    /** Returns a SpinLock that is locked whenever the plugin state is being changed.
     *  It is not thread-safe to process audio while the state change is occuring.
     */
    SpinLock& getStateLoadLock() noexcept { return stateLoadingLock; }

    PresetManager& getPresetManager() { return presetManager; }

    bool getIsLoading() const noexcept { return isLoading.load(); }

private:
    AudioProcessorValueTreeState& vts;
    HostParamControl& paramControl;
    std::array<InputNode, 2>& inputNodes;
    SpinLock stateLoadingLock;
    std::atomic_bool isLoading { false };

    PresetManager presetManager;
    std::array<std::unique_ptr<XmlElement>, 2> abStates;
    bool currentState = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StateManager)
};
