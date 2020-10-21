#pragma once

#include "NodeComponent.h"
#include "../../dsp/DelayNode.h"
#include "../NodeInfo.h"

class DelayNodeComponent : public NodeComponent,
                           private AudioProcessorParameter::Listener
{
public:
    DelayNodeComponent (DelayNode& node, GraphView* view);
    ~DelayNodeComponent();

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void paint (Graphics& g) override;
    void updatePosition() override;

    void selectionChanged();
    void parameterValueChanged (int, float) override;
    void parameterGestureChanged (int, bool) override {}

private:
    void updateParams();
    float getMaxDist() const noexcept;

    DelayNode& node;
    NodeInfo nodeInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNodeComponent)
};
