#include "DelayNodeComponent.h"

DelayNodeComponent::DelayNodeComponent (DelayNode& node) :
    NodeComponent (node),
    node (node)
{

}

void DelayNodeComponent::mouseDrag (const MouseEvent& e)
{
    auto* parent = getParentComponent();
    jassert (parent);

    setCentrePosition (e.getEventRelativeTo (parent).getPosition());
    parent->repaint();

    updatePosition();
}

Point<float> DelayNodeComponent::getDistance()
{
    const float maxDist = (float) getParentComponent()->getHeight() / 5.0f;
    return { 0.0f, -maxDist * (node.getDelayMs() / DelayConsts::maxDelay) };
}

void DelayNodeComponent::updatePosition()
{
    const float maxDist = (float) getParentComponent()->getHeight() / 5.0f;

    auto parentPos = node.getParent()->getEditor()->getPosition().toFloat();
    auto newDelay = DelayConsts::maxDelay * (getPosition().toFloat().getDistanceFrom (parentPos) / maxDist);
    node.setDelay (newDelay);
}
