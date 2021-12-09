#pragma once

#include "DelayNodeComponent.h"
#include "InputNodeComponent.h"
#include "NodeCompManager.h"
#include <pch.h>

class ChowMatrix;
class GraphView : public Component,
                  public SettableTooltipClient,
                  public DBaseNode::Listener
{
public:
    GraphView (ChowMatrix& plugin, Viewport& parent);
    ~GraphView() override;

    enum ColourIDs
    {
        backgroundColour,
        nodeColour,
        nodeColour2,
        nodeSelectedColour,
    };

    void mouseDown (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void paint (Graphics& g) override;
    void updateParentSize (int parentWidth, int parentHeight);
    int getVisibleHeight() const noexcept { return visibleHeight; }

    void setSelected (DelayNode* node, bool justCreated = false);
    void setSoloed (DelayNode* node);
    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;

    OwnedArray<DelayNodeComponent>& getDelayNodeComps() { return manager.delayNodeComponents; }

private:
    void createNodeForMouseEvent (const MouseEvent& e);

    ChowMatrix& plugin;
    NodeCompManager manager;
    Viewport& parent;

    int visibleHeight = 100;

    chowdsp::SharedLNFAllocator lnfAllocator;

#if JUCE_IOS
    std::atomic_bool doubleClickFlag { false };
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphView)
};
