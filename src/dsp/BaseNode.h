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
    int getNumChildren() const noexcept { return children.size(); }
    Child* getChild (int idx) { return children[idx]; }
    BaseNode* getParent() {return parent; }
    void setParent (BaseNode* parent);

protected:
    NodeComponent* editor = nullptr;
    BaseNode* parent = nullptr;

    OwnedArray<Child> children;

private:
    double sampleRate = 44100.0;
    int samplesPerBlock = 256;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseNode)
};
