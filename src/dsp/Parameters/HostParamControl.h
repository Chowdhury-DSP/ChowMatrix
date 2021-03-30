#include "BaseController.h"

class HostParamControl : public BaseController
{
public:
    HostParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    static void addParameters (Parameters& params);
    void newNodeRemoved (DelayNode* newNode) override;

    void parameterChanged (const String&, float newValue) override;

    void addParameterMenus (PopupMenu& parentMenu, const String& paramID, DelayNode* node) override;

private:
    struct MapInfo
    {
        DelayNode* nodePtr = nullptr;
        String mappedParamID;
    };

    std::vector<MapInfo>::iterator findMap (DelayNode* node, const String& paramID, size_t mapIdx);
    void toggleParamMap (DelayNode* node, const String& paramID, size_t mapIdx);

    static constexpr size_t numParams = 16;
    static inline StringArray paramIDs;

    std::array<std::vector<MapInfo>, numParams> paramControlMaps;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostParamControl)
};
