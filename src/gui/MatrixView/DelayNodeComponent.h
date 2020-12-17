#pragma once

#include "NodeComponent.h"
#include "../../dsp/DelayNode.h"
#include "../NodeInfo.h"

class DelayNodeComponent : public NodeComponent,
                           public SettableTooltipClient,
                           private Timer,
                           private AudioProcessorParameter::Listener
{
public:
    DelayNodeComponent (DelayNode& node, GraphView* view);
    ~DelayNodeComponent();

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    bool keyPressed (const KeyPress& key) override;
    void paint (Graphics& g) override;
    void updatePosition() override;
    NodeInfo& getNodeInfo() { return nodeInfo; }

    void selectionChanged();
    void parameterValueChanged (int, float) override;
    void parameterGestureChanged (int, bool) override {}
    void timerCallback() override;

private:
    void updateParams();
    float getMaxDist() const noexcept;
    void updateTimerFreq (float modFrequency);

    DelayNode& node;
    NodeInfo nodeInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNodeComponent)
};
