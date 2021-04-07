#pragma once

#include "BaseController.h"

class HostParamControl : public BaseController
{
public:
    HostParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    static void addParameters (Parameters& params);
    void newNodeAdded (DelayNode* newNode) override;
    void newNodeRemoved (DelayNode* newNode) override;

    void parameterChanged (const String&, float newValue) override;

    void addParameterMenus (PopupMenu& parentMenu, const String& paramID, DelayNode* node) override;

    void beginParameterChange (const StringArray& paramIDs, DelayNode* node) override;
    void endParameterChange (const StringArray& paramIDs, DelayNode* node) override;
    void applyParameterChange (const String& paramID, DelayNode* node, float value01) override;

    void saveExtraNodeState (XmlElement* nodeState, DelayNode* node) override;
    void loadExtraNodeState (XmlElement* nodeState, DelayNode* node) override;

    void saveGlobalMap (XmlElement* mapXml);
    void loadGlobalMap (XmlElement* mapXml);

    void loadParamList (StringArray& paramList, size_t mapIdx) const;
    constexpr size_t getNumAssignableParams() const noexcept { return numParams; }

private:
    struct MapInfo
    {
        DelayNode* nodePtr = nullptr;
        String mappedParamID;
    };

    using MapIter = std::vector<MapInfo>::const_iterator;
    using GlobalIter = std::vector<String>::const_iterator;

    bool doForParamMap (DelayNode* node, const String& paramID, size_t mapIdx, std::function<void (MapIter)> found, std::function<void()> notFound);
    MapIter findMap (DelayNode* node, const String& paramID, size_t mapIdx) const;
    void toggleParamMap (DelayNode* node, const String& paramID, size_t mapIdx);

    bool doForGroupMap (const String& paramID, size_t mapIdx, std::function<void (GlobalIter)> found, std::function<void()> notFound);
    GlobalIter findMap (const String& paramID, size_t mapIdx) const;
    void toggleGroupParamMap (DelayNode* node, const String& paramID, size_t mapIdx);

    bool isParamMapped (size_t mapIdx) const noexcept;
    bool doForBothMaps (DelayNode* node, const String& paramID, size_t mapIdx, std::function<void()> found, std::function<void()> notFound);

    static constexpr size_t numParams = 16;
    static inline StringArray paramIDs;

    std::array<std::vector<String>, numParams> paramGroupMaps;
    std::array<std::vector<MapInfo>, numParams> paramControlMaps;
    std::array<RangedAudioParameter*, numParams> parameterHandles;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostParamControl)
};
