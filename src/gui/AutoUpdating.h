#pragma once

#if CHOWDSP_AUTO_UPDATE

#include <pch.h>

struct UpdateButtonLNF;
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

    String newVersion = String (JucePlugin_VersionString);

    TextButton yesButton { "Yes" };
    TextButton noButton { "No" };
    std::unique_ptr<UpdateButtonLNF> ubLNF;

    std::future<bool> needsUpdate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoUpdater)
};

#endif // CHOWDSP_AUTO_UPDATE
