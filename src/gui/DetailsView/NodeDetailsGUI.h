#pragma once

#include "NodeDetailsViewport.h"

class NodeDetailsGUI : public Component
{
public:
    NodeDetailsGUI (ChowMatrix& chowMatrix);

    void resized() override;

    enum ColourIDs
    {
        nodeColour = 0x110778,
        nodeSelectedColour,
        scrollThumbColour,
        scrollTrackColour,
    };

private:
    OwnedArray<Label> labels;
    NodeDetailsViewport nodeDetailsViewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsGUI)
};

class NodeDetailsItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (NodeDetailsItem)

    NodeDetailsItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) : foleys::GuiItem (builder, node)
    {
        setColourTranslation ({
            { "node", NodeDetailsGUI::nodeColour },
            { "node-selected", NodeDetailsGUI::nodeSelectedColour },
            { "scroll", NodeDetailsGUI::scrollThumbColour },
            { "scroll-track", NodeDetailsGUI::scrollTrackColour },
        });

        auto* plugin = dynamic_cast<ChowMatrix*> (builder.getMagicState().getProcessor());
        jassert (plugin);
        nodeDetails = std::make_unique<NodeDetailsGUI> (*plugin);

        addAndMakeVisible (nodeDetails.get());
    }

    void update() override
    {
        nodeDetails->repaint();
    }

    Component* getWrappedComponent() override
    {
        return nodeDetails.get();
    }

private:
    std::unique_ptr<NodeDetailsGUI> nodeDetails;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsItem)
};
