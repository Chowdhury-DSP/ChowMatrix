#pragma once

#include "ChowMatrix.h"
#include "GraphViewport.h"

class GraphViewItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (GraphViewItem)

    GraphViewItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) : foleys::GuiItem (builder, node)
    {
        auto* plugin = dynamic_cast<ChowMatrix*> (builder.getMagicState().getProcessor());
        jassert (plugin);
        graphView = std::make_unique<GraphViewport> (*plugin);

        setColourTranslation ({
            { "background", GraphView::backgroundColour },
            { "node", GraphView::nodeColour },
            { "node2", GraphView::nodeColour2 },
            { "node-selected", GraphView::nodeSelectedColour },
        });

        addAndMakeVisible (graphView.get());
    }

    void update() override
    {
        graphView->repaint();
    }

    Component* getWrappedComponent() override
    {
        return graphView.get();
    }

private:
    std::unique_ptr<GraphViewport> graphView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphViewItem)
};
