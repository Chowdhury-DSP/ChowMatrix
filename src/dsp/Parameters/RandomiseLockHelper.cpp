#include "RandomiseLockHelper.h"

RandomiseLockHelper::RandomiseLockHelper()
{
    lockedParams.add (ParamTags::pitchTag);
    lockedParams.add (ParamTags::modFreqTag);
}

void RandomiseLockHelper::getLockedParamsFromParent (const RandomiseLockHelper& parentHelper)
{
    // copy locked params from parent
    lockedParams = parentHelper.lockedParams;
}

void RandomiseLockHelper::toggleRandomiseLock (const String& paramID)
{
    if (lockedParams.contains (paramID))
        lockedParams.removeString (paramID);
    else
        lockedParams.addIfNotAlreadyThere (paramID);
}

void RandomiseLockHelper::createPopupMenu (PopupMenu& parent, const String& paramID)
{
    bool isLocked = isParamLocked (paramID);

    PopupMenu randLockMenu;
    for (int i = 0; i < 2; ++i)
    {
        PopupMenu::Item item (i == 0 ? "Unlock" : "Lock");
        item.itemID = i + 1;
        item.setColour (Colour (i == (int) isLocked ? 0xFF21CCA5 : 0xFFFFFFFF));
        item.action = [=] { toggleRandomiseLock (paramID); };

        randLockMenu.addItem (item);
    }

    parent.addSubMenu ("Randomise Lock:", randLockMenu);
}

void RandomiseLockHelper::saveState (XmlElement* xml)
{
    xml->setAttribute ("rand_lock", lockedParams.joinIntoString (",") + ",");
}

void RandomiseLockHelper::loadState (XmlElement* xml)
{
    ParamHelpers::loadStringArray (lockedParams, xml->getStringAttribute ("rand_lock", String()));
}
