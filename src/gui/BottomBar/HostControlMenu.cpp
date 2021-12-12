#include "HostControlMenu.h"
#include "HostControlMenuComp.h"
#include "../IOSUtils/PopupMenuOptionsHelpers.h"

HostControlMenu::HostControlMenu (HostParamControl& controller) : controller (controller)
{
    cog = Drawable::createFromImageData (BinaryData::cogsolid_svg, BinaryData::cogsolid_svgSize);
    cog->replaceColour (Colours::black, Colours::white);

    setTooltip ("Lists which parameters are currently assigned to targets");
}

void HostControlMenu::paint (Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto dim = jmin (bounds.getWidth(), bounds.getHeight());
    bounds.setSize (dim, dim);
    bounds.reduce (6.0f, 6.0f);

    auto placement = RectanglePlacement (RectanglePlacement::stretchToFit);
    cog->drawWithin (g, bounds, placement, 1.0f);
}

void HostControlMenu::mouseDown (const MouseEvent& e)
{
    PopupMenu menu;
    for (size_t i = 0; i < controller.getNumAssignableParams(); ++i)
        menu.addCustomItem ((int) i + 1, std::make_unique<HostControlMenuComp> (controller, i));

    auto popupOptions = PopupMenuOptionsHelpers::createPopupMenuOptions (this)
                            .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::upwards);

    menu.setLookAndFeel (&getLookAndFeel());
    menu.showMenuAsync (popupOptions,
                        [=] (int id)
                        {
                            if (id > 0)
                                mouseDown (e);
                        });
}
