#pragma once

#include "dsp/DelayNode.h"
#include "dsp/InputNode.h"

/**
 * Utility class to manage node graph
 */
class NodeManager : private DBaseNode::Listener
{
public:
    NodeManager() = default;

    /** Iterate through node tree and perform nodeFunc for all nodes */
    static void doForNodes (DBaseNode* root, std::function<void (DelayNode*)> nodeFunc);
    static void doForNodes (std::array<InputNode, 2>* nodes, std::function<void (DelayNode*)> nodeFunc);

    /** Initialise node manager with an array of root nodes */
    void initialise (std::array<InputNode, 2>* _nodes);

    // Node listener overrides
    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;
    void setParameterDiff (DelayNode* sourceNode, const String& paramID, float diff01) override;
    void nodeParamLockChanged (DelayNode* node) override { listeners.call (&Listener::nodeParamLockChanged, node); }

    /** Sources that can trigger node selection */
    enum class ActionSource
    {
        GraphView,
        DetailsView
    };

    // Manage selected node
    void setSelected (DelayNode* node, ActionSource source);
    DelayNode* getSelected() const noexcept;

    // Manage soloed node
    void setSoloed (DelayNode* node, ActionSource source);

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void nodeSelected (DelayNode* /*selectedNode*/, ActionSource /*source*/) {}
        virtual void nodeSoloed (DelayNode* /*soloedNode*/, ActionSource /*source*/) {}
        virtual void nodeParamLockChanged (DelayNode* /*node*/) {}
    };

    void addListener (Listener* l) { listeners.add (l); }
    void removeListener (Listener* l) { listeners.remove (l); }

private:
    std::array<InputNode, 2>* nodes = nullptr;
    int nodeCount = 0;
    DelayNode* selectedNodePtr = nullptr;
    DelayNode::SoloState newNodeSoloState = DelayNode::SoloState::None;

    ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManager)
};
