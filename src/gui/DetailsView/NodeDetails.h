#pragma once

#include "../NodeInfo.h"
#include "../../NodeManager.h"

namespace DetailsConsts
{
    constexpr int buttonHeight = 60;
    constexpr int circleRadius = 16;
}

class NodeDetails : public Component
{
public:
    NodeDetails (DelayNode& node, NodeManager& manager);
    ~NodeDetails();

    void resized() override;
    DelayNode* getNode() { return nodeInfo.getNode(); }

    void setSelected() { manager.setSelected (getNode()); }

    struct Button : Component, SettableTooltipClient
    {
        Button (NodeDetails& nodeDetails);
        enum ColourIDs { nodeColour, selectedColour };
        void mouseDown (const MouseEvent& e) override;
        bool keyPressed (const KeyPress& key) override;
        void paint (Graphics& g) override;

    private:
        NodeDetails& nodeDetails;
    };

private:
    NodeManager& manager;
    NodeInfo nodeInfo;
    Button button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetails)
};