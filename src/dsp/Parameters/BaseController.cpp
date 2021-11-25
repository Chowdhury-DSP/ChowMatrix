#include "BaseController.h"
#include "../../NodeManager.h"

BaseController::BaseController (AudioProcessorValueTreeState& vts,
                                std::array<InputNode, 2>* nodes,
                                StringArray paramsToListenFor) : vts (vts),
                                                                 nodes (nodes),
                                                                 paramsToListenFor (paramsToListenFor)
{
    for (auto& node : *nodes)
    {
        node.addNodeListener (this);
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { n->addNodeListener (this); });
    }

    for (const auto& param : paramsToListenFor)
        vts.addParameterListener (param, this);
}

BaseController::~BaseController()
{
    NodeManager::doForNodes (nodes, [=] (DelayNode* n) { n->removeNodeListener (this); });

    for (const auto& param : paramsToListenFor)
        vts.removeParameterListener (param, this);
}

void BaseController::nodeAdded (DelayNode* newNode)
{
    newNode->addNodeListener (this);
    newNodeAdded (newNode);
}

void BaseController::nodeRemoved (DelayNode* nodeToRemove)
{
    newNodeRemoved (nodeToRemove);
    nodeToRemove->removeNodeListener (this);
}

void BaseController::nodeIndexChanged (DelayNode* node, int oldIndex, int newIndex)
{
    nodeIndexHasChanged (node, oldIndex, newIndex);
}

void BaseController::doForNodes (std::function<void (DelayNode*)> nodeFunc)
{
    NodeManager::doForNodes (nodes, nodeFunc);
}
