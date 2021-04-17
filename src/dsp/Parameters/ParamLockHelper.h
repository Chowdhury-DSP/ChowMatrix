#pragma once

#include "ParamHelpers.h"

/**
 * Helper class to manage parameter insanity
 * lock and reset functionality
 */
class ParamLockHelper
{
public:
    ParamLockHelper (std::function<void()> onParamLockChange);

    void toggleInsanityLock (const String& paramID);
    void setInsanityLock (const String& paramID, bool shouldBeLocked, bool shouldBeReset);

    bool isParamLocked (const String& paramID) const noexcept { return lockedParams.contains (paramID); }
    bool shouldParamReset (const String& paramID) const noexcept { return resetParams.contains (paramID); }

    void createPopupMenu (PopupMenu& parent, const String& paramID);

    void saveState (XmlElement* xml);
    void loadState (XmlElement* xml);

private:
    StringArray lockedParams;
    StringArray resetParams;

    std::function<void()> onParamLockChange;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamLockHelper)
};
