#include "HostParamControl.h"

namespace
{

static String getParamID (size_t idx) { return "assigned_" + String (idx); }
static String getParamName (size_t idx) { return "Assign " + String (idx + 1); }

} // namespace

// EDGE CASES TO WATCH FOR:
// - Nodes getting re-ordered
// - Saving/loading

HostParamControl::HostParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) :
    BaseController (vts, nodes, paramIDs)
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
                map.nodePtr->setNodeParameter (map.mappedParamID, newValue);

            return;
        }
    }
}

void HostParamControl::beginParameterChange (const String& paramID, DelayNode* node)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        doForParamMap (node, paramID, i,
            [=] (MapIter) { parameterHandles[i]->beginChangeGesture(); }, [] {});
    }
}

void HostParamControl::endParameterChange (const String& paramID, DelayNode* node)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        doForParamMap (node, paramID, i,
            [=] (MapIter) { parameterHandles[i]->endChangeGesture(); }, [] {});
    }
}

void HostParamControl::applyParameterChange (const String& paramID, DelayNode* node, float value01)
{
    for (size_t i = 0; i < numParams; ++i)
    {
        doForParamMap (node, paramID, i,
            [=] (MapIter) { parameterHandles[i]->setValueNotifyingHost (value01); }, [] {});
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

    parentMenu.addSubMenu ("Assign Parameter:", paramMapMenu);
}

void HostParamControl::doForParamMap (DelayNode* node, const String& paramID, size_t mapIdx,
                                      std::function<void(MapIter)> found, std::function<void()> notFound)
{
    auto& controlMap = paramControlMaps[mapIdx];
    auto paramMapIter = findMap (node, paramID, mapIdx);

    if (paramMapIter != controlMap.end())
    {
        found (paramMapIter);
        return;
    }

    notFound();
}

HostParamControl::MapIter HostParamControl::findMap (DelayNode* node, const String& paramID, size_t mapIdx)
{
    auto& controlMap = paramControlMaps[mapIdx];
    for (size_t i = 0; i < controlMap.size(); ++i)
    {
        if (controlMap[i].nodePtr == node && controlMap[i].mappedParamID == paramID) // remove this node
            return controlMap.begin() + (int) i;
    }

    return controlMap.end();
}

void HostParamControl::toggleParamMap (DelayNode* node, const String& paramID, size_t mapIdx)
{
    doForParamMap (node, paramID, mapIdx, [=] (MapIter iter) { paramControlMaps[mapIdx].erase (iter); },
        [=] { 
            paramControlMaps[mapIdx].push_back ({ node, paramID });
            parameterHandles[mapIdx]->setValueNotifyingHost (node->getNodeParameter (paramID)->getValue());
        });
}
