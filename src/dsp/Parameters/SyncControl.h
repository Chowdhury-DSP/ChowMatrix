#pragma once

#include "BaseController.h"

/**
 * Utility class to manage sync/free delay modes
 */
class SyncControl : private BaseController
{
public:
    SyncControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    static void addParameters (Parameters& params);
    void newNodeAdded (DelayNode* newNode) override;

    void parameterChanged (const String&, float newValue) override;
    void setTempo (AudioPlayHead* playhead);

private:
    std::atomic<float>* syncParam = nullptr;
    std::atomic<double> tempo = 120.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SyncControl)
};
