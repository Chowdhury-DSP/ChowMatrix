#include "NodeComponent.h"
#include "GraphViewItem.h"
#include "../../dsp/InputNode.h"

NodeComponent::NodeComponent (DBaseNode& node, GraphView* view) :
    graphView (view),
    node (node)
{
    setSize (32, 32);
}

void NodeComponent::paint (Graphics& g)
{
    g.setColour (nodeColour);
    g.fillEllipse (getLocalBounds().toFloat());
}
