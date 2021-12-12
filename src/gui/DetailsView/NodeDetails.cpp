#include "NodeDetails.h"
#include "../MatrixView/NodeComponent.h"
#include "NodeDetailsGUI.h"
#include "gui/BottomBar/BottomBarLNF.h"
#include "../IOSUtils/PopupMenuOptionsHelpers.h"

using namespace DetailsConsts;

NodeDetails::NodeDetails (DelayNode& node, NodeManager& manager) : manager (manager),
                                                                   nodeInfo (node, false),
                                                                   button (*this)
{
    addAndMakeVisible (nodeInfo);
    addAndMakeVisible (button);
}

NodeDetails::~NodeDetails() = default;

void NodeDetails::resized()
{
    button.setBounds (0, 0, getWidth(), buttonHeight);
    nodeInfo.setBounds (0, buttonHeight, getWidth(), getHeight() - buttonHeight);
}

NodeDetails::Button::Button (NodeDetails& nd) : nodeDetails (nd)
{
    setWantsKeyboardFocus (true);

    setName ("Node Details");
    setTooltip ("Click to select this node, alt+click to solo, press \"Delete\" to delete");

#if JUCE_IOS
    longPressAction.longPressCallback = [=] (Point<int>)
    {
        PopupMenu actionMenu;
        actionMenu.addItem ("Solo Node", [=]
                            { nodeDetails.setSoloed(); });
        actionMenu.addItem ("Delete Node", [=]
                            {
            if (nodeDetails.getNode()->getSelected())
                nodeDetails.getNode()->deleteNode(); });

        actionMenu.setLookAndFeel (lnfAllocator->getLookAndFeel<BottomBarLNF>());
        actionMenu.showMenuAsync (PopupMenuOptionsHelpers::createPopupMenuOptions (this));
    };
#endif
}

void NodeDetails::Button::paint (Graphics& g)
{
    const int x = getWidth() / 2 - circleRadius;
    const int y = getHeight() / 2 - circleRadius;
    auto bounds = Rectangle<int> (x, y, 2 * circleRadius, 2 * circleRadius).toFloat();
    auto* node = nodeDetails.getNode();
    const auto alphaMult = node->getSoloed() == DelayNode::SoloState::Mute ? 0.4f : 1.0f;

    if (node->getSelected())
    {
        g.setColour (findColour (NodeDetailsGUI::nodeSelectedColour, true));
        g.fillEllipse (bounds);

        g.setColour (Colours::white);
        g.drawEllipse (bounds.reduced (1.0f), 2.0f);
    }
    else
    {
        const auto* editor = nodeDetails.getNode()->getEditor();
        if (editor != nullptr)
        {
            g.setColour (editor->getColour().withMultipliedAlpha (alphaMult));
            g.fillEllipse (bounds);
        }
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

#if JUCE_IOS
    longPressAction.startPress (e.getMouseDownPosition());
#endif
}

void NodeDetails::Button::mouseDrag (const MouseEvent& e)
{
#if JUCE_IOS
    longPressAction.setDragDistance ((float) e.getDistanceFromDragStart());
#else
    Component::mouseDrag (e);
#endif
}

void NodeDetails::Button::mouseUp (const MouseEvent& e)
{
#if JUCE_IOS
    ignoreUnused (e);
    longPressAction.abortPress();
#else
    Component::mouseUp (e);
#endif
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
