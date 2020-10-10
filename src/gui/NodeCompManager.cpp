#include "NodeCompManager.h"
#include "GraphView.h"

template<typename Node, typename Editor>
Editor* createAndAddEditorToList (Node* node, OwnedArray<Editor>& list, GraphView* view)
{ 
    return list.add (dynamic_cast<Editor*> (node->createEditor (view).release()));
}

void doForAllChildNodes (NodeComponent* root, NodeCompManager::NodeFunc funcToDo)
{
    if (root == nullptr)
        return;
    
    auto& rootNode = root->getNode();
    for (int i = 0; i < rootNode.getNumChildren(); ++i)
    {
        auto* childNode = rootNode.getChild (i);
        funcToDo (root, childNode);
        doForAllChildNodes (childNode->getEditor(), funcToDo);
    }
}

///////////////////////////////////////////////////////
NodeCompManager::NodeCompManager (GraphView* parent) :
    parent (parent)
{}

void NodeCompManager::createAndAddEditor (InputNode* node)
{
    parent->addAndMakeVisible (createAndAddEditorToList (node, inputNodeComponents, parent));
}

void NodeCompManager::createAndAddEditor (DelayNode* node)
{
    parent->addAndMakeVisible (createAndAddEditorToList (node, delayNodeComponents, parent));
}

void NodeCompManager::doForAllNodes (NodeFunc nodeFunc)
{
    for (auto* nodeComp : inputNodeComponents)
        doForAllChildNodes (nodeComp, nodeFunc);
}
