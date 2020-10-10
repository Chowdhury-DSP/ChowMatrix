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

    Child* addChild();
    int getNumChildren() const noexcept { return children.size(); }
    Child* getChild (int idx) { return children[idx]; }
    BaseNode* getParent() {return parent; }

protected:
    NodeComponent* editor = nullptr;
    BaseNode* parent = nullptr;

private:
    OwnedArray<Child> children;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseNode)
};
