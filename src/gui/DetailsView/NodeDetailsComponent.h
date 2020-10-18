#pragma once

#include "../../ChowMatrix.h"
#include "../NodeInfo.h"

class NodeDetailsComponent : public Component,
                             private DBaseNode::Listener
{
public:
    NodeDetailsComponent (ChowMatrix& plugin);
    ~NodeDetailsComponent();

    void paint (Graphics& g) override;
    void resized() override;

    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;

    void setMinWidth (int newMinWidth);
    int calcWidth();

private:
    void addNode (DelayNode* node);

    ChowMatrix& plugin;
    OwnedArray<NodeInfo> nodes;
    int minWidth = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeDetailsComponent)
};
