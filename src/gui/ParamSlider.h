#pragma once

#include <pch.h>
#include "../dsp/DelayNode.h"

/**
 * Text slider mapped to a parameter
 */ 
class ParamSlider : public Slider,
                    private AudioProcessorParameter::Listener,
                    private Slider::Listener
{
public:
    ParamSlider (DelayNode& node, AudioProcessorValueTreeState::Parameter* param, bool showLabel = true);
    ~ParamSlider();

    void parameterValueChanged (int, float) override;
    void parameterGestureChanged (int, bool) override {}
    void sliderValueChanged (Slider*) override;

    void resized() override;
    void paint (Graphics&) override {}

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

private:
    DelayNode& node;
    AudioProcessorValueTreeState::Parameter* param;
    Label nameLabel;
    Label valueLabel;

    const bool showLabel;
    bool isDragging = false;
    std::atomic_bool linkFlag;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSlider)
};
