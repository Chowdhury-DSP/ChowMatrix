#pragma once

#include "../Delay/VariableDelay.h"
#include "../InputNode.h"

class DelayTypeControl : private AudioProcessorValueTreeState::Listener,
                         private DBaseNode::Listener
{
public:
    using Parameters = std::vector<std::unique_ptr<juce::RangedAudioParameter>>;

    DelayTypeControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);
    ~DelayTypeControl();

    static void addParameters (Parameters& params);
    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;

    void parameterChanged (const String&, float newValue) override;
    std::atomic<float>* getParameter() const noexcept { return delayTypeParam; }

private:
    AudioProcessorValueTreeState& vts;
    std::array<InputNode, 2>* nodes;

    std::atomic<float>* delayTypeParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayTypeControl)
};
