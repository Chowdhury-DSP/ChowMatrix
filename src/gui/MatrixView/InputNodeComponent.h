#pragma once

#include "../../dsp/InputNode.h"
#include "NodeComponent.h"

class InputNodeComponent : public NodeComponent,
                           public SettableTooltipClient
{
public:
    InputNodeComponent (InputNode& node, GraphView* view) : NodeComponent (node, view)
    {
        setName ("Input Node");
        setTooltip ("Represents the plugin input for either the left or right channel");
    }
};
