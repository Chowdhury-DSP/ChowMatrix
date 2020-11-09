#pragma once

#include <pch.h>
#include "../../ChowMatrix.h"
#include "DelayNodeComponent.h"
#include "InputNodeComponent.h"
#include "NodeCompManager.h"
#include "MatrixAurora.h"

class GraphView : public Component,
                  public SettableTooltipClient,
                  public DBaseNode::Listener
{
public:
    GraphView (ChowMatrix& plugin);
    ~GraphView();

    enum ColourIDs
    {
        backgroundColour,
        nodeColour,
        nodeSelectedColour,
    };

    void mouseDown (const MouseEvent& e) override;
    void paint (Graphics& g) override;
    void resized() override;

    void setSelected (DelayNode* node);
    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;

private:
    ChowMatrix& plugin;
    NodeCompManager manager;
    MatrixAurora aurora;

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
            { "background",    GraphView::backgroundColour },
            { "node",          GraphView::nodeColour },
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
    std::unique_ptr<GraphView> graphView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphViewItem)
};
