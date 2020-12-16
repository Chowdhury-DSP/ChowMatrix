#include "NodeDetails.h"
#include "NodeDetailsGUI.h"
#include "../MatrixView/NodeComponent.h"

using namespace DetailsConsts;

NodeDetails::NodeDetails (DelayNode& node, NodeManager& manager) :
    manager (manager),
    nodeInfo (node, false),
    button (*this)
{
    addAndMakeVisible (nodeInfo);
    addAndMakeVisible (button);
}

NodeDetails::~NodeDetails()
{
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

    setName ("Node Details");
    setTooltip ("Click to select this node, alt+click to solo, press \"Delete\" to delete");
}

void NodeDetails::Button::paint (Graphics& g)
{
    const int x = getWidth() / 2 - circleRadius;
    const int y = getHeight() / 2 - circleRadius;
    auto bounds = Rectangle<int> (x, y, 2 * circleRadius, 2 * circleRadius).toFloat();
    auto* node = nodeDetails.getNode();
    const auto alphaMult = node->getSoloed() == DelayNode::SoloState::Mute ? 0.4f : 1.0f;

    bool isSelected = node->getSelected();
    Colour cc = isSelected ?
        findColour (NodeDetailsGUI::nodeSelectedColour, true) :
        findColour (NodeDetailsGUI::nodeColour, true);

    g.setColour (cc.withMultipliedAlpha (alphaMult));
    g.fillEllipse (bounds);

    if (isSelected)
    {
        g.setColour (Colours::white.withMultipliedAlpha (alphaMult));
        g.drawEllipse (bounds.reduced (1.0f), 2.0f);
    }

    g.setColour (Colours::white.withMultipliedAlpha (alphaMult));
    int nodeIdx = node->getIndex();
    jassert (nodeIdx >= 0);
    g.drawFittedText (String (nodeIdx + 1), bounds.toNearestInt(), Justification::centred, 1);
}

void NodeDetails::Button::mouseDown (const MouseEvent& e)
{
    if (e.mods.isAltDown())
        nodeDetails.setSoloed();

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
