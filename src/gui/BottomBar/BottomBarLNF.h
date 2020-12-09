#pragma once

#include <pch.h>

/** Look and feel for the bottom bar section */
class BottomBarLNF : public chowdsp::ChowLNF
{
public:
    BottomBarLNF();
    virtual ~BottomBarLNF() = default;

protected:
    void drawRotarySlider (Graphics& g, int, int, int, int,
                           float, const float, const float, Slider& slider) override;

    Slider::SliderLayout getSliderLayout (Slider& slider) override;
    Label* createSliderTextBox (Slider& slider) override;

    void drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box) override;
    void positionComboBoxText (ComboBox& box, Label& label) override;

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    Font getTextButtonFont (TextButton&, int buttonHeight) override
    {
        float fontHeight = jmin (17.0f, (float) buttonHeight * 0.85f);
        return Font { fontHeight, Font::bold };
    }

    void drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
        const bool isSeparator, const bool isActive,
        const bool isHighlighted, const bool /*isTicked*/,
        const bool hasSubMenu, const String& text,
        const String& shortcutKeyText,
        const Drawable* icon, const Colour* const textColourToUse) override
    {
        LookAndFeel_V4::drawPopupMenuItem (g, area, isSeparator, isActive,
            isHighlighted, false /*isTicked*/, hasSubMenu, text,
            shortcutKeyText, icon, textColourToUse);
    }

    void drawPopupMenuBackground (Graphics& g, int width, int height) override
    {
        g.fillAll (findColour (PopupMenu::backgroundColourId));
        ignoreUnused (width, height);
    }
};
