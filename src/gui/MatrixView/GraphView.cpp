#include "GraphView.h"

GraphView::GraphView (ChowMatrix& plugin) :
    plugin (plugin),
    manager (this)
{
    setColour (backgroundColour, Colours::darkgrey);
    setColour (nodeColour, Colours::greenyellow);

    for (auto& node : plugin.inputNodes)
        manager.createAndAddEditor (&node);

    manager.doForAllNodes ([=] (NodeComponent*, DelayNode* child) { manager.createAndAddEditor (child); });
}

GraphView::~GraphView()
{
    for (auto& node : plugin.inputNodes)
        node.removeNodeListener (this);

    manager.doForAllNodes ([=] (NodeComponent*, DelayNode* child) { child->removeNodeListener (this); });
}

void GraphView::mouseDown (const MouseEvent&)
{
    clearSelected();
}

void GraphView::paint (Graphics& g)
{
    g.fillAll (findColour (backgroundColour));

    g.setColour (findColour (nodeColour));
    manager.doForAllNodes ([&g] (NodeComponent* root, DelayNode* childNode) {
        auto* editor = childNode->getEditor();
        auto rootPos = root->getCentrePosition();
        auto childPos = editor->getCentrePosition();
        g.drawLine (Line (rootPos, childPos).toFloat());
    });
}

void GraphView::resized()
{
    int idx = 1;
    for (auto* nodeComp : manager.inputNodeComponents)
    {
        nodeComp->setCentrePosition (idx * getWidth() / 3, getHeight());
        idx++;
    }

    manager.doForAllNodes ([=] (NodeComponent*, DelayNode* childNode) { childNode->getEditor()->updatePosition(); });
}

void GraphView::clearSelected()
{
    for (auto* nodeComp : manager.delayNodeComponents)
        nodeComp->setSelected (false);
}

void GraphView::nodeAdded (DelayNode* newNode)
{
    manager.createAndAddEditor (newNode);

    MessageManagerLock mml;
    resized();
    repaint();
}
