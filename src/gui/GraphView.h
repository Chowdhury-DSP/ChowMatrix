#pragma once

#include <JuceHeader.h>
#include "../ChowMatrix.h"
#include "DelayNodeComponent.h"
#include "InputNodeComponent.h"
#include "NodeCompManager.h"

class GraphView : public Component
{
public:
    GraphView (ChowMatrix& plugin);

    enum ColourIDs
    {
        backgroundColour,
        nodeColour,
    };

    void paint (Graphics& g) override;
    void resized() override;

    void addNode (DelayNode* newNode);

private:
    NodeCompManager manager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphView)
};

class GraphViewItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (GraphViewItem)

    GraphViewItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) :
        foleys::GuiItem (builder, node)
    {
        auto* plugin = dynamic_cast<ChowMatrix*> (builder.getMagicState().getProcessor());
        jassert (plugin);
        graphView = std::make_unique<GraphView> (*plugin);

        setColourTranslation ({
            { "background", GraphView::backgroundColour },
            { "node",       GraphView::nodeColour },
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
    std::unique_ptr<GraphView> graphView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphViewItem)
};
