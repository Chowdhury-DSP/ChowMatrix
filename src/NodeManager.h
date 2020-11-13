#pragma once

#include "dsp/InputNode.h"
#include "dsp/DelayNode.h"

/**
 * Utility class to manage node graph
 */ 
class NodeManager : private DBaseNode::Listener
{
public:
    NodeManager() = default;
    
    /** Iterate through node tree and perform nodeFunc for all nodes */
    static void doForNodes (DBaseNode* root, std::function<void(DelayNode*)> nodeFunc);
    static void doForNodes (std::array<InputNode, 2>* nodes, std::function<void(DelayNode*)> nodeFunc);
    
    /** Initialise node manager with an array of root nodes */
    void initialise (std::array<InputNode, 2>* _nodes);

    // Node listener overrides
    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;
    void setParameter (DelayNode* sourceNode, const String& paramID, float value01) override;

    // Manage selected node
    void setSelected (DelayNode* node);
    DelayNode* getSelected() const noexcept;

    // Manage soloed node
    void setSoloed (DelayNode* node);

private:
    std::array<InputNode, 2>* nodes = nullptr;
    int nodeCount = 0;
    DelayNode* selectedNodePtr = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManager)
};
