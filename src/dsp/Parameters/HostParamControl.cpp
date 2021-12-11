#include "HostParamControl.h"

namespace
{
static String getParamID (size_t idx) { return "assigned_" + String (idx); }
static String getParamName (size_t idx) { return "Assign " + String (idx + 1); }

} // namespace

HostParamControl::HostParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) : BaseController (vts, nodes, paramIDs)
{
    for (size_t i = 0; i < numParams; ++i)
        parameterHandles[i] = vts.getParameter (getParamID (i));
}

void HostParamControl::addParameters (Parameters& params)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        auto id = getParamID (i);
        auto name = getParamName (i);
        paramIDs.add (id);
        chowdsp::ParamUtils::emplace_param<AudioParameterFloat> (params, id, name, 0.0f, 1.0f, 0.0f);
    }
}

void HostParamControl::newNodeAdded (DelayNode* newNode)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        auto& groupMap = paramGroupMaps[i];
        for (auto& mappedParam : groupMap)
            newNode->setNodeParameter (mappedParam, parameterHandles[i]->getValue());
    }
}

void HostParamControl::newNodeRemoved (DelayNode* newNode)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        auto& controlMap = paramControlMaps[i];
        for (auto j = (int) controlMap.size() - 1; j >= 0; --j)
        {
            if (controlMap[(size_t) j].nodePtr == newNode)
                controlMap.erase (controlMap.begin() + j);
        }
    }
}

void HostParamControl::parameterChanged (const String& paramID, float newValue)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        if (paramID == getParamID (i))
        {
            auto& groupMap = paramGroupMaps[i];
            for (const auto& mappedParamID : groupMap)
                MessageManager::callAsync ([=] {
                    doForNodes ([=] (DelayNode* n) { n->setNodeParameter (mappedParamID, newValue); });
                });

            auto& controlMap = paramControlMaps[i];
            for (auto& map : controlMap)
                MessageManager::callAsync ([=] { map.nodePtr->setNodeParameter (map.mappedParamID, newValue); });

            return;
        }
    }
}

void HostParamControl::beginParameterChange (const StringArray& IDs, DelayNode* node)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        for (const auto& paramID : IDs)
        {
            if (doForBothMaps (
                    node, paramID, i, [=] { parameterHandles[i]->beginChangeGesture(); }, [] {}))
                break;
        }
    }
}

void HostParamControl::endParameterChange (const StringArray& IDs, DelayNode* node)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        for (const auto& paramID : IDs)
        {
            if (doForBothMaps (
                    node, paramID, i, [=] { parameterHandles[i]->endChangeGesture(); }, [] {}))
                break;
        }
    }
}

void HostParamControl::applyParameterChange (const String& paramID, DelayNode* node, float value01)
{
    for (size_t i = 0; i < numParams; ++i)
        doForBothMaps (
            node, paramID, i, [=] { parameterHandles[i]->setValueNotifyingHost (value01); }, [] {});
}

void HostParamControl::addParameterMenus (PopupMenu& parentMenu, const String& paramID, DelayNode* node)
{
    PopupMenu paramMapMenu;
    for (size_t i = 0; i < numParams; ++i)
    {
        auto& controlMap = paramControlMaps[i];
        auto isMapped = findMap (node, paramID, i) != controlMap.end();

        PopupMenu::Item paramItem (getParamName (i));
        paramItem.itemID = (int) i + 1;
        paramItem.action = [=] { toggleParamMap (node, paramID, i); };
        paramItem.setColour (Colour (isMapped ? 0xFF21CCA5 : 0xFFFFFFFF));

        paramMapMenu.addItem (paramItem);
    }

    PopupMenu paramGroupMapMenu;
    for (size_t i = 0; i < numParams; ++i)
    {
        auto& groupMap = paramGroupMaps[i];
        auto mapIter = findMap (paramID, i);
        auto isMapped = mapIter != groupMap.end();

        PopupMenu::Item paramItem (getParamName (i));
        paramItem.itemID = (int) i + 1;
        paramItem.action = [=] { toggleGroupParamMap (node, paramID, i); };
        paramItem.setColour (Colour (isMapped ? 0xFF21CCA5 : 0xFFFFFFFF));

        paramGroupMapMenu.addItem (paramItem);
    }

    parentMenu.addSubMenu ("Assign Parameter:", paramMapMenu);
    parentMenu.addSubMenu ("Assign Global:", paramGroupMapMenu);
}

