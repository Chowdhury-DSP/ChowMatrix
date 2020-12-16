#pragma once

#include "../../ChowMatrix.h"
#include "NodeDetails.h"

class NodeDetailsComponent : public Component,
                             private DBaseNode::Listener
{
public:
    NodeDetailsComponent (ChowMatrix& plugin);
    ~NodeDetailsComponent() override;

    void paint (Graphics& g) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;

    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;

    void setMinWidth (int newMinWidth);
    int calcWidth() const;

    NodeDetails* getNodeDetails (DelayNode* node);
    Point<int> getNodeDetailsPosition (DelayNode* node);

private:
    void addNode (DelayNode* node);

    ChowMatrix& plugin;
    OwnedArray<NodeDetails> nodes;
    int minWidth = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsComponent)
};
