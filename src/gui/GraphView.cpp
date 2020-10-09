#include "GraphView.h"
#include "DelayNodeComponent.h"

GraphView::GraphView (ChowMatrix& plugin)
{
    setColour (backgroundColour, Colours::darkgrey);
    setColour (nodeColour, Colours::greenyellow);

    for (auto& node : plugin.inputNodes)
    {
        nodeComponents.add (node.createEditor());
        addAndMakeVisible (nodeComponents.getLast());
    }
}

void drawNodeConnections (NodeComponent* root, Graphics& g)
{
    if (root == nullptr)
        return;

    auto& rootNode = root->getNode();
    auto rootPos = root->getPosition().translated (root->getWidth() / 2, root->getHeight() / 2);

    for (int i = 0; i < rootNode.getNumChildren(); ++i)
    {
        auto* childNode = rootNode.getChild (i);
        auto* editor = dynamic_cast<DelayNodeComponent*> (childNode->getEditor());
        auto childPos = editor->getPosition().translated (editor->getWidth() / 2, editor->getHeight() / 2);

        g.drawLine (Line (rootPos, childPos).toFloat());
        drawNodeConnections (editor, g);
    }
}

void GraphView::paint (Graphics& g)
{
    g.fillAll (findColour (backgroundColour));

    g.setColour (findColour (nodeColour));
    drawNodeConnections (dynamic_cast<NodeComponent*> (nodeComponents[0]), g);
    drawNodeConnections (dynamic_cast<NodeComponent*> (nodeComponents[1]), g);
}

void positionNodes (NodeComponent* root)
{
    if (root == nullptr)
        return;

    auto& rootNode = root->getNode();
    auto rootPos = root->getPosition().translated (root->getWidth() / 2, root->getHeight() / 2);

    for (int i = 0; i < rootNode.getNumChildren(); ++i)
    {
        auto* childNode = rootNode.getChild (i);
        auto* editor = dynamic_cast<DelayNodeComponent*> (childNode->getEditor());
        editor->setCentrePosition (rootPos + editor->getDistance().toInt());
        positionNodes (editor);
    }
}

void GraphView::resized()
{
    nodeComponents[0]->setCentrePosition (1 * getWidth() / 3, getHeight());
    nodeComponents[1]->setCentrePosition (2 * getWidth() / 3, getHeight());

    positionNodes (dynamic_cast<NodeComponent*> (nodeComponents[0]));
    positionNodes (dynamic_cast<NodeComponent*> (nodeComponents[1]));
}

void GraphView::addNode (BaseNode* newNode)
{
    nodeComponents.add (newNode->createEditor());
    addAndMakeVisible (nodeComponents.getLast());
    resized();
    repaint();
}
