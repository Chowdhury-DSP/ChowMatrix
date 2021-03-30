#include "BaseController.h"

class HostParamControl : public BaseController
{
public:
    HostParamControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes);

    static void addParameters (Parameters& params);
    void newNodeRemoved (DelayNode* newNode) override;

    void parameterChanged (const String&, float newValue) override;

private:
    static constexpr size_t numParams = 16;
    static inline StringArray paramIDs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostParamControl)
};
