#include "DelayTypeControl.h"
#include "../../NodeManager.h"

namespace
{
const String delayTypeTag = "delay_type";
}

static inline VariableDelay::DelayType getDelayType (float param)
{
    return static_cast<VariableDelay::DelayType> (int (param));
}

DelayTypeControl::DelayTypeControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) : BaseController (vts, nodes, { delayTypeTag })
{
    delayTypeParam = vts.getRawParameterValue (delayTypeTag);
    parameterChanged (delayTypeTag, delayTypeParam->load());
}

void DelayTypeControl::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterChoice> (delayTypeTag,
                                                              "Delay Type",
                                                              StringArray ({ "Glitch", "Rough", "Smooth", "Ultra Smooth", "Liquid", "Super Liquid" }),
                                                              2));
}

void DelayTypeControl::parameterChanged (const String&, float newValue)
{
    auto type = getDelayType (newValue);
    doForNodes ([=] (DelayNode* n) { n->setDelayType (type); });
}

void DelayTypeControl::newNodeAdded (DelayNode* newNode)
{
    newNode->setDelayType (getDelayType (delayTypeParam->load()));
}
