#include "InputNode.h"
#include "../gui/MatrixView/InputNodeComponent.h"

std::unique_ptr<NodeComponent> InputNode::createNodeEditor (GraphView* view)
{
    auto editorPtr = std::make_unique<InputNodeComponent> (*this, view);
    editor = editorPtr.get();
    return std::move (editorPtr);
}
