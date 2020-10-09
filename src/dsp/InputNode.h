#pragma once

#include "BaseNode.h"
#include "../gui/DelayNodeComponent.h"

class InputNode : public BaseNode
{
public:
    InputNode() {}

    std::unique_ptr<Component> createEditor() override
    {
        auto editorPtr = std::make_unique<NodeComponent> (*this);
        editor = editorPtr.get();
        return std::move (editorPtr);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputNode)
};
