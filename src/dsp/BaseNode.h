#pragma once

#include <JuceHeader.h>

class NodeComponent;
class GraphView;

template<typename Child>
class BaseNode
{
public:
    BaseNode();
    virtual ~BaseNode() {}

    virtual std::unique_ptr<NodeComponent> createEditor (GraphView*) = 0;
    NodeComponent* getEditor();

    virtual void prepare (double sampleRate, int samplesPerBlock);
    virtual void process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples);

    Child* addChild();
    void removeChild (Child* childToRemove);
    void clearChildren();

    int getNumChildren() const noexcept { return children.size(); }
    Child* getChild (int idx) { return children[idx]; }
    BaseNode* getParent() {return parent; }
    void setParent (BaseNode* parent);

    struct Listener
    {
        virtual ~Listener() {}
        virtual void nodeAdded (Child* /*newNode*/) {}
        virtual void nodeRemoved (Child* /*nodeToRemove*/) {}
    };

    void addNodeListener (Listener* l) { nodeListeners.add (l); }
    void removeNodeListener (Listener* l) { nodeListeners.remove (l); }

    virtual XmlElement* saveXml();
    virtual void loadXml (XmlElement*);

protected:
    NodeComponent* editor = nullptr;
    BaseNode* parent = nullptr;

    OwnedArray<Child> children;
    ListenerList<Listener> nodeListeners;

private:
    std::unique_ptr<Child> nodeBeingDeleted;

    double sampleRate = 44100.0;
    int samplesPerBlock = 256;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseNode)
};
