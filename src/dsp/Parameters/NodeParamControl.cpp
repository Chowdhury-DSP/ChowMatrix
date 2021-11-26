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

template <typename FuncType>
void doForNodeParamIndexes (int nodeIndex, FuncType&& func)
{
    for (int paramIndex = 0; paramIndex < ParamHelpers::numParams; ++paramIndex)
    {
        int forwardIndex = nodeIndex * ParamHelpers::numParams + paramIndex;
        func (forwardIndex);
    }
}
} // namespace

NodeParamControl::NodeParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes, std::unique_ptr<chowdsp::PresetManager>& presetMgr)
    : BaseController (vts, nodes, StringArray()), presetManager (presetMgr)
{
    doForParams ([&] (int nodeNum, int paramNum)
                 {
                     auto id = getForwardParamID (nodeNum, paramNum);
                     auto forwardedParam = std::make_unique<chowdsp::ForwardingParameter> (id, nullptr, "Blank");

                     forwardedParam->setProcessor (&vts.processor);
                     forwardedParams.add (forwardedParam.get());
                     forwardedParam->addListener (this);
                     vts.processor.addParameter (forwardedParam.release());
                 });
}

void NodeParamControl::newNodeAdded (DelayNode* newNode)
{
    doForNodeParams (newNode, [=] (RangedAudioParameter* param, int nodeIndex, int forwardIndex)
                     { forwardedParams[forwardIndex]->setParam (param, "Node " + String (nodeIndex + 1) + ": " + param->getName (1024)); });

    setCurrentPresetDirty();
}

void NodeParamControl::newNodeRemoved (DelayNode* node)
{
    doForNodeParamIndexes (node->getIndex(), [=] (int forwardIndex)
                           { forwardedParams[forwardIndex]->setParam (nullptr); });

    setCurrentPresetDirty();
}

void NodeParamControl::nodeIndexHasChanged (DelayNode* node, int oldIndex, int /*newIndex*/)
{
    doForNodeParamIndexes (oldIndex, [=] (int forwardIndex)
                           { forwardedParams[forwardIndex]->setParam (nullptr); });

    newNodeAdded (node);
}

void NodeParamControl::parameterValueChanged (int /*paramIndex*/, float /*newValue*/)
{
    setCurrentPresetDirty();
}

void NodeParamControl::setCurrentPresetDirty()
{
    if (presetManager == nullptr)
        return;

    return presetManager->setIsDirty (true);
}
