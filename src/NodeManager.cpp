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
}

void NodeManager::nodeRemoved (DelayNode* nodeToRemove)
{
    nodeToRemove->removeNodeListener (this);

    nodeCount = 0;
    for (auto& node : *nodes)
        doForNodes (&node, [=] (DelayNode* n) { n->setIndex (nodeCount++); });
}

void NodeManager::setSelected (DelayNode* selectedNode)
{
    for (auto& node : *nodes)
    {
        doForNodes (&node, [=] (DelayNode* n) {
            if (n->getSelected() && selectedNode == n) // already selected!
                return;
            else if (n->getSelected()) // is currently selected, but no longer
                n->setSelected (false);
            else if (selectedNode == n) // should now be seleced
                n->setSelected (true);
        });
    }
}
