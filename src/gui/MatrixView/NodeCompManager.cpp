#include "NodeCompManager.h"
#include "GraphView.h"

template<typename Node, typename Editor>
Editor* createAndAddEditorToList (Node* node, OwnedArray<Editor>& list, GraphView* view)
{ 
    return list.add (dynamic_cast<Editor*> (node->createNodeEditor (view).release()));
}

void doForAllChildNodes (DBaseNode* root, NodeCompManager::NodeFunc funcToDo)
{
    if (root == nullptr)
        return;

    for (int i = 0; i < root->getNumChildren(); ++i)
    {
        auto* childNode = root->getChild (i);
        funcToDo (root, childNode);
        doForAllChildNodes (childNode, funcToDo);
    }
}

///////////////////////////////////////////////////////
NodeCompManager::NodeCompManager (GraphView* parent) :
    parent (parent)
{}

void NodeCompManager::createAndAddEditor (InputNode* node)
{
    parent->addAndMakeVisible (createAndAddEditorToList (node, inputNodeComponents, parent));
    node->addNodeListener (parent);
}

void NodeCompManager::createAndAddEditor (DelayNode* node)
{
    parent->addAndMakeVisible (createAndAddEditorToList (node, delayNodeComponents, parent));
    node->addNodeListener (parent);
}

void NodeCompManager::removeEditor (DelayNode* nodeToRemove)
{
    delayNodeComponents.removeObject (dynamic_cast<DelayNodeComponent*> (nodeToRemove->getEditor()));
    nodeToRemove->removeNodeListener (parent);
}

void NodeCompManager::doForAllNodes (NodeFunc nodeFunc)
{
    for (auto* nodeComp : inputNodeComponents)
        doForAllChildNodes (&nodeComp->getNode(), nodeFunc);
}
