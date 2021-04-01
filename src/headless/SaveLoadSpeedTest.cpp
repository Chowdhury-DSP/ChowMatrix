#include "SaveLoadSpeedTest.h"
#include "../ChowMatrix.h"

SaveLoadSpeedTest::SaveLoadSpeedTest()
{
    this->commandOption = "--save-load-test";
    this->argumentDescription = "--save-load-test --num=NUM";
    this->shortDescription = "Runs speed test on saving/loading plugin state";
    this->longDescription = "";
    this->command = std::bind (&SaveLoadSpeedTest::runSaveLoadTests, this, std::placeholders::_1);
}

void SaveLoadSpeedTest::runSaveLoadTests (const ArgumentList& args)
{
    int numIters = 100;
    if (args.containsOption ("--num"))
        numIters = args.getValueForOption ("--num").getIntValue();

    std::cout << "Running Save/Load Speed Test with " << numIters << " iterations" << std::endl;
    saveTest (numIters);
    loadTest (numIters);
}

void SaveLoadSpeedTest::saveTest (int numIter)
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
}

void SaveLoadSpeedTest::loadTest (int numIter)
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
}
