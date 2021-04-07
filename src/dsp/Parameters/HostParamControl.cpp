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
        params.push_back (std::make_unique<AudioParameterFloat> (id, name, 0.0f, 1.0f, 0.0f));
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
            auto& controlMap = paramControlMaps[i];
            for (auto& map : controlMap)
                if (map.mappedParamID == paramID)
                    map.nodePtr->setNodeParameter (paramID, newValue);

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
            if (doForParamMap (node, paramID, i, [=] (MapIter) { parameterHandles[i]->beginChangeGesture(); }, [] {}))
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
            if (doForParamMap (node, paramID, i, [=] (MapIter) { parameterHandles[i]->endChangeGesture(); }, [] {}))
                break;
        }
    }
}

void HostParamControl::applyParameterChange (const String& paramID, DelayNode* node, float value01)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        doForParamMap (
            node, paramID, i, [=] (MapIter) { parameterHandles[i]->setValueNotifyingHost (value01); }, [] {});
    }
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
    // for (size_t i = 0; i < numParams; ++i)
    // {
    //     auto& groupMap = paramGroupMaps[i];
    //     auto mapIter = std::find (groupMap.begin(), groupMap.end(), paramID);
    //     auto isMapped = mapIter != groupMap.end();

    //     PopupMenu::Item paramItem (getParamName (i));
    //     paramItem.itemID = (int) i + 1;
    //     paramItem.action = [=] { toggleGroupParamMap (paramID, i); };
    //     paramItem.setColour (Colour (isMapped ? 0xFF21CCA5 : 0xFFFFFFFF));

    //     paramGroupMapMenu.addItem (paramItem);
    // }

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

HostParamControl::MapIter HostParamControl::findMap (DelayNode* node, const String& paramID, size_t mapIdx)
{
    auto& controlMap = paramControlMaps[mapIdx];
    for (size_t i = 0; i < controlMap.size(); ++i)
    {
        if (controlMap[i].nodePtr == node && controlMap[i].mappedParamID == paramID)
            return controlMap.begin() + (int) i;
    }

    return controlMap.end();
}

void HostParamControl::toggleParamMap (DelayNode* node, const String& paramID, size_t mapIdx)
{
    doForParamMap (
        node, paramID, mapIdx, [=] (MapIter iter) { paramControlMaps[mapIdx].erase (iter); }, [=] { 
            paramControlMaps[mapIdx].push_back ({ node, paramID });

            if (paramControlMaps[mapIdx].size() == 1) // this is the only parameter here
                parameterHandles[mapIdx]->setValueNotifyingHost (node->getNodeParameter (paramID)->getValue());
            else
                node->setNodeParameter (paramID, parameterHandles[mapIdx]->getValue());
        });
}

void HostParamControl::toggleGroupParamMap (const String& paramID, size_t mapIdx)
{
    auto& groupMap = paramGroupMaps[mapIdx];
    auto mapIter = std::find (groupMap.begin(), groupMap.end(), paramID);
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

    groupMap.push_back (paramID);
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
        // forEachChildElement is deprecated in some version of JUCE
        JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wdeprecated-declarations")
        forEachXmlChildElement (*hostParamControlState, paramMap)
        {
            if (paramMap->hasAttribute ("assigned_param") && paramMap->hasAttribute ("param_id"))
            {
                auto mapIdx = (size_t) paramMap->getIntAttribute ("assigned_param");
                auto paramID = paramMap->getStringAttribute ("param_id");
                paramControlMaps[mapIdx].push_back ({ node, paramID });
            }
        }
        JUCE_END_IGNORE_WARNINGS_GCC_LIKE
    }
}

void HostParamControl::loadParamList (StringArray& paramList, size_t mapIdx) const
{
    auto& controlMap = paramControlMaps[mapIdx];
    for (auto& map : controlMap)
    {
        auto nodeIdx = map.nodePtr->getIndex();
        auto paramName = map.nodePtr->getNodeParameter (map.mappedParamID)->name;
        paramList.addIfNotAlreadyThere ("Node " + String (nodeIdx + 1) + ": " + paramName);
    }
}
