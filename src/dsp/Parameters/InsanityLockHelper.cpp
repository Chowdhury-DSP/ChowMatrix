#include "InsanityLockHelper.h"

InsanityLockHelper::InsanityLockHelper (std::function<void()> onParamLockChange) : onParamLockChange (onParamLockChange)
{
}

void InsanityLockHelper::setInsanityLock (const String& paramID, bool shouldBeLocked, bool shouldBeReset)
{
    jassert (! (shouldBeLocked && shouldBeReset)); // can't have both!

    const auto isLocked = lockedParams.contains (paramID);
    const auto isReset = resetParams.contains (paramID);

    if (shouldBeLocked == isLocked && shouldBeReset == isReset) // no change!
        return;

    if (! shouldBeLocked && ! shouldBeReset) // unlock
    {
        lockedParams.removeString (paramID);
        resetParams.removeString (paramID);
    }
    else if (shouldBeLocked)
    {
        lockedParams.addIfNotAlreadyThere (paramID);
        resetParams.removeString (paramID);
    }
    else if (shouldBeReset)
    {
        resetParams.addIfNotAlreadyThere (paramID);
        lockedParams.removeString (paramID);
    }

    onParamLockChange();
}

void InsanityLockHelper::toggleInsanityLock (const String& paramID)
{
    const auto isLocked = lockedParams.contains (paramID);
    const auto isReset = resetParams.contains (paramID);

    if (isLocked)
    {
        lockedParams.removeString (paramID);
        resetParams.addIfNotAlreadyThere (paramID);
    }
    else if (isReset)
    {
        resetParams.removeString (paramID);
    }
    else
    {
        lockedParams.addIfNotAlreadyThere (paramID);
    }

    onParamLockChange();
}

void InsanityLockHelper::createPopupMenu (PopupMenu& parent, const String& paramID)
{
    bool isLocked = isParamLocked (paramID);
    bool isReset = shouldParamReset (paramID);

    const std::map<int, std::pair<String, bool>> settings {
        { 1, { "Unlock", ! (isLocked || isReset) } },
        { 2, { "Lock", isLocked } },
        { 3, { "Reset", isReset } }
    };

    PopupMenu insanityLockMenu;
    for (const auto& [idx, setting] : settings)
    {
        const auto& [name, isOn] = setting;
        PopupMenu::Item item (name);
        item.itemID = idx;
        item.setColour (Colour (isOn ? 0xFF21CCA5 : 0xFFFFFFFF));

        bool setLock = name == "Lock";
        bool setReset = name == "Reset";
        item.action = [=] { setInsanityLock (paramID, setLock, setReset); };

        insanityLockMenu.addItem (item);
    }

    parent.addSubMenu ("Insanity Lock:", insanityLockMenu);
}

void InsanityLockHelper::saveState (XmlElement* xml)
{
    xml->setAttribute ("locked", lockedParams.joinIntoString (",") + ",");
    xml->setAttribute ("reset", resetParams.joinIntoString (",") + ",");
}

void InsanityLockHelper::loadState (XmlElement* xml)
{
    ParamHelpers::loadStringArray (lockedParams, xml->getStringAttribute ("locked", String()));
    ParamHelpers::loadStringArray (resetParams, xml->getStringAttribute ("reset", String()));
}
