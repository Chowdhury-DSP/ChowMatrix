#pragma once

#include "BaseNode.h"

namespace DelayConsts
{
    constexpr float maxDelay = 200.0f;
}

class DelayNode : public BaseNode<DelayNode>
{
public:
    DelayNode();

    float getDelayMs() const noexcept { return delayMs->get(); }
    void setDelay (float newDelayMs) { *delayMs = newDelayMs; }

    float getPan() const noexcept { return pan->get(); }
    void setPan (float newPan) { *pan = newPan; }

    std::unique_ptr<NodeComponent> createEditor (GraphView*) override;

    int getNumParams() const noexcept { return params.size(); }
    AudioParameterFloat* getParam (int idx) { return params[idx]; }

private:
    OwnedArray<AudioParameterFloat> params;

    AudioParameterFloat* delayMs = nullptr;
    AudioParameterFloat* pan = nullptr;

    // float delayMs = 50.0f;
    // float pan = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNode)
};

using DBaseNode = BaseNode<DelayNode>;
