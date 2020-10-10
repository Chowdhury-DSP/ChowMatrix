#pragma once

#include "NodeComponent.h"
#include "../dsp/InputNode.h"

class InputNodeComponent : public NodeComponent
{
public:
    InputNodeComponent (InputNode& node, GraphView* view) :
        NodeComponent (node, view)
    {}
};
