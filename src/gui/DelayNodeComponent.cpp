#include "DelayNodeComponent.h"
#include "GraphView.h"

DelayNodeComponent::DelayNodeComponent (DelayNode& node, GraphView* view) :
    NodeComponent (node, view),
    node (node),
    nodeInfo (node)
{
    view->addChildComponent (nodeInfo);
}

void DelayNodeComponent::mouseDown (const MouseEvent&)
{
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
    NodeComponent::paint (g);

    if (isSelected)
    {
        g.setColour (Colours::white);
        g.drawEllipse (getLocalBounds().toFloat().reduced (1.0f), 1.0f);
    }
}

void DelayNodeComponent::updateParams()
{
    const float maxDist = (float) graphView->getHeight() / 5.0f;

    auto parentEditor = node.getParent()->getEditor();
    auto parentPos = parentEditor->getCentrePosition().toFloat();
    auto myPos = getCentrePosition().toFloat();

    auto newDelay = DelayConsts::maxDelay * jlimit (0.0f, 1.0f, myPos.getDistanceFrom (parentPos) / maxDist);
    node.setDelay (newDelay);

    auto newPan = jlimit (-1.0f, 1.0f, parentPos.getAngleToPoint (myPos) / MathConstants<float>::halfPi);
    node.setPan (newPan);
}

void DelayNodeComponent::updatePosition()
{
    const float maxDist = (float) graphView->getHeight() / 5.0f;

    auto parentEditor = node.getParent()->getEditor();
    auto parentPos = parentEditor->getCentrePosition().toFloat();

    float delay = node.getDelayMs();
    float pan = node.getPan();

    float radius = (delay / DelayConsts::maxDelay) * maxDist;
    float angle = (pan - 1.0f) * MathConstants<float>::halfPi;
    setCentrePosition (parentPos.translated (radius * std::cos (angle), radius * std::sin (angle)).toInt());
    
    for (int i = 0; i < node.getNumChildren(); ++i)
        node.getChild (i)->getEditor()->updatePosition();

    nodeInfo.setTopLeftPosition (getPosition().translated (getWidth() + 5, 0));

    graphView->repaint();
}

void DelayNodeComponent::setSelected (bool shouldBeSelected)
{
    isSelected = shouldBeSelected;
    nodeInfo.setVisible (isSelected);
    repaint();
}
