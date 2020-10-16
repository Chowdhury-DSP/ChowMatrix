#include "NodeDetailsComponent.h"

namespace
{
    constexpr int xOffset = 5;
    constexpr int xPad = 10;
}

void doForNodes (DBaseNode* root, std::function<void(DelayNode*)> nodeFunc)
{
    for (int i = 0; i < root->getNumChildren(); ++i)
    {
        auto child = root->getChild (i);
        nodeFunc (child);
        doForNodes (child, nodeFunc);
    }
}

NodeDetailsComponent::NodeDetailsComponent (ChowMatrix& plugin) :
    plugin (plugin)
{
    setSize (1500, 200);

    for (auto& node : plugin.inputNodes)
    {
        node.addListener (this);
        doForNodes (&node, [=] (DelayNode* node) { addNode (node); });
    }
}

NodeDetailsComponent::~NodeDetailsComponent()
{
    for (auto& node : plugin.inputNodes)
    {
        doForNodes (&node, [=] (DelayNode* node) { node->removeListener (this); });
        node.removeListener (this);
    }
}

void NodeDetailsComponent::addNode (DelayNode* node)
{
    addAndMakeVisible (nodes.add (std::make_unique<NodeInfo> (*node)));
    node->addListener (this);
}

void NodeDetailsComponent::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void NodeDetailsComponent::resized()
{
    for (int i = 0; i < nodes.size(); ++i)
    {
        nodes[i]->setBounds (xOffset + (NodeInfoConsts::InfoWidth + xPad) * i, 0,
            NodeInfoConsts::InfoWidth, getHeight());
    }
}

void NodeDetailsComponent::nodeAdded (DelayNode* newNode)
{
    addAndMakeVisible (nodes.add (std::make_unique<NodeInfo> (*newNode)));
    newNode->addListener (this);

    MessageManagerLock mml;
    setSize (calcWidth(), getHeight());
    resized();
    repaint();
}

void NodeDetailsComponent::setMinWidth (int newMinWidth)
{
    minWidth = newMinWidth;
    setSize (calcWidth(), getHeight());
}

int NodeDetailsComponent::calcWidth()
{
    int width = 2 * xOffset + (xPad + NodeInfoConsts::InfoWidth) * nodes.size();
    return jmax (width, minWidth);
}
