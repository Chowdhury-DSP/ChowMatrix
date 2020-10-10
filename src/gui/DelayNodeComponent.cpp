#include "DelayNodeComponent.h"

DelayNodeComponent::DelayNodeComponent (DelayNode& node, GraphView* view) :
    NodeComponent (node, view),
    node (node)
{

}

void DelayNodeComponent::mouseDrag (const MouseEvent& e)
{
    auto* parent = getParentComponent();
    setCentrePosition (e.getEventRelativeTo (parent).getPosition());
    updateParams();
    updatePosition();
}

void DelayNodeComponent::updateParams()
{
    const float maxDist = (float) getParentComponent()->getHeight() / 5.0f;

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
    const float maxDist = (float) getParentComponent()->getHeight() / 5.0f;

    auto parentEditor = node.getParent()->getEditor();
    auto parentPos = parentEditor->getCentrePosition().toFloat();

    float delay = node.getDelayMs();
    float pan = node.getPan();

    float radius = (delay / DelayConsts::maxDelay) * maxDist;
    float angle = (pan - 1.0f) * MathConstants<float>::halfPi;
    setCentrePosition (parentPos.translated (radius * std::cos (angle), radius * std::sin (angle)).toInt());
    
    for (int i = 0; i < node.getNumChildren(); ++i)
        node.getChild (i)->getEditor()->updatePosition();

    auto* parent = getParentComponent();
    parent->repaint();
}
