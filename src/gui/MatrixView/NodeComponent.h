#pragma once

#include "../../dsp/DelayNode.h"
#include <pch.h>

class GraphView;
class NodeComponent : public Component
{
public:
    NodeComponent (DBaseNode& node, GraphView* view);
    virtual ~NodeComponent() = default;

    void paint (Graphics& g) override;

    DBaseNode& getNode() { return node; }

    // functions for managing node colour
    void setColour (const Colour& colour) { nodeColour = colour; }
    const Colour& getColour() const noexcept { return nodeColour; }
    void setHueIncrement (float newInc) { hueIncrement = newInc; }
    float getHueIncrement() const noexcept { return hueIncrement; }

    virtual void updatePosition() {}

    inline Point<int> getCentrePosition() const noexcept
    {
        return getPosition().translated (getWidth() / 2, getHeight() / 2);
    }

protected:
    GraphView* graphView;
    Colour nodeColour;

private:
    DBaseNode& node;
    float hueIncrement = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent)
};
