#pragma once

#include <JuceHeader.h>

class BottomBarLNF : public chowdsp::ChowLNF
{
public:
    BottomBarLNF() {}
    virtual ~BottomBarLNF() {}

protected:
    void drawRotarySlider (Graphics& g, int, int, int, int,
                           float, const float, const float, Slider& slider) override;

    Slider::SliderLayout getSliderLayout (Slider& slider) override;

    Label* createSliderTextBox (Slider& slider) override;
};
