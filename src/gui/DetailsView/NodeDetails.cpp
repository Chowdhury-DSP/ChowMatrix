#include "NodeDetails.h"
#include "NodeDetailsGUI.h"
#include "../MatrixView/NodeComponent.h"

using namespace DetailsConsts;

NodeDetails::NodeDetails (DelayNode& node) :
    nodeInfo (node, false),
    button (nodeInfo)
{
    addAndMakeVisible (nodeInfo);
    addAndMakeVisible (button);
}

void NodeDetails::resized()
{
    button.setBounds (0, 0, getWidth(), buttonHeight);
    nodeInfo.setBounds (0, buttonHeight, getWidth(), getHeight() - buttonHeight);
}

NodeDetails::Button::Button (NodeInfo& nodeInfo) :
    nodeInfo (nodeInfo)
{
}

void NodeDetails::Button::paint (Graphics& g)
{
    const int x = getWidth() / 2 - circleRadius;
    const int y = getHeight() / 2 - circleRadius;

    Colour cc = true ? // nodeInfo.getNode()->getEditor()->isSelected() ?
        findColour (NodeDetailsGUI::nodeSelectedColour, true) :
        findColour (NodeDetailsGUI::nodeColour, true);

    g.setColour (cc);
    g.fillEllipse (x, y, 2 * circleRadius, 2 * circleRadius);
}
