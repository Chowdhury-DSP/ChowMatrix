#include "../../ChowMatrix.h"

class SaveLoadSpeedTest : public UnitTest
{
public:
    SaveLoadSpeedTest() : UnitTest ("Save/Load Speed Test")
    {
    }

    void runTest() override
    {
        int numIters = 100;

        beginTest ("Save Test");
        saveTest (numIters);

        beginTest ("Load Test");
        loadTest (numIters);
    }

    void saveTest (int numIter)
    {
        auto proc = createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone);
        std::unique_ptr<ChowMatrix> plugin (dynamic_cast<ChowMatrix*> (proc));
        plugin->setCurrentProgram (1);

        auto& stateManager = plugin->getStateManager();

        Time time;
        auto start = time.getMillisecondCounterHiRes();
        for (int i = 0; i < numIter; ++i)
            stateManager.saveState();

        auto duration = (time.getMillisecondCounterHiRes() - start) / 1000.0;
        std::cout << "Saved state " << numIter << " times in " << duration << " seconds" << std::endl;
        std::cout << "Average time " << duration / (double) numIter << std::endl;

        expectLessThan (duration, 0.1, "Saving state took too long!");
    }

    void loadTest (int numIter)
    {
        auto proc = createPluginFilterOfType (AudioProcessor::WrapperType::wrapperType_Standalone);
        std::unique_ptr<ChowMatrix> plugin (dynamic_cast<ChowMatrix*> (proc));
        auto& stateManager = plugin->getStateManager();

        plugin->setCurrentProgram (1);
        auto stateXml = stateManager.saveState();
        plugin->setCurrentProgram (0);

        Time time;
        auto start = time.getMillisecondCounterHiRes();
        for (int i = 0; i < numIter; ++i)
            stateManager.loadState (stateXml.get());

        auto duration = (time.getMillisecondCounterHiRes() - start) / 1000.0;
        std::cout << "Loaded state " << numIter << " times in " << duration << " seconds" << std::endl;
        std::cout << "Average time " << duration / (double) numIter << std::endl;

        expectLessThan (duration, 3.0, "Loading state took too long!");
    }
};

static SaveLoadSpeedTest saveLoadSpeedTest;
