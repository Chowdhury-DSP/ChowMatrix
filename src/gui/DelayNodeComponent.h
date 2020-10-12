#pragma once

#include "NodeComponent.h"
#include "../dsp/DelayNode.h"
#include "NodeInfo.h"

class DelayNodeComponent : public NodeComponent
{
public:
    DelayNodeComponent (DelayNode& node, GraphView* view);

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void paint (Graphics& g) override;
    void updatePosition() override;

    void setSelected (bool shouldBeSelected);

private:
    void updateParams();
    float getMaxDist() const noexcept;

    DelayNode& node;
    bool isSelected = false;
    NodeInfo nodeInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNodeComponent)
};
