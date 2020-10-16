#include "ParamSlider.h"

ParamSlider::ParamSlider (AudioParameterFloat* param, bool showLabel) :
    param (param),
    showLabel (showLabel)
{
    param->addListener (this);

    addAndMakeVisible (nameLabel);
    addAndMakeVisible (valueLabel);

    if (! showLabel)
        valueLabel.setJustificationType (Justification::centred);

    nameLabel.setText (param->paramID, sendNotification);
    parameterValueChanged (0, 0.0f);
}

ParamSlider::~ParamSlider()
{
    param->removeListener (this);
}

void ParamSlider::parameterValueChanged (int, float)
{
    valueLabel.setText (param->getCurrentValueAsText(), sendNotification);
}

void ParamSlider::resized()
{
    if (showLabel)
    {
        const int halfWidth = getWidth() / 2;
        nameLabel.setBounds (0, 0, halfWidth, getHeight());
        valueLabel.setBounds (halfWidth, 0, halfWidth, getHeight());
    }
    else
    {
        valueLabel.setBounds (getLocalBounds());
    }
    
}
