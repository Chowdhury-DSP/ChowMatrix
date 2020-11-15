#pragma once

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

    String newVersion = String (JucePlugin_VersionString);

    TextButton yesButton { "Yes" };
    TextButton noButton { "No" };
    std::unique_ptr<UpdateButtonLNF> ubLNF;

    std::future<bool> needsUpdate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoUpdater)
};
