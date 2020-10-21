#pragma once

#include "../NodeInfo.h"

namespace DetailsConsts
{
    constexpr int buttonHeight = 60;
    constexpr int circleRadius = 16;
}

class NodeDetails : public Component
{
public:
    NodeDetails (DelayNode& node);

    void resized() override;
    const DelayNode* getNode() const noexcept { return nodeInfo.getNode(); }

    struct Button : Component
    {
        Button (NodeInfo& nodeInfo);
        enum ColourIDs { nodeColour, selectedColour };
        void paint (Graphics& g) override;

    private:
        NodeInfo& nodeInfo;
    };

private:
    NodeInfo nodeInfo;
    Button button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetails)
};
