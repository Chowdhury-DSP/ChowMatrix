#include "NodeDetails.h"
#include "NodeDetailsGUI.h"
#include "../MatrixView/NodeComponent.h"

using namespace DetailsConsts;

NodeDetails::NodeDetails (DelayNode& node, NodeManager& manager) :
    manager (manager),
    nodeInfo (node, false),
    button (*this)
{
    node.setNodeDetails (this);

    addAndMakeVisible (nodeInfo);
    addAndMakeVisible (button);
}

NodeDetails::~NodeDetails()
{
    getNode()->setNodeDetails (nullptr);
}

void NodeDetails::resized()
{
    button.setBounds (0, 0, getWidth(), buttonHeight);
    nodeInfo.setBounds (0, buttonHeight, getWidth(), getHeight() - buttonHeight);
}

NodeDetails::Button::Button (NodeDetails& nodeDetails) :
    nodeDetails (nodeDetails)
{
    setWantsKeyboardFocus (true);
}

void NodeDetails::Button::paint (Graphics& g)
{
    const int x = getWidth() / 2 - circleRadius;
    const int y = getHeight() / 2 - circleRadius;
    auto bounds = Rectangle<int> (x, y, 2 * circleRadius, 2 * circleRadius).toFloat();

    bool isSelected = nodeDetails.getNode()->getSelected();
    Colour cc = isSelected ?
        findColour (NodeDetailsGUI::nodeSelectedColour, true) :
        findColour (NodeDetailsGUI::nodeColour, true);

    g.setColour (cc);
    g.fillEllipse (bounds);

    if (isSelected)
    {
        g.setColour (Colours::white);
        g.drawEllipse (bounds.reduced (1.0f), 2.0f);
    }

    g.setColour (Colours::white);
    g.drawFittedText (String (nodeDetails.getNode()->getIndex() + 1),
        bounds.toNearestInt(), Justification::centred, 1);
}

void NodeDetails::Button::mouseDown (const MouseEvent& e)
{
    nodeDetails.setSelected();
    grabKeyboardFocus();
}

bool NodeDetails::Button::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::deleteKey || key == KeyPress::backspaceKey)
    {
        if (nodeDetails.getNode()->getSelected())
            nodeDetails.getNode()->deleteNode();
            
        return true;
    }

    return false;
}
