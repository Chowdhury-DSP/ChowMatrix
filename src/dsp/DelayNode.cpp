#include "DelayNode.h"
#include "../gui/DelayNodeComponent.h"

DelayNode::DelayNode()
{

}

std::unique_ptr<Component> DelayNode::createEditor()
{
    auto editorPtr = std::make_unique<DelayNodeComponent> (*this);
    editor = editorPtr.get();
    return std::move (editorPtr);
}
