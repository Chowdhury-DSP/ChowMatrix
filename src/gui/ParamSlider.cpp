#include "ParamSlider.h"
#include "../dsp/DelayNode.h"

ParamSlider::ParamSlider (Parameter* param, bool showLabel) :
    param (param),
    showLabel (showLabel)
{
    setName (param->name);
    setTooltip (ParamHelpers::getTooltip (param->paramID));

    param->addListener (this);

    addAndMakeVisible (nameLabel);
    addAndMakeVisible (valueLabel);

    if (showLabel)
    {
        nameLabel.setFont (Font (13.0f, Font::bold));
        nameLabel.setColour (Label::textColourId, Colours::white);
        valueLabel.setFont (Font (13.0f, Font::bold));
        valueLabel.setColour (Label::textColourId, Colour (0xFF21CCA5));
        valueLabel.setJustificationType (Justification::right);
    }
    else
    {
        valueLabel.setFont (Font (16.0f));
        valueLabel.setColour (Label::textColourId, Colours::white);
        valueLabel.setJustificationType (Justification::centred);
    }

    nameLabel.setText (param->paramID, sendNotification);
    valueLabel.setInterceptsMouseClicks (false, false);
    parameterValueChanged (0, 0.0f);

    setRange (0.0, 1.0);
    setSliderStyle (SliderStyle::RotaryVerticalDrag);
    setDoubleClickReturnValue (true, param->getDefaultValue());
    onValueChange = [=] {
        ParamHelpers::setParameterValue (param, param->convertFrom0to1 ((float) this->getValue()));
    };
}

ParamSlider::~ParamSlider()
{
    param->removeListener (this);
}

void ParamSlider::parameterValueChanged (int, float)
{
    valueLabel.setText (param->getCurrentValueAsText(), sendNotification);
    this->setValue (param->convertTo0to1 (param->get()), dontSendNotification);
}

void ParamSlider::resized()
{
    if (showLabel)
    {
        const int halfWidth = getBounds().getProportion (Rectangle<float> (0.4f, 1.0f)).getWidth();
        nameLabel.setBounds (0, 0, halfWidth, getHeight());
        valueLabel.setBounds (halfWidth, 0, getWidth() - halfWidth, getHeight());
    }
    else
    {
        valueLabel.setBounds (getLocalBounds());
    }
}

void ParamSlider::mouseDrag (const MouseEvent& e)
{
    isDragging = true;
    Slider::mouseDrag (e);
}

void ParamSlider::mouseDoubleClick (const MouseEvent& e)
{
    hideTextBox (false);
    Slider::mouseDoubleClick (e);
}

void ParamSlider::mouseUp (const MouseEvent& e)
{
    Slider::mouseUp (e);

    if (! isDragging && ! e.mods.isAnyModifierKeyDown())
        showTextBox();

    isDragging = false;
}
