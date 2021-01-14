#include "NodeCompManager.h"
#include "GraphView.h"

template <typename Node, typename Editor>
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
NodeCompManager::NodeCompManager (GraphView* parent) : parent (parent)
{
}

void NodeCompManager::createAndAddEditor (InputNode* node, const Colour& colour, float hueIncrement)
{
    auto editor = createAndAddEditorToList (node, inputNodeComponents, parent);
    parent->addAndMakeVisible (editor);
    node->addNodeListener (parent);

    editor->setColour (colour);
    editor->setHueIncrement (hueIncrement);
}

void NodeCompManager::createAndAddEditor (DelayNode* node)
{
    auto editor = createAndAddEditorToList (node, delayNodeComponents, parent);
    parent->addAndMakeVisible (editor);
    node->addNodeListener (parent);

    const auto* parentEditor = node->getParent()->getEditor();
    const auto& parentColour = parentEditor->getColour();
    editor->setColour (parentColour.withRotatedHue (parentEditor->getHueIncrement()));
    editor->setHueIncrement (parentEditor->getHueIncrement());
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
