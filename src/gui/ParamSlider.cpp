#include "ParamSlider.h"

ParamSlider::ParamSlider (AudioParameterFloat* param) :
    param (param)
{
    param->addListener (this);

    addAndMakeVisible (nameLabel);
    addAndMakeVisible (valueLabel);

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
    const int halfWidth = getWidth() / 2;
    nameLabel.setBounds (0, 0, halfWidth, getHeight());
    valueLabel.setBounds (halfWidth, 0, halfWidth, getHeight());
}
