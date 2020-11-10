#pragma once

#include "../Delay/VariableDelay.h"
#include "BaseController.h"

/**
 * Utility class to manage delay interpolation types
 */ 
class DelayTypeControl : private BaseController
{
public:
    DelayTypeControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    static void addParameters (Parameters& params);
    void newNodeAdded (DelayNode* newNode) override;

    void parameterChanged (const String&, float newValue) override;
    std::atomic<float>* getParameter() const noexcept { return delayTypeParam; }

private:
    std::atomic<float>* delayTypeParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayTypeControl)
};
