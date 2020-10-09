#include "NodeComponent.h"
#include "GraphView.h"

NodeComponent::NodeComponent (BaseNode& node) :
    node (node)
{
    setSize (30, 30);
}

void NodeComponent::paint (Graphics& g)
{
    g.setColour (findColour (GraphView::nodeColour, true));
    g.fillEllipse (getLocalBounds().toFloat());
}

void NodeComponent::mouseDoubleClick (const MouseEvent& e)
{
    auto newNode = node.addChild();

    auto graphView = dynamic_cast<GraphView*> (getParentComponent());
    graphView->addNode (newNode);
}
