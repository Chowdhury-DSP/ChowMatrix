#pragma once

#include "BaseController.h"
#include "ParamHelpers.h"

class NodeParamControl : public BaseController,
                         private AudioProcessorParameter::Listener
{
public:
    NodeParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes, std::unique_ptr<chowdsp::PresetManager>& presetMgr);

    void newNodeAdded (DelayNode* newNode) override;
    void newNodeRemoved (DelayNode* newNode) override;
    void nodeIndexHasChanged (DelayNode* node, int oldIndex, int newIndex) override;

    void parameterChanged (const String& /*parameterID*/, float /*newValue*/) override {}

    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int, bool) override {}

private:
    void setCurrentPresetDirty();

    Array<chowdsp::ForwardingParameter*> forwardedParams;

    std::unique_ptr<chowdsp::PresetManager>& presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeParamControl)
};
