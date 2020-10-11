#include "DelayNode.h"
#include "../gui/DelayNodeComponent.h"

using namespace DelayConsts;

DelayNode::DelayNode()
{
    NormalisableRange<float> delayRange { 0.0f, maxDelay };
    delayMs = params.add (std::make_unique<AudioParameterFloat> ("DLY", "Delay", delayRange, 50.0f));

    NormalisableRange<float> panRange { -1.0f, 1.0f };
    pan = params.add (std::make_unique<AudioParameterFloat> ("PAN", "Pan", panRange, 0.0f));
}

std::unique_ptr<NodeComponent> DelayNode::createEditor (GraphView* view)
{
    auto editorPtr = std::make_unique<DelayNodeComponent> (*this, view);
    editor = editorPtr.get();
    return std::move (editorPtr);
}
