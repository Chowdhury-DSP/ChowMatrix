#include "BaseNode.h"
#include "InputNode.h"
#include "DelayNode.h"
#include "../gui/MatrixView/DelayNodeComponent.h"

template<typename Child>
BaseNode<Child>::BaseNode()
{

}

template<typename Child>
void BaseNode<Child>::prepare (double newSampleRate, int newSamplesPerBlock)
{
    sampleRate = newSampleRate;
    samplesPerBlock = newSamplesPerBlock;

    for (auto* child : children)
        child->prepare (sampleRate, samplesPerBlock);
}

template<typename Child>
void BaseNode<Child>::process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples)
{
    for (auto* child : children)
        child->process (inBuffer, outBuffer, numSamples);
}

template<typename Child>
NodeComponent* BaseNode<Child>::getEditor()
{
    return editor;
}

template<typename Child>
Child* BaseNode<Child>::addChild()
{
    auto newChild = std::make_unique<Child>();
    newChild->setParent (this);
    auto* createdChild = children.add (std::move (newChild));

    nodeListeners.call (&BaseNode<Child>::Listener::nodeAdded, createdChild);

    return createdChild;
}

template<typename Child>
void BaseNode<Child>::removeChild (Child* childToRemove)
{
    int indexToRemove = children.indexOf (childToRemove);
    nodeBeingDeleted.reset (children.removeAndReturn (indexToRemove));
}

template<typename Child>
void BaseNode<Child>::clearChildren()
{
    while (! children.isEmpty())
        children.getLast()->deleteNode();
    
    nodeBeingDeleted.reset();
}

template<typename Child>
void BaseNode<Child>::setParent (BaseNode* newParent)
{
    parent = newParent;
    prepare (parent->sampleRate, parent->samplesPerBlock);
}

template<typename Child>
XmlElement* BaseNode<Child>::saveXml()
{
    std::unique_ptr<XmlElement> xml = std::make_unique<XmlElement> ("children");
    for (auto* child : children)
        xml->addChildElement (child->saveXml());
    
    return xml.release();
}

template<typename Child>
void BaseNode<Child>::loadXml (XmlElement* xml)
{
    if (xml == nullptr)
        return;

    if (xml->hasTagName ("children"))
    {
        forEachXmlChildElement (*xml, childXml)
            addChild()->loadXml (childXml);
    }
}

template class BaseNode<DelayNode>;
