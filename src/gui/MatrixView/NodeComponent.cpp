#include "NodeComponent.h"
#include "../../dsp/InputNode.h"
#include "GraphViewItem.h"

NodeComponent::NodeComponent (DBaseNode& node, GraphView* view) : graphView (view),
                                                                  node (node)
{
    setSize (32, 32);
}

void NodeComponent::paint (Graphics& g)
{
    g.setColour (nodeColour);
    g.fillEllipse (getLocalBounds().toFloat());
}
