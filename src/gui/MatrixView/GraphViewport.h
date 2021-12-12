#pragma once

#include "GraphView.h"
#include "MatrixAurora.h"
#include "NodeManager.h"

#if JUCE_IOS
#include "../IOSUtils/TwoFingerDragToScrollListener.h"
#endif

class GraphViewport : public Viewport,
                      private NodeManager::Listener,
                      private Timer
{
public:
    explicit GraphViewport (ChowMatrix& plugin);
    ~GraphViewport() override;

    void resized() override;
    void mouseDrag (const MouseEvent& e) override;

    void nodeSelected (DelayNode* selectedNode, NodeManager::ActionSource source) override;
    void nodeSoloed (DelayNode* soloedNode, NodeManager::ActionSource source) override;
    void nodeInsanityLockChanged (DelayNode* node) override;

    MatrixAurora& getAurora() { return aurora; }

private:
    void setupHomeButton();
    void centerView();
    void timerCallback() override;

    GraphView graphView;
    MatrixAurora aurora;
    NodeManager& manager;

    DrawableButton homeButton;

#if JUCE_IOS
    bool firstTouch = false;
    std::unique_ptr<TwoFingerDragToScrollListener> dragToScrollListener;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphViewport)
};
