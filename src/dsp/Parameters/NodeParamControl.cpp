#include "NodeParamControl.h"

namespace
{
constexpr int maxNumNodes = 50;

String getForwardParamID (int nodeNum, int paramNum)
{
    return "node_" + String (nodeNum) + "_" + ParamHelpers::getParamID (paramNum);
}

template <typename FuncType>
void doForParams (FuncType&& func)
{
    for (int nodeNum = 0; nodeNum < maxNumNodes; ++nodeNum)
        for (int paramNum = 0; paramNum < ParamHelpers::numParams; ++paramNum)
            func (nodeNum, paramNum);
}

template <typename FuncType>
void doForNodeParams (DelayNode* node, FuncType&& func)
{
    const auto nodeIndex = node->getIndex();
    int paramIndex = 0;
    for (auto* param : node->getParameters())
    {
        auto* paramCast = dynamic_cast<RangedAudioParameter*> (param);
        jassert (paramCast != nullptr);

        int forwardIndex = nodeIndex * ParamHelpers::numParams + (paramIndex++);
        func (paramCast, nodeIndex, forwardIndex);
    }
}
}

NodeParamControl::NodeParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) : BaseController (vts, nodes, StringArray())
{
    doForParams ([&] (int nodeNum, int paramNum) {
                     auto id = getForwardParamID (nodeNum, paramNum);
                     auto forwardedParam = std::make_unique<chowdsp::ForwardingParameter> (id, "Blank");

                     forwardedParam->setProcessor (&vts.processor);
                     forwardedParams.add(forwardedParam.get());
                     vts.processor.addParameter (forwardedParam.release());
                 });
}

void NodeParamControl::newNodeAdded (DelayNode* newNode)
{
    doForNodeParams(newNode, [=] (RangedAudioParameter* param, int nodeIndex, int forwardIndex) {
        forwardedParams[forwardIndex]->setParam (param, "Node " + String (nodeIndex + 1) + ": " + param->getName (1024));
    });
}

void NodeParamControl::newNodeRemoved (DelayNode* newNode)
{
    doForNodeParams(newNode, [=] (RangedAudioParameter* /*param*/, int /*nodeIndex*/, int forwardIndex) {
        forwardedParams[forwardIndex]->setParam (nullptr);
    });
}
