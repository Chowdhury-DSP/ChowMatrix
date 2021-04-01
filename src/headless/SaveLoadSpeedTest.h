#pragma once

#include "../pch.h"

class SaveLoadSpeedTest : public ConsoleApplication::Command
{
public:
    SaveLoadSpeedTest();

private:
    void runSaveLoadTests (const ArgumentList& args);
    void saveTest (int numIter);
    void loadTest (int numIter);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SaveLoadSpeedTest)
};
