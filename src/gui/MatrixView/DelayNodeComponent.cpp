#include "DelayNodeComponent.h"
#include "GraphView.h"

using namespace ParamTags;

DelayNodeComponent::DelayNodeComponent (DelayNode& node, GraphView* view) :
    NodeComponent (node, view),
    node (node),
    nodeInfo (node)
{
    setWantsKeyboardFocus (true);
    view->addChildComponent (nodeInfo);
    node.getNodeParameter (delayTag)->addListener (this);
    node.getNodeParameter (panTag)->addListener (this);

    setName ("Delay Node");
    setTooltip ("Click to select node, drag to move, alt+click to solo, ctrl+click to delete");
}

DelayNodeComponent::~DelayNodeComponent()
{
    node.getNodeParameter (delayTag)->removeListener (this);
    node.getNodeParameter (panTag)->removeListener (this);
}

void DelayNodeComponent::mouseDown (const MouseEvent& e)
{
    if (e.mods.isCommandDown())
    {
        node.deleteNode();
        return;
    }

    if (e.mods.isAltDown())
        graphView->setSoloed (&node);

    graphView->setSelected (&node);
    grabKeyboardFocus();
}

void DelayNodeComponent::mouseDrag (const MouseEvent& e)
{
    setCentrePosition (e.getEventRelativeTo (graphView).getPosition());
    updateParams();
    updatePosition();
}

bool DelayNodeComponent::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::deleteKey || key == KeyPress::backspaceKey)
    {
        if (node.getSelected())
            node.deleteNode();

        return true;
    }

    return false;
}

void DelayNodeComponent::paint (Graphics& g)
{
    // damp colours for muted nodes
    const auto alphaMult = node.getSoloed() == DelayNode::SoloState::Mute ? 0.4f : 1.0f;

    if (node.getSelected())
    {
        g.setColour (findColour (GraphView::nodeSelectedColour, true)
            .withMultipliedAlpha (alphaMult));
        g.fillEllipse (getLocalBounds().toFloat());

        g.setColour (Colours::white.withMultipliedAlpha (alphaMult));
        g.drawEllipse (getLocalBounds().toFloat().reduced (1.0f), 2.0f);
    }
    else
    {
        g.setColour (findColour (GraphView::nodeColour, true)
            .withMultipliedAlpha (alphaMult));
        g.fillEllipse (getLocalBounds().toFloat());
    }

    g.setColour (Colours::white.withMultipliedAlpha (alphaMult));
    g.drawFittedText (String (node.getIndex() + 1), getLocalBounds(), Justification::centred, 1);
}

float DelayNodeComponent::getMaxDist() const noexcept
{
    return (float) graphView->getHeight() / 2.0f;
}

void DelayNodeComponent::updateParams()
{
    auto parentEditor = node.getParent()->getEditor();
    auto parentPos = parentEditor->getCentrePosition().toFloat();
    auto myPos = getCentrePosition().toFloat();

    auto newDelay = jlimit (0.0f, 1.0f, myPos.getDistanceFrom (parentPos) / getMaxDist());
    node.setDelay (newDelay);

    auto newPan = jlimit (-1.0f, 1.0f, parentPos.getAngleToPoint (myPos) / MathConstants<float>::halfPi);
    node.setPan (newPan);
}

void DelayNodeComponent::updatePosition()
{
    auto parentEditor = node.getParent()->getEditor();
    auto parentPos = parentEditor->getCentrePosition().toFloat();

    float delay = node.getDelay();
    float pan = node.getPan();

    float radius = delay * getMaxDist();
    float angle = (pan - 1.0f) * MathConstants<float>::halfPi;
    setCentrePosition (parentPos.translated (radius * std::cos (angle), radius * std::sin (angle)).toInt());
    
    for (int i = 0; i < node.getNumChildren(); ++i)
        node.getChild (i)->getEditor()->updatePosition();

    constexpr int pad = 5;
    bool showOnRight = (pan >= 0.0f
        || getPosition().translated(-(nodeInfo.getWidth() + pad), 0).x < 0)
        && getPosition().translated(nodeInfo.getWidth() + pad, 0).x < graphView->getWidth();

    int x = showOnRight ? getWidth() + pad : -(nodeInfo.getWidth() + pad);
    int y = jmin ((int) parentPos.y - getPosition().translated (0, nodeInfo.getHeight()).y, 0);
    auto infoPos = getPosition().translated (x, y);
    infoPos.setY (jmax (infoPos.y, 0));
    nodeInfo.setTopLeftPosition (infoPos);

    graphView->repaint();
}

void DelayNodeComponent::parameterValueChanged (int, float)
{
    updatePosition();
}

void DelayNodeComponent::selectionChanged()
{
    bool isSelected = node.getSelected();
    nodeInfo.setVisible (isSelected);
    nodeInfo.toFront (true);
}
