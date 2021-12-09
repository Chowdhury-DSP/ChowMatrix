#include "BottomBarLNF.h"

namespace
{
constexpr float heightFrac = 0.5f;
}

int getNameWidth (int height, const String& text)
{
    Font f = Font ((float) height * heightFrac);
    return f.getStringWidth (text);
}

BottomBarLNF::BottomBarLNF()
{
    setColour (PopupMenu::backgroundColourId, Colour (0xFF31323A));
    setColour (PopupMenu::highlightedBackgroundColourId, Colour (0x7FC954D4));
    setColour (PopupMenu::highlightedTextColourId, Colours::white);
}

void BottomBarLNF::drawRotarySlider (Graphics& g, int, int, int, int height, float, const float, const float, Slider& slider)
{
    g.setColour (Colours::white); // @TODO: make colour selectable
    g.setFont (Font ((float) height * heightFrac).boldened());

    String text = slider.getName() + ": ";
    int width = getNameWidth (height, text);
    g.drawFittedText (text, 0, 0, width, height, Justification::centred, 1);
}

Slider::SliderLayout BottomBarLNF::getSliderLayout (Slider& slider)
{
    auto layout = LookAndFeel_V4::getSliderLayout (slider);
    layout.textBoxBounds = slider.getLocalBounds()
                               .removeFromRight (slider.getWidth()
                                                 - getNameWidth (slider.getHeight(), slider.getName() + ":_") + 3);
    return layout;
}

Label* BottomBarLNF::createSliderTextBox (Slider& slider)
{
    auto* label = LookAndFeel_V4::createSliderTextBox (slider);
    label->setInterceptsMouseClicks (false, false);
    label->setColour (Label::outlineColourId, Colours::transparentBlack);
    label->setColour (Label::outlineWhenEditingColourId, Colours::transparentBlack);
    label->setJustificationType (Justification::centred);
    label->setFont (Font (16.0f).boldened());

    label->onEditorShow = [label] {
        if (auto editor = label->getCurrentTextEditor())
        {
            editor->setBounds (label->getLocalBounds());
            editor->setColour (CaretComponent::caretColourId, Colour (0xFFC954D4));
            editor->setColour (TextEditor::backgroundColourId, Colours::transparentBlack);
            editor->setJustification (Justification::left);
            editor->applyFontToAllText (Font (14.0f).boldened());
        }
    };

    return label;
}

void BottomBarLNF::drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box)
{
    auto cornerSize = 5.0f;
    Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    if (box.getName().isNotEmpty())
    {
        g.setColour (Colours::white);
        g.setFont (getComboBoxFont (box).boldened());
        auto nameBox = boxBounds.withWidth (boxBounds.proportionOfWidth (0.3f));
        g.drawFittedText (box.getName() + ": ", nameBox, Justification::right, 1);
    }
}

void BottomBarLNF::positionComboBoxText (ComboBox& box, Label& label)
{
    auto b = box.getBounds();

    if (box.getName().isNotEmpty())
    {
        auto width = b.proportionOfWidth (0.7f);
        auto x = b.proportionOfWidth (0.3f);
        b = b.withX (x).withWidth (width);
    }

    label.setBounds (b);
    label.setFont (getComboBoxFont (box).boldened());
    label.setJustificationType (Justification::topLeft);
}

void BottomBarLNF::drawButtonBackground (Graphics& g,
                                         Button& button,
                                         const Colour& backgroundColour,
                                         bool /*shouldDrawButtonAsHighlighted*/,
                                         bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                          .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    g.setColour (baseColour);
    g.fillRect (bounds);
}
