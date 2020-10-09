#pragma once

#include "../dsp/BaseNode.h"

class NodeComponent : public Component
{
public:
    NodeComponent (BaseNode& node);
    virtual ~NodeComponent() {}

    void paint (Graphics& g) override;

    void mouseDoubleClick (const MouseEvent& e) override;

    BaseNode& getNode() { return node; }

private:
    BaseNode& node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent)
};
