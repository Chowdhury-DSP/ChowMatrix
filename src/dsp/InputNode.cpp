#include "InputNode.h"
#include "../gui/MatrixView/InputNodeComponent.h"

XmlElement* InputNode::saveXml()
{
    std::unique_ptr<XmlElement> xml = std::make_unique<XmlElement> ("input_node");
    xml->addChildElement (DBaseNode::saveXml());

    return xml.release();
}

void InputNode::loadXml (XmlElement* xmlState)
{
    if (xmlState == nullptr)
        return;

    if (xmlState->hasTagName ("input_node"))
    {
        if (auto* childrenXml = xmlState->getChildByName ("children"))
            DBaseNode::loadXml (childrenXml);
    }
}

std::unique_ptr<NodeComponent> InputNode::createNodeEditor (GraphView* view)
{
    auto editorPtr = std::make_unique<InputNodeComponent> (*this, view);
    editor = editorPtr.get();
    return std::move (editorPtr);
}
