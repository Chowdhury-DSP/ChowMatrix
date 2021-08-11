#pragma once

#include "ParamHelpers.h"

/**
 * Helper class to manage parameter randomise
 * lock functionality
 */
class RandomiseLockHelper
{
public:
    RandomiseLockHelper();

    void toggleRandomiseLock (const String& paramID);
    bool isParamLocked (const String& paramID) const noexcept { return lockedParams.contains (paramID); }

    void createPopupMenu (PopupMenu& parent, const String& paramID);

    void saveState (XmlElement* xml);
    void loadState (XmlElement* xml);

private:
    StringArray lockedParams;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RandomiseLockHelper)
};
