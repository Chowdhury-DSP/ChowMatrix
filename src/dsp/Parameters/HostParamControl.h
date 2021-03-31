#include "BaseController.h"

class HostParamControl : public BaseController
{
public:
    HostParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    static void addParameters (Parameters& params);
    void newNodeRemoved (DelayNode* newNode) override;

    void parameterChanged (const String&, float newValue) override;

    void addParameterMenus (PopupMenu& parentMenu, const String& paramID, DelayNode* node) override;

    void beginParameterChange (const String& paramID, DelayNode* node) override;
    void endParameterChange (const String& paramID, DelayNode* node) override;
    void applyParameterChange (const String& paramID, DelayNode* node, float value01) override;

    void saveExtraNodeState (XmlElement* nodeState, DelayNode* node) override;
    void loadExtraNodeState (XmlElement* nodeState, DelayNode* node) override;

private:
    struct MapInfo
    {
        DelayNode* nodePtr = nullptr;
        String mappedParamID;
    };

    using MapIter = std::vector<MapInfo>::iterator;

    void doForParamMap (DelayNode* node, const String& paramID, size_t mapIdx, std::function<void (MapIter)> found, std::function<void()> notFound);
    MapIter findMap (DelayNode* node, const String& paramID, size_t mapIdx);
    void toggleParamMap (DelayNode* node, const String& paramID, size_t mapIdx);

    static constexpr size_t numParams = 16;
    static inline StringArray paramIDs;

    std::array<std::vector<MapInfo>, numParams> paramControlMaps;
    std::array<RangedAudioParameter*, numParams> parameterHandles;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostParamControl)
};
