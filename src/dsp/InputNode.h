#pragma once

#include "DelayNode.h"

/**
 * Class for audio Input Node
 */ 
class InputNode : public DBaseNode
{
public:
    InputNode() = default;

    std::unique_ptr<NodeComponent> createEditor (GraphView*) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputNode)
};
