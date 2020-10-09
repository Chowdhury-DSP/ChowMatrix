#pragma once

#include "NodeComponent.h"
#include "../dsp/DelayNode.h"

class DelayNodeComponent : public NodeComponent
{
public:
    DelayNodeComponent (DelayNode& node);

    void mouseDrag (const MouseEvent& e) override;
    Point<float> getDistance();

private:
    void updatePosition();

    DelayNode& node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNodeComponent)
};
