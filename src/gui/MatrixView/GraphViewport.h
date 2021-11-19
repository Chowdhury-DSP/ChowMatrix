#pragma once

#include "GraphView.h"
#include "MatrixAurora.h"

#if JUCE_IOS
#include "../IOSUtils/TwoFingerDragToScrollListener.h"
#endif

class GraphViewport : public Viewport,
                      private NodeManager::Listener
{
public:
    GraphViewport (ChowMatrix& plugin);
    virtual ~GraphViewport();

    void resized() override;
    void mouseDrag (const MouseEvent& e) override;

    void nodeSelected (DelayNode* selectedNode, NodeManager::ActionSource source) override;
    void nodeSoloed (DelayNode* soloedNode, NodeManager::ActionSource source) override;
    void nodeInsanityLockChanged (DelayNode* node) override;

    MatrixAurora& getAurora() { return aurora; }

private:
    void setupHomeButton();
    void centerView();

    GraphView graphView;
    MatrixAurora aurora;
    NodeManager& manager;

    DrawableButton homeButton;

#if JUCE_IOS
    std::unique_ptr<TwoFingerDragToScrollListener> dragToScrollListener;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphViewport)
};
