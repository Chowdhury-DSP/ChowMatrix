#pragma once

#include <JuceHeader.h>
#include "../../dsp/DelayNode.h"

class GraphView;
class NodeComponent : public Component
{
public:
    NodeComponent (DBaseNode& node, GraphView* view);
    virtual ~NodeComponent() {}

    void paint (Graphics& g) override;

    DBaseNode& getNode() { return node; }

    virtual void updatePosition() {};
    
    inline Point<int> getCentrePosition() const noexcept
    {
        return getPosition().translated (getWidth() / 2, getHeight() / 2);
    }

protected:
    GraphView* graphView;

private:
    DBaseNode& node;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent)
};
