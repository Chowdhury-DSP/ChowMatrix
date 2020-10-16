#pragma once

#include "NodeDetailsViewport.h"

class NodeDetailsGUI : public Component
{
public:
    NodeDetailsGUI (ChowMatrix& chowMatrix) :
        nodeDetailsViewport (chowMatrix)
    {
        addAndMakeVisible (nodeDetailsViewport);
    }

    void resized() override
    {
        nodeDetailsViewport.setBounds (getLocalBounds());
    }

private:
    NodeDetailsViewport nodeDetailsViewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsGUI)
};

class NodeDetailsItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (NodeDetailsItem)

    NodeDetailsItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) :
        foleys::GuiItem (builder, node)
    {
        auto* plugin = dynamic_cast<ChowMatrix*> (builder.getMagicState().getProcessor());
        jassert (plugin);
        nodeDetails = std::make_unique<NodeDetailsViewport> (*plugin);

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
    std::unique_ptr<NodeDetailsViewport> nodeDetails;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsItem)
};
