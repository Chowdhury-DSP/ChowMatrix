#pragma once

#include "dsp/InputNode.h"
#include "dsp/DelayNode.h"

class NodeManager : private DBaseNode::Listener
{
public:
    NodeManager() = default;
    
    static void doForNodes (DBaseNode* root, std::function<void(DelayNode*)> nodeFunc);
    void initialise (std::array<InputNode, 2>* _nodes);

    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;
    void setParameter (DelayNode* sourceNode, const String& paramID, float value01) override;

    void setSelected (DelayNode* node);
    DelayNode* getSelected() const noexcept;

private:
    std::array<InputNode, 2>* nodes = nullptr;
    int nodeCount = 0;
    DelayNode* selectedNodePtr = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManager)
};
