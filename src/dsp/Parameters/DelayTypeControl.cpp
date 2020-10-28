#include "DelayTypeControl.h"
#include "../../NodeManager.h"

namespace
{
    const String delayTypeTag = "delay_type";
}

inline VariableDelay::DelayType getDelayType (float param)
{
    return static_cast<VariableDelay::DelayType> (int (param));
}

DelayTypeControl::DelayTypeControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) :
    vts (vts),
    nodes (nodes)
{
    delayTypeParam = vts.getRawParameterValue (delayTypeTag);
    vts.addParameterListener (delayTypeTag, this);
    
    for (auto& node : *nodes)
    {
        node.addNodeListener (this);
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { n->addNodeListener (this); });
    }

    parameterChanged (delayTypeTag, delayTypeParam->load());
}

DelayTypeControl::~DelayTypeControl()
{
    for (auto& node : *nodes)
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { n->removeNodeListener (this); });

    vts.removeParameterListener (delayTypeTag, this);
}

void DelayTypeControl::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterChoice> (delayTypeTag,
        "Delay Type", StringArray ({ "Glitch", "Rough", "Smooth", "Ultra Smooth" }), 2));
}

void DelayTypeControl::parameterChanged (const String&, float newValue)
{
    auto type = getDelayType (newValue);
    for (auto& node : *nodes)
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { n->setDelayType (type); });
}

void DelayTypeControl::nodeAdded (DelayNode* newNode)
{
    newNode->addNodeListener (this);
    newNode->setDelayType (getDelayType (delayTypeParam->load()));
}

void DelayTypeControl::nodeRemoved (DelayNode* nodeToRemove)
{
    nodeToRemove->removeNodeListener (this);
}
