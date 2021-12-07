#pragma once

#include "../dsp/DelayNode.h"

/**
 * Text slider mapped to a parameter
 */
class ParamSlider : public Slider
{
public:
    ParamSlider (DelayNode& node, AudioProcessorValueTreeState::Parameter* param, bool showLabel = true);
    ~ParamSlider() override;

    void resized() override;
    void paint (Graphics&) override;

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

private:
    class SliderAttachment : private Slider::Listener
    {
    public:
        SliderAttachment (ParamSlider& slider);
        ~SliderAttachment() override;

    private:
        void setValue (float value);
        void sliderValueChanged (Slider*) override;

        void sliderDragStarted (Slider*) override { attachment.beginGesture(); }
        void sliderDragEnded (Slider*) override { attachment.endGesture(); }

        ParamSlider& slider;
        ParameterAttachment attachment;
        bool ignoreCallbacks = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderAttachment)
    };

    void setParameterValue (float value01, NotificationType notificationType);
    void setValueText (const String& paramID, float value01);
    void setLinkedParams (float diff01);
    void toggleParamLock();

    DelayNode& node;
    AudioProcessorValueTreeState::Parameter* param;

    Label nameLabel;
    Label valueLabel;

    SliderAttachment attachment;

    const bool showLabel;
    bool isDragging = false;
    bool isDoubleClicking = false;
    bool isMultiClicking = false;
    std::atomic_bool linkFlag;
    std::atomic_bool isInGesture;

    chowdsp::SharedLNFAllocator lnfAllocator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSlider)
};
