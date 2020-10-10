#pragma once

#include "NodeComponent.h"
#include "../dsp/DelayNode.h"

class DelayNodeComponent : public NodeComponent
{
public:
    DelayNodeComponent (DelayNode& node, GraphView* view);

    void mouseDrag (const MouseEvent& e) override;
    void updatePosition() override;

private:
    void updateParams();

    DelayNode& node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNodeComponent)
};
