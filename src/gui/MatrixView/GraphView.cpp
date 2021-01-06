#include "GraphView.h"
#include "GraphViewItem.h"

GraphView::GraphView (ChowMatrix& plugin, Viewport& parentView) :
    plugin (plugin),
    manager (this),
    parent (parentView)
{
    setName ("Graph View");
    setTooltip ("Graph view of all delay nodes, shift+click to create a new node");

    setColour (backgroundColour, Colour (0xFF162947));
    setColour (nodeColour, Colour (0xFFC954D4));
    setColour (nodeColour2, Colour (0xFFE0B500));
    setColour (nodeSelectedColour, Colour (0xFF21CCA5));

    auto& inputNodes = *plugin.getNodes();
    manager.createAndAddEditor (&inputNodes[0], findColour (nodeColour),  -0.04f);
    manager.createAndAddEditor (&inputNodes[1], findColour (nodeColour2), -0.02f);

    manager.doForAllNodes ([=] (DBaseNode*, DelayNode* child) { manager.createAndAddEditor (child); });
}

GraphView::~GraphView()
{
    for (auto& node : *plugin.getNodes())
        node.removeNodeListener (this);

    manager.doForAllNodes ([=] (DBaseNode*, DelayNode* child) { child->removeNodeListener (this); });
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

void GraphView::mouseDrag (const MouseEvent& e)
{
    auto compE = e.withNewPosition (e.originalComponent->getPosition());
    Component::beginDragAutoRepeat (10);
    parent.mouseDrag (compE);
}

void GraphView::paint (Graphics& g)
{
    g.fillAll (findColour (backgroundColour, true));

    manager.doForAllNodes ([=, &g] (DBaseNode* root, DelayNode* childNode) {
        auto* editor = childNode->getEditor();
        if (editor == nullptr) // editor has not been created yet!
            return;

        auto rootPos = root->getEditor()->getCentrePosition();
        auto childPos = editor->getCentrePosition();

        const auto alphaMult = childNode->getSoloed() == DelayNode::SoloState::Mute ? 0.4f : 1.0f;
        g.setColour (findColour (nodeColour, true).withMultipliedAlpha (alphaMult));
        g.drawLine (Line<int> (rootPos, childPos).toFloat(), 2.0f);
    });
}

void GraphView::parentSizeChanged (int parentWidth, int parentHeight)
{
    int xOffset = (getWidth() - parentWidth) / 2;
    visibleHeight = parentHeight;

    int idx = 1;
    for (auto* nodeComp : manager.inputNodeComponents)
    {
        nodeComp->setCentrePosition (xOffset + idx * parentWidth / 3, getHeight());
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
