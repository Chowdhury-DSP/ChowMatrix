#include "DelayNode.h"
#include "../gui/DelayNodeComponent.h"

DelayNode::DelayNode()
{

}

std::unique_ptr<NodeComponent> DelayNode::createEditor (GraphView* view)
{
    auto editorPtr = std::make_unique<DelayNodeComponent> (*this, view);
    editor = editorPtr.get();
    return std::move (editorPtr);
}
