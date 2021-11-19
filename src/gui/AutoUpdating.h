#pragma once

#if CHOWDSP_AUTO_UPDATE

#include <pch.h>

class AutoUpdater : public Component
{
public:
    AutoUpdater();
    ~AutoUpdater();

    void paint (Graphics& g) override;
    void resized() override;

    void showUpdaterScreen (Component* parent);

    bool runAutoUpdateCheck();
    void noButtonPressed();
    void yesButtonPressed();

private:
    File getUpdateCheckFile();
    String getLatestVersion();
    String getUpdateFileVersion (const File& updateFile);
    bool getUpdateFileYesNo (const File& updateFile);
    void editUpdateCheckFile (String version, bool wantsUpdate);
    void parentSizeChanged() override;

    chowdsp::VersionUtils::Version newVersion { JucePlugin_VersionString };

    TextButton yesButton { "Yes" };
    TextButton noButton { "No" };

    std::future<bool> needsUpdate;

    chowdsp::SharedLNFAllocator lnfAllocator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoUpdater)
};

#endif // CHOWDSP_AUTO_UPDATE
