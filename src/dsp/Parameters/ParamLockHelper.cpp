#include "ParamLockHelper.h"

ParamLockHelper::ParamLockHelper (std::function<void()> onParamLockChange) : onParamLockChange (onParamLockChange)
{
}

void ParamLockHelper::setInsanityLock (const String& paramID, bool shouldBeLocked, bool shouldBeReset)
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

void ParamLockHelper::toggleInsanityLock (const String& paramID)
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

void ParamLockHelper::createPopupMenu (PopupMenu& parent, const String& paramID)
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

    parent.addSubMenu ("Insanity Lock", insanityLockMenu);
}

static void loadStringArray (StringArray& array, String string)
{
    array.clear();
    while (string.isNotEmpty())
    {
        auto splitIdx = string.indexOfChar (',');
        if (splitIdx <= 0)
            break;

        array.add (string.substring (0, splitIdx));
        string = string.substring (splitIdx + 1);
    }
}

void ParamLockHelper::saveState (XmlElement* xml)
{
    xml->setAttribute ("locked", lockedParams.joinIntoString (",") + ",");
    xml->setAttribute ("reset", resetParams.joinIntoString (",") + ",");
}

void ParamLockHelper::loadState (XmlElement* xml)
{
    loadStringArray (lockedParams, xml->getStringAttribute ("locked", String()));
    loadStringArray (resetParams, xml->getStringAttribute ("reset", String()));
}
