#pragma once

#include <JuceHeader.h>

class ParamSlider : public Slider,
                    private AudioProcessorParameter::Listener
{
public:
    ParamSlider (AudioProcessorValueTreeState::Parameter* param, bool showLabel = true);
    ~ParamSlider();

    void parameterValueChanged (int, float) override;
    void parameterGestureChanged (int, bool) override {}

    void paint (Graphics& g) override;
    void resized() override;

    void mouseDrag (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

private:
    AudioProcessorValueTreeState::Parameter* param;
    Label nameLabel;
    Label valueLabel;

    const bool showLabel;
    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSlider)
};
