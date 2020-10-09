#pragma once

#include <JuceHeader.h>

class BaseNode
{
public:
    BaseNode();
    virtual ~BaseNode() {}

    virtual std::unique_ptr<Component> createEditor() = 0;
    Component* getEditor();

    BaseNode* addChild();
    int getNumChildren() const noexcept { return children.size(); }
    BaseNode* getChild (int idx) {return children[idx]; }
    BaseNode* getParent() {return parent; }

protected:
    Component* editor = nullptr;
    BaseNode* parent = nullptr;

private:
    OwnedArray<BaseNode> children;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseNode)
};
