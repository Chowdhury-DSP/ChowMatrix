#include "DelayNodeComponent.h"
#include "GraphView.h"

using namespace ParamTags;

DelayNodeComponent::DelayNodeComponent (DelayNode& node, GraphView* view) :
    NodeComponent (node, view),
    node (node),
    nodeInfo (node)
{
    view->addChildComponent (nodeInfo);
    node.getNodeParameter (delayTag)->addListener (this);
    node.getNodeParameter (panTag)->addListener (this);
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

    graphView->clearSelected();
    setSelected (true);
}

void DelayNodeComponent::mouseDrag (const MouseEvent& e)
{
    setCentrePosition (e.getEventRelativeTo (graphView).getPosition());
    updateParams();
    updatePosition();
}

void DelayNodeComponent::paint (Graphics& g)
{
    if (isSelected)
    {
        g.setColour (findColour (GraphView::nodeSelectedColour, true));
        g.fillEllipse (getLocalBounds().toFloat());

        g.setColour (Colours::white);
        g.drawEllipse (getLocalBounds().toFloat().reduced (1.0f), 2.0f);
        return;
    }

    NodeComponent::paint (g);
}

float DelayNodeComponent::getMaxDist() const noexcept
{
    return (float) graphView->getHeight() / 4.0f;
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
    nodeInfo.setTopLeftPosition (getPosition().translated (x, y));

    graphView->repaint();
}

void DelayNodeComponent::parameterValueChanged (int, float)
{
    updatePosition();
}

void DelayNodeComponent::setSelected (bool shouldBeSelected)
{
    isSelected = shouldBeSelected;
    nodeInfo.setVisible (isSelected);
    nodeInfo.toFront (true);
    repaint();
}
