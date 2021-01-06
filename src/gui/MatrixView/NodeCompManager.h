#pragma once

#include "../../dsp/InputNode.h"
#include "DelayNodeComponent.h"
#include "InputNodeComponent.h"

class GraphView;
class NodeCompManager
{
public:
    NodeCompManager (GraphView* parent);

    void createAndAddEditor (InputNode*, const Colour& colour, float hueIncrement);
    void createAndAddEditor (DelayNode*);
    void removeEditor (DelayNode*);

    using NodeFunc = std::function<void(DBaseNode*, DelayNode*)>;
    void doForAllNodes (NodeFunc nodeFunc);

    friend class GraphView;

private:
    GraphView* parent;

    OwnedArray<InputNodeComponent> inputNodeComponents;
    OwnedArray<DelayNodeComponent> delayNodeComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeCompManager)
};
