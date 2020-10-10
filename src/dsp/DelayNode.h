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

    float getDelayMs() const noexcept { return delayMs; }
    void setDelay (float newDelayMs) { delayMs = newDelayMs; }

    float getPan() const noexcept { return pan; }
    void setPan (float newPan) { pan = newPan; }

    std::unique_ptr<NodeComponent> createEditor (GraphView*) override;

private:
    float delayMs = 50.0f;
    float pan = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNode)
};

using DBaseNode = BaseNode<DelayNode>;