bool HostParamControl::doForParamMap (DelayNode* node, const String& paramID, size_t mapIdx, std::function<void (MapIter)> found, std::function<void()> notFound)
{
    auto& controlMap = paramControlMaps[mapIdx];
    auto paramMapIter = findMap (node, paramID, mapIdx);

    if (paramMapIter != controlMap.end())
    {
        found (paramMapIter);
        return true;
    }

    notFound();
    return false;
}

bool HostParamControl::doForGroupMap (const String& paramID, size_t mapIdx, std::function<void (GlobalIter)> found, std::function<void()> notFound)
{
    auto& groupMap = paramGroupMaps[mapIdx];
    auto mapIter = findMap (paramID, mapIdx);

    if (mapIter != groupMap.end())
    {
        found (mapIter);
        return true;
    }

    notFound();
    return false;
}

bool HostParamControl::doForBothMaps (DelayNode* node, const String& paramID, size_t mapIdx, std::function<void()> found, std::function<void()> notFound)
{
    if (doForGroupMap (
            paramID, mapIdx, [=] (GlobalIter) { found(); }, [=] { notFound(); }))
        return true;

    if (doForParamMap (
            node, paramID, mapIdx, [=] (MapIter) { found(); }, [=] { notFound(); }))
        return true;

    return false;
}

HostParamControl::MapIter HostParamControl::findMap (DelayNode* node, const String& paramID, size_t mapIdx) const
{
    const auto& controlMap = paramControlMaps[mapIdx];
    for (size_t i = 0; i < controlMap.size(); ++i)
    {
        if (controlMap[i].nodePtr == node && controlMap[i].mappedParamID == paramID)
            return controlMap.cbegin() + (int) i;
    }

    return controlMap.end();
}

HostParamControl::GlobalIter HostParamControl::findMap (const String& paramID, size_t mapIdx) const
{
    const auto& groupMap = paramGroupMaps[mapIdx];
    return std::find (groupMap.begin(), groupMap.end(), paramID);
}

bool HostParamControl::isParamMapped (size_t mapIdx) const noexcept
{
    return ! paramControlMaps[mapIdx].empty() || ! paramGroupMaps[mapIdx].empty();
}

void HostParamControl::toggleParamMap (DelayNode* node, const String& paramID, size_t mapIdx)
{
    doForParamMap (
        node, paramID, mapIdx, [=] (MapIter iter) { paramControlMaps[mapIdx].erase (iter); }, [=] {
            auto isMapped = isParamMapped (mapIdx);

            paramControlMaps[mapIdx].push_back ({ node, paramID });

            if (! isMapped) // this is the only parameter here
                parameterHandles[mapIdx]->setValueNotifyingHost (node->getNodeParameter (paramID)->getValue());
            else
                node->setNodeParameter (paramID, parameterHandles[mapIdx]->getValue()); });
}

void HostParamControl::toggleGroupParamMap (DelayNode* node, const String& paramID, size_t mapIdx)
{
    auto& groupMap = paramGroupMaps[mapIdx];
    auto mapIter = findMap (paramID, mapIdx);
    if (mapIter != groupMap.end()) // parameter is currently mapped
    {
        groupMap.erase (mapIter);
        return;
    }

    // we need to add it to the map
    // first remove it from existing node maps
    for (size_t idx = 0; idx < numParams; ++idx)
    {
        auto& controlMap = paramControlMaps[idx];
        for (int i = (int) controlMap.size() - 1; i >= 0; --i)
        {
            if (controlMap[(size_t) i].mappedParamID == paramID) // remove this node
                controlMap.erase (controlMap.begin() + i);
        }
    }

    auto isMapped = isParamMapped (mapIdx);
    groupMap.push_back (paramID);

    if (! isMapped) // this is the only parameter here
    {
        auto nodeParam = node->getNodeParameter (paramID);
        auto value = nodeParam->getValue();
        parameterHandles[mapIdx]->setValueNotifyingHost (value);
        doForNodes ([=] (DelayNode* n) { n->setNodeParameter (paramID, value); });
    }
    else
    {
        // sync other parameters to this node
        doForNodes ([=] (DelayNode* n) { n->setNodeParameter (paramID, parameterHandles[mapIdx]->getValue()); });
    }
}

