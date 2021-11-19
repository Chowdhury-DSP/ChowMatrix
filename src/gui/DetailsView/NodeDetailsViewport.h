#pragma once

#include "NodeDetailsComponent.h"

#if JUCE_IOS
#include "../IOSUtils/TwoFingerDragToScrollListener.h"
#endif

class NodeDetailsViewport : public Viewport,
                            private NodeManager::Listener
{
public:
    NodeDetailsViewport (ChowMatrix& chowMatrix);
    virtual ~NodeDetailsViewport();

    void resized() override;
    void paint (Graphics&) override;
    void visibleAreaChanged (const Rectangle<int>&) override;

    void nodeSelected (DelayNode* selectedNode, NodeManager::ActionSource source) override;
    void nodeSoloed (DelayNode* soloedNode, NodeManager::ActionSource source) override;
    void nodeInsanityLockChanged (DelayNode* node) override;

private:
    NodeManager& manager;
    NodeDetailsComponent detailsComp;

#if JUCE_IOS
    std::unique_ptr<TwoFingerDragToScrollListener> dragToScrollListener;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsViewport)
};
