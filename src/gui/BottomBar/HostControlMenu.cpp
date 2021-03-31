#include "HostControlMenu.h"

class HostControlMenuComp : public PopupMenu::CustomComponent
{
public:
    HostControlMenuComp (const HostParamControl& controller, size_t idx) : PopupMenu::CustomComponent (false),
                                                                           name ("Assign " + String (idx + 1))
    {
        controller.loadParamList (paramList, idx);
    }

    void getIdealSize (int& idealWidth, int& idealHeight) override
    {
        idealWidth = 250;
        idealHeight = labelHeight * jmax (paramList.size(), 1);
    }

    void paint (Graphics& g) override
    {
        auto bounds = getLocalBounds();

        g.setColour (Colours::white);
        g.setFont (Font (font).boldened());

        auto labelBox = bounds.removeFromLeft (60);
        g.drawFittedText (name, labelBox.removeFromTop (labelHeight), Justification::centred, 1);

        if (paramList.isEmpty())
        {
            g.setColour (Colours::lightgrey.withAlpha (0.85f));
            g.setFont (Font (font));
            g.drawFittedText ("None", bounds, Justification::centred, 1);
        }
        else
        {
            g.setColour (Colours::white);
            g.setFont (Font (font));
            for (auto& pString : paramList)
                g.drawFittedText (pString, bounds.removeFromTop (labelHeight), Justification::centred, 1);
        }

        g.setColour (Colours::lightgrey.withAlpha (0.25f));
        auto lineY = (float) getHeight();
        g.drawLine (0, lineY, (float) getWidth(), lineY);
    }

private:
    String name;
    StringArray paramList;

    static constexpr float font = 16.0f;
    static constexpr int labelHeight = 25;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostControlMenuComp)
};

HostControlMenu::HostControlMenu (const HostParamControl& controller) : controller (controller)
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

void HostControlMenu::mouseDown (const MouseEvent&)
{
    PopupMenu menu;
    for (size_t i = 0; i < controller.getNumAssignableParams(); ++i)
    {
        menu.addCustomItem ((int) i + 1, std::make_unique<HostControlMenuComp> (controller, i));
    }

    menu.setLookAndFeel (&getLookAndFeel());
    menu.showMenuAsync (PopupMenu::Options().withPreferredPopupDirection (PopupMenu::Options::PopupDirection::upwards));
}
