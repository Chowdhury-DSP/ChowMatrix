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

    setTooltip ("Graph view of all delay nodes, shift+click to create a new node");

    for (auto& node : *plugin.getNodes())
        manager.createAndAddEditor (&node);

    manager.doForAllNodes ([=] (DBaseNode*, DelayNode* child) { manager.createAndAddEditor (child); });

    plugin.getManager().addListener (this);
}

GraphView::~GraphView()
{
    for (auto& node : *plugin.getNodes())
        node.removeNodeListener (this);

    manager.doForAllNodes ([=] (DBaseNode*, DelayNode* child) { child->removeNodeListener (this); });

    plugin.getManager().removeListener (this);
}

void GraphView::mouseDown (const MouseEvent& e)
{
    if (! e.mods.isAnyModifierKeyDown()) // deselect current node
    {
        setSelected (nullptr);
        setSoloed (nullptr);
        return;   
    }

    if (e.mods.isShiftDown()) // create new node at mouse position
    {
        // lambda to add child to parent and move to mouse position
        auto addNode = [&e] (DBaseNode* parent) { parent->addChild()->getEditor()->mouseDrag (e); };

        // if a node is selected create child from that node
        if (auto selectedNode = plugin.getManager().getSelected())
        {
            addNode (selectedNode);
            return;
        }

        // otherwise create a node from whichever side the mouse is on
        bool mouseSide = e.getPosition().x > getWidth() / 2;
        addNode (&plugin.getNodes()->at ((size_t) mouseSide));
    }
}

void GraphView::paint (Graphics& g)
{
    g.fillAll (findColour (backgroundColour));

    manager.doForAllNodes ([=, &g] (DBaseNode* root, DelayNode* childNode) {
        auto* editor = childNode->getEditor();
        if (editor == nullptr) // editor has not been created yet!
            return;

        auto rootPos = root->getEditor()->getCentrePosition();
        auto childPos = editor->getCentrePosition();

        const auto alphaMult = childNode->getSoloed() == DelayNode::SoloState::Mute ? 0.4f : 1.0f;
        g.setColour (findColour (nodeColour).withMultipliedAlpha (alphaMult));
        g.drawLine (Line<int> (rootPos, childPos).toFloat(), 2.0f);
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
    plugin.getManager().setSelected (node, NodeManager::ActionSource::GraphView);
}

void GraphView::setSoloed (DelayNode* node)
{
    plugin.getManager().setSoloed (node, NodeManager::ActionSource::GraphView);
}

void GraphView::nodeAdded (DelayNode* newNode)
{
    MessageManagerLock mml;
    manager.createAndAddEditor (newNode);

    resized();
    repaint();
}

void GraphView::nodeRemoved (DelayNode* newNode)
{
    MessageManagerLock mml;
    manager.removeEditor (newNode);

    resized();
    repaint();
}

void GraphView::nodeSelected (DelayNode* /*selectedNode*/, NodeManager::ActionSource /*source*/)
{
    repaint();

    for (auto* comp : manager.delayNodeComponents)
        comp->selectionChanged();
}

void GraphView::nodeSoloed (DelayNode* /*soloedNode*/, NodeManager::ActionSource /*source*/)
{
    repaint();
}

void GraphView::nodeParamLockChanged (DelayNode* node)
{
    for (auto* comp : manager.delayNodeComponents)
        if (&comp->getNode() == node)
            comp->getNodeInfo().repaint();
}
