#pragma once

#include <pch.h>

namespace PopupMenuOptionsHelpers
{
inline PopupMenu::Options createPopupMenuOptions (Component* targetComp = nullptr, Component* parentComp = nullptr)
{
    auto options = PopupMenu::Options();

    if (targetComp != nullptr)
        options = options.withTargetComponent (targetComp);

    if (parentComp != nullptr)
        options = options.withParentComponent (parentComp);

#if JUCE_IOS
    options = options.withStandardItemHeight (35);
#endif

    return options;
}
} // namespace PopupMenuOptionsHelpers
