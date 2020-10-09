#pragma once

#include "BaseNode.h"

namespace DelayConsts
{
    constexpr float maxDelay = 200.0f;
}

class DelayNode : public BaseNode
{
public:
    DelayNode();

    float getDelayMs() const noexcept { return delayMs; }
    void setDelay (float newDelayMs) { delayMs = newDelayMs; }

    std::unique_ptr<Component> createEditor() override;

private:
    float delayMs = 50.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNode)
};
