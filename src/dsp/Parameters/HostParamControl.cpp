#include "HostParamControl.h"

namespace
{

static String getParamID (size_t idx) { return "assigned_" + String (idx); }
static String getParamName (size_t idx) { return "Assigned " + String (idx + 1); }

} // namespace

HostParamControl::HostParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) :
    BaseController (vts, nodes, paramIDs)
{
    
}

void HostParamControl::addParameters (Parameters& params)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        auto id = getParamID (i);
        auto name = getParamName (i);
        paramIDs.add (id);
        params.push_back (std::make_unique<AudioParameterFloat> (id, name, 0.0f, 1.0f, 0.0f));
    }
}

void HostParamControl::newNodeRemoved (DelayNode* newNode)
{

}

void HostParamControl::parameterChanged (const String& paramID, float newValue)
{

}
