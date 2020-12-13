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
    childBuffer.setSize (1, newSamplesPerBlock);

    for (auto* child : children)
        child->prepare (sampleRate, samplesPerBlock);
}

template<typename Child>
void BaseNode<Child>::process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer)
{
    for (auto* child : children)
    {
        childBuffer.makeCopyOf (inBuffer, true);
        child->process (childBuffer, outBuffer);
    }
}

template<typename Child>
NodeComponent* BaseNode<Child>::getEditor()
{
    return editor;
}

template<typename Child>
Child* BaseNode<Child>::addChild()
{
    // create child
    auto newChild = std::make_unique<Child>();

    // make this node parent of new child
    newChild->setParent (this);

    // add child to array of children
    // this must be done last so child is prepared before called to process audio
    auto* createdChild = children.add (std::move (newChild));

    // tell listeners about new child
    nodeListeners.call (&BaseNode<Child>::Listener::nodeAdded, createdChild);

    return createdChild;
}

template<typename Child>
void BaseNode<Child>::removeChild (Child* childToRemove)
{
    // remove child from children array
    int indexToRemove = children.indexOf (childToRemove);

    // put child in `toBeDeleted` placeholder
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
    if (children.isEmpty())
        return new XmlElement ("no_children");

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
