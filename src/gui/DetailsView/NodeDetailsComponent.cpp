#include "NodeDetailsComponent.h"
#include "../../NodeManager.h"
#include "NodeDetailsGUI.h"

namespace
{
constexpr int xPad = 3;
constexpr int xOffset = xPad;
constexpr int scrollOffset = 4;
} // namespace

constexpr int calcHeight()
{
    return DetailsConsts::buttonHeight + NodeInfoConsts::InfoHeightNoLabel * ParamHelpers::numParams + scrollOffset;
}

NodeDetailsComponent::NodeDetailsComponent (ChowMatrix& plugin) : plugin (plugin)
{
    for (auto& node : *plugin.getNodes())
    {
        node.addNodeListener (this);
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { addNode (n); });
    }

    setSize (calcWidth(), calcHeight());
}

NodeDetailsComponent::~NodeDetailsComponent()
{
    for (auto& node : *plugin.getNodes())
    {
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { n->removeNodeListener (this); });
        node.removeNodeListener (this);
    }
}

void NodeDetailsComponent::addNode (DelayNode* node)
{
    addAndMakeVisible (nodes.add (std::make_unique<NodeDetails> (*node, plugin.getManager())));
    node->addNodeListener (this);
}

void NodeDetailsComponent::mouseDown (const MouseEvent& e)
{
    if (! e.mods.isAnyModifierKeyDown()) // deselect current node
    {
        auto& manager = plugin.getManager();
        manager.setSelected (nullptr, NodeManager::ActionSource::DetailsView);
        manager.setSoloed (nullptr, NodeManager::ActionSource::DetailsView);
    }
}

void NodeDetailsComponent::paint (Graphics& g)
{
    g.fillAll (Colours::transparentBlack);
    constexpr float rectOffset = xPad / 2.0f;

    for (auto* nd : nodes)
    {
        if (nd->getNode()->getSelected())
        {
            g.setColour (findColour (NodeDetailsGUI::nodeColour, true));
            g.drawRect (nd->getBounds().toFloat().expanded (rectOffset, 0.0f).withHeight ((float) getHeight()), rectOffset);
            break;
        }
    }
}

void NodeDetailsComponent::resized()
{
    for (int i = 0; i < nodes.size(); ++i)
    {
        nodes[i]->setBounds (xOffset + (NodeInfoConsts::InfoWidthNoLabel + xPad) * i, 0, NodeInfoConsts::InfoWidthNoLabel, getHeight() - xPad);
    }
}

void NodeDetailsComponent::nodeAdded (DelayNode* newNode)
{
    MessageManagerLock mml;
    addNode (newNode);

    setSize (calcWidth(), calcHeight());
    resized();
    repaint();
}

void NodeDetailsComponent::nodeRemoved (DelayNode* nodeToRemove)
{
    for (auto* node : nodes)
    {
        if (node->getNode() == nodeToRemove)
        {
            MessageManagerLock mml;
            nodes.removeObject (node);
            break;
        }
    }

    nodeToRemove->removeNodeListener (this);

    MessageManagerLock mml;
    setSize (calcWidth(), calcHeight());
    resized();
    repaint();
}

void NodeDetailsComponent::setMinWidth (int newMinWidth)
{
    minWidth = newMinWidth;
    setSize (calcWidth(), calcHeight());
}

int NodeDetailsComponent::calcWidth() const
{
    int width = 2 * xOffset + (xPad + NodeInfoConsts::InfoWidthNoLabel) * nodes.size();
    return jmax (width, minWidth);
}

NodeDetails* NodeDetailsComponent::getNodeDetails (DelayNode* node)
{
    for (auto* nd : nodes)
        if (nd->getNode() == node)
            return nd;

    jassertfalse; // If this function is being called every node should be linked to a details component
    return nullptr;
}

Point<int> NodeDetailsComponent::getNodeDetailsPosition (DelayNode* node)
{
    for (auto* nd : nodes)
    {
        if (nd->getNode() == node)
            return nd->getPosition();
    }

    return Point<int>();
}
