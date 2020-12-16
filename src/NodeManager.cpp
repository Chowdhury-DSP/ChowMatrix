#include "NodeManager.h"

void NodeManager::doForNodes (DBaseNode* root, std::function<void(DelayNode*)> nodeFunc)
{
    for (int i = 0; i < root->getNumChildren(); ++i)
    {
        auto child = root->getChild (i);
        nodeFunc (child);
        doForNodes (child, nodeFunc);
    }
}

void NodeManager::doForNodes (std::array<InputNode, 2>* nodes, std::function<void(DelayNode*)> nodeFunc)
{
    for (auto& node : *nodes)
        NodeManager::doForNodes (&node, nodeFunc);
}

void NodeManager::initialise (std::array<InputNode, 2>* _nodes)
{
    nodes = _nodes;
    nodeCount = 0;

    for (auto& node : *nodes)
    {
        node.addNodeListener (this);
        doForNodes (&node, [=] (DelayNode* n) { nodeAdded (n); });
    }
}

void NodeManager::nodeAdded (DelayNode* newNode)
{
    newNode->addNodeListener (this);
    newNode->setIndex (nodeCount++);
    newNode->setSoloed (newNodeSoloState);
}

void NodeManager::nodeRemoved (DelayNode* nodeToRemove)
{
    nodeToRemove->removeNodeListener (this);
    if (nodeToRemove->getSelected())
        selectedNodePtr = nullptr;

    nodeCount = 0;
    doForNodes (nodes, [=] (DelayNode* n) { n->setIndex (nodeCount++); });
}

void NodeManager::setParameterDiff (DelayNode* sourceNode, const String& paramID, float diff01)
{
    doForNodes (nodes, [=] (DelayNode* n) {
        if (n == sourceNode)
            return;

        n->setNodeParameterDiff (paramID, diff01);
    });
}

void NodeManager::setSelected (DelayNode* selectedNode, SelectionSource source)
{
    selectedNodePtr = selectedNode;

    doForNodes (nodes, [=] (DelayNode* n) {
        if (n->getSelected() && selectedNode == n) // already selected!
            return;
        else if (n->getSelected()) // is currently selected, but no longer
            n->setSelected (false);
        else if (selectedNode == n) // should now be seleced
            n->setSelected (true);
    });

    listeners.call (&Listener::nodeSelected, selectedNode, source);
}

DelayNode* NodeManager::getSelected() const noexcept
{
    return selectedNodePtr;
}

void NodeManager::setSoloed (DelayNode* soloedNode)
{
    newNodeSoloState = soloedNode == nullptr ?
        DelayNode::SoloState::None : DelayNode::SoloState::Mute;

    doForNodes (nodes, [=] (DelayNode* n) {
        if (soloedNode == nullptr) // "un-solo" and currently soloed nodes
        {
            n->setSoloed (DelayNode::SoloState::None);
            return;
        }

        if (n == soloedNode) // Node that has been selected for soloing
        {
            if (n->getSoloed() == DelayNode::SoloState::Solo) // already soloed
                return;

            n->setSoloed (DelayNode::SoloState::Solo); // should now be soloed
            return;
        }

        // any other nodes should be muted
        n->setSoloed (DelayNode::SoloState::Mute);
    });
}
