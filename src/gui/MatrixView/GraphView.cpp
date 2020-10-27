#include "GraphView.h"

GraphView::GraphView (ChowMatrix& plugin) :
    plugin (plugin),
    manager (this),
    aurora (plugin.getInsanityParam())
{
    addAndMakeVisible (aurora);

    setColour (backgroundColour, Colours::darkgrey);
    setColour (nodeColour, Colours::pink);
    setColour (nodeSelectedColour, Colours::greenyellow);

    for (auto& node : plugin.inputNodes)
        manager.createAndAddEditor (&node);

    manager.doForAllNodes ([=] (DBaseNode*, DelayNode* child) { manager.createAndAddEditor (child); });
}

GraphView::~GraphView()
{
    for (auto& node : plugin.inputNodes)
        node.removeNodeListener (this);

    manager.doForAllNodes ([=] (DBaseNode*, DelayNode* child) { child->removeNodeListener (this); });
}

void GraphView::mouseDown (const MouseEvent&)
{
    setSelected (nullptr);
}

void GraphView::paint (Graphics& g)
{
    g.fillAll (findColour (backgroundColour));

    g.setColour (findColour (nodeColour));
    manager.doForAllNodes ([&g] (DBaseNode* root, DelayNode* childNode) {
        auto* editor = childNode->getEditor();
        auto rootPos = root->getEditor()->getCentrePosition();
        auto childPos = editor->getCentrePosition();
        g.drawLine (Line (rootPos, childPos).toFloat(), 2.0f);
    });
}

void GraphView::resized()
{
    aurora.setBounds (getLocalBounds());

    int idx = 1;
    for (auto* nodeComp : manager.inputNodeComponents)
    {
        nodeComp->setCentrePosition (idx * getWidth() / 3, getHeight());
        idx++;
    }

    manager.doForAllNodes ([=] (DBaseNode*, DelayNode* childNode) { childNode->getEditor()->updatePosition(); });
}

void GraphView::setSelected (DelayNode* node)
{
    plugin.getManager().setSelected (node);
}

void GraphView::nodeAdded (DelayNode* newNode)
{
    manager.createAndAddEditor (newNode);

    MessageManagerLock mml;
    resized();
    repaint();
}

void GraphView::nodeRemoved (DelayNode* newNode)
{
    manager.removeEditor (newNode);

    MessageManagerLock mml;
    resized();
    repaint();
}