void HostParamControl::saveExtraNodeState (XmlElement* nodeState, DelayNode* node)
{
    auto hostParamControlState = std::make_unique<XmlElement> ("host_controls");

    for (size_t i = 0; i < numParams; ++i)
    {
        for (int j = 0; j < node->getNumParams(); ++j)
        {
            auto paramID = node->getParamID (j);
            doForParamMap (
                node, paramID, i, [=, &hostParamControlState] (MapIter) {
                auto paramMap = std::make_unique<XmlElement> ("Map_" + paramID + "_assign" + String (i));
                paramMap->setAttribute ("assigned_param", (int) i);
                paramMap->setAttribute ("param_id", paramID);
                hostParamControlState->addChildElement (paramMap.release()); }, [] {});
        }
    }

    nodeState->addChildElement (hostParamControlState.release());
}

void HostParamControl::loadExtraNodeState (XmlElement* nodeState, DelayNode* node)
{
    if (auto* hostParamControlState = nodeState->getChildByName ("host_controls"))
    {
        for (auto* paramMap : hostParamControlState->getChildIterator())
        {
            if (paramMap->hasAttribute ("assigned_param") && paramMap->hasAttribute ("param_id"))
            {
                auto mapIdx = (size_t) paramMap->getIntAttribute ("assigned_param");
                auto paramID = paramMap->getStringAttribute ("param_id");
                paramControlMaps[mapIdx].push_back ({ node, paramID });
            }
        }
    }
}

void HostParamControl::saveGlobalMap (XmlElement* mapXml)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        auto& groupMap = paramGroupMaps[i];
        for (auto& mappedParam : groupMap)
        {
            auto paramMap = std::make_unique<XmlElement> ("Map_" + mappedParam + "_assign" + String (i));
            paramMap->setAttribute ("assigned_param", (int) i);
            paramMap->setAttribute ("param_id", mappedParam);
            mapXml->addChildElement (paramMap.release());
        }
    }
}

void HostParamControl::loadGlobalMap (XmlElement* mapXml)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        auto& groupMap = paramGroupMaps[i];
        groupMap.clear();
    }

    for (auto* paramMap : mapXml->getChildIterator())
    {
        if (paramMap->hasAttribute ("assigned_param") && paramMap->hasAttribute ("param_id"))
        {
            auto mapIdx = (size_t) paramMap->getIntAttribute ("assigned_param");
            auto paramID = paramMap->getStringAttribute ("param_id");
            paramGroupMaps[mapIdx].push_back (paramID);
        }
    }
}

void HostParamControl::loadParamList (StringArray& paramList, std::vector<std::function<void()>>& xCallbacks, size_t mapIdx)
{
    auto& groupMap = paramGroupMaps[mapIdx];
    for (auto& mappedParam : groupMap)
    {
        auto paramName = ParamHelpers::getName (mappedParam);
        paramList.addIfNotAlreadyThere ("Global: " + paramName);
        xCallbacks.push_back ([=, &groupMap] {
            auto mapIter = findMap (mappedParam, mapIdx);
            groupMap.erase (mapIter);
        });
    }

    auto& controlMap = paramControlMaps[mapIdx];
    for (auto& map : controlMap)
    {
        auto nodeIdx = map.nodePtr->getIndex();
        auto paramName = ParamHelpers::getName (map.mappedParamID);
        paramList.addIfNotAlreadyThere ("Node " + String (nodeIdx + 1) + ": " + paramName);
        xCallbacks.push_back ([=, &controlMap] {
            auto mapIter = findMap (map.nodePtr, map.mappedParamID, mapIdx);
            controlMap.erase (mapIter);
        });
    }
}
