#include "BaseNode.h"
#include "DelayNode.h"
#include "../gui/DelayNodeComponent.h"

BaseNode::BaseNode()
{

}

Component* BaseNode::getEditor()
{
    return editor;
}

BaseNode* BaseNode::addChild()
{
    // @TODO: make DelayNode
    children.add (std::make_unique<DelayNode>());
    children.getLast()->parent = this;

    return children.getLast();
}
