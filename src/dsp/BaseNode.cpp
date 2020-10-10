#include "BaseNode.h"
#include "InputNode.h"
#include "DelayNode.h"
#include "../gui/DelayNodeComponent.h"

template<typename Child>
BaseNode<Child>::BaseNode()
{

}

template<typename Child>
NodeComponent* BaseNode<Child>::getEditor()
{
    return editor;
}

template<typename Child>
Child* BaseNode<Child>::addChild()
{
    auto newChild = children.add (std::make_unique<Child>());
    newChild->parent = this;

    return newChild;
}

template class BaseNode<DelayNode>;
