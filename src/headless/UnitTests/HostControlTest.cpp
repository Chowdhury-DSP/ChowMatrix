#include "../../ChowMatrix.h"

class HostControlTest : public UnitTest
{
public:
    HostControlTest() : UnitTest ("Host Control Test")
    {
    }

    void runTest() override
    {
        auto proc = createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone);
        std::unique_ptr<ChowMatrix> plugin (dynamic_cast<ChowMatrix*> (proc));
        auto& paramControl = plugin->getHostControl();

        beginTest ("Param Assign Test");
        paramAssignTest (plugin.get(), paramControl);
    }

    AudioProcessorParameter* getAssignParameter (ChowMatrix* plugin, size_t assignIdx)
    {
        auto params = plugin->getParameters();
        for (auto param : params)
        {
            if (param->getName (1024) == "Assign " + String (assignIdx + 1))
                return param;
        }

        return nullptr;
    }

    void paramAssignTest (ChowMatrix* plugin, HostParamControl& paramControl)
    {
        constexpr auto paramIdx = 0;
        constexpr float value1 = 0.8f;
        constexpr float value2 = 0.2f;

        auto* node = plugin->getNodes()->front().getChild (0);
        auto* assignParam = getAssignParameter (plugin, paramIdx);

        paramControl.toggleParamMap (node, ParamTags::delayTag, paramIdx);
        paramControl.toggleParamMap (node, ParamTags::fbTag, paramIdx);

        assignParam->setValueNotifyingHost (value1);
        MessageManager::getInstance()->runDispatchLoopUntil(50); // wait for message thread to catch up...
        expectWithinAbsoluteError (node->getNodeParameter (ParamTags::delayTag)->getValue(), value1, 1.0e-5f);
        expectWithinAbsoluteError (node->getNodeParameter (ParamTags::fbTag)->getValue(), value1, 1.0e-5f);

        paramControl.toggleParamMap (node, ParamTags::delayTag, paramIdx);
        paramControl.toggleParamMap (node, ParamTags::fbTag, paramIdx);

        assignParam->setValueNotifyingHost (value2);
        MessageManager::getInstance()->runDispatchLoopUntil(50);
        expectWithinAbsoluteError (node->getNodeParameter (ParamTags::delayTag)->getValue(), value1, 1.0e-5f);
        expectWithinAbsoluteError (node->getNodeParameter (ParamTags::fbTag)->getValue(), value1, 1.0e-5f);
    }
};

static HostControlTest hostControlTest;
