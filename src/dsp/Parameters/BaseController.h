#pragma once

#include "../InputNode.h"

/**
 * Base class for some parameter controller that needs
 * access to all active nodes
 */
class BaseController : private AudioProcessorValueTreeState::Listener,
                       private DBaseNode::Listener
{
public:
    using Parameters = std::vector<std::unique_ptr<juce::RangedAudioParameter>>;

    BaseController (AudioProcessorValueTreeState& vts,
                    std::array<InputNode, 2>* nodes,
                    StringArray paramsToListenFor);
    ~BaseController() override;

protected:
    virtual void newNodeAdded (DelayNode*) {}
    virtual void newNodeRemoved (DelayNode*) {}
    virtual void nodeIndexHasChanged (DelayNode* /*node*/, int /*oldIndex*/, int /*newIndex*/) {}
    void doForNodes (std::function<void (DelayNode*)> nodeFunc);

private:
    void nodeAdded (DelayNode* newNode) override;
    void nodeRemoved (DelayNode* nodeToRemove) override;
    void nodeIndexChanged (DelayNode* node, int oldIndex, int newIndex) override;

    AudioProcessorValueTreeState& vts;
    std::array<InputNode, 2>* nodes;
    StringArray paramsToListenFor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseController)
};
