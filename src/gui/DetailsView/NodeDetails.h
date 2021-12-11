#pragma once

#include "../../NodeManager.h"
#include "../NodeInfo.h"
#include "gui/IOSUtils/LongPressActionHelper.h"

namespace DetailsConsts
{
constexpr int buttonHeight = 45;
constexpr int circleRadius = 16;
} // namespace DetailsConsts

class NodeDetails : public Component
{
public:
    NodeDetails (DelayNode& node, NodeManager& manager);
    ~NodeDetails() override;

    void resized() override;
    DelayNode* getNode() { return nodeInfo.getNode(); }

    void setSelected() { manager.setSelected (getNode(), NodeManager::ActionSource::DetailsView); }
    void setSoloed() { manager.setSoloed (getNode(), NodeManager::ActionSource::DetailsView); }

    struct Button : Component, SettableTooltipClient
    {
        Button (NodeDetails& nodeDetails);
        
        enum ColourIDs
        {
            nodeColour,
            selectedColour
        };
        
        void mouseDown (const MouseEvent& e) override;
        void mouseDrag (const MouseEvent& e) override;
        void mouseUp (const MouseEvent& e) override;
        
        bool keyPressed (const KeyPress& key) override;
        void paint (Graphics& g) override;

    private:
        NodeDetails& nodeDetails;
        
#if JUCE_IOS
        LongPressActionHelper longPressAction;
        chowdsp::SharedLNFAllocator lnfAllocator;
#endif
    };

private:
    NodeManager& manager;
    NodeInfo nodeInfo;
    Button button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetails)
};
