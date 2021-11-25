#pragma once

#include "BaseController.h"
#include "ParamHelpers.h"

class NodeParamControl : public BaseController
{
public:
    NodeParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    void newNodeAdded (DelayNode* newNode) override;
    void newNodeRemoved (DelayNode* newNode) override;
    void nodeIndexHasChanged (DelayNode* node, int oldIndex, int newIndex) override;

    void parameterChanged (const String& /*parameterID*/, float /*newValue*/) override {}

private:
    Array<chowdsp::ForwardingParameter*> forwardedParams;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeParamControl)
};
