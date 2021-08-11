#include "ParamSlider.h"
#include "../dsp/Delay/TempoSyncUtils.h"
#include "../dsp/DelayNode.h"

using namespace ParamTags;

ParamSlider::ParamSlider (DelayNode& node, Parameter* param, bool showLabel) : node (node),
                                                                               param (param),
                                                                               showLabel (showLabel)
{
    setName (param->name);
    setTooltip (ParamHelpers::getTooltip (param->paramID));
    nameLabel.setTooltip (getTooltip());
    linkFlag.store (false);
    isInGesture.store (false);

    addListener (this);
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
        valueLabel.setColour (Label::outlineWhenEditingColourId, Colours::transparentBlack);
        valueLabel.setJustificationType (Justification::centred);
        valueLabel.onEditorHide = [=] {
            auto stringFunc = ParamHelpers::getStringFuncForParam (param->paramID);
            auto unNormalisedValue = stringFunc (valueLabel.getText (true));
            ParamHelpers::setParameterValue (param, unNormalisedValue);
        };
    }

    nameLabel.setText (param->paramID, sendNotification);
    valueLabel.setInterceptsMouseClicks (false, false);
    parameterValueChanged (0, 0.0f);

    setRange (0.0, 1.0);
    setSliderStyle (SliderStyle::RotaryVerticalDrag);
    setDoubleClickReturnValue (true, param->getDefaultValue());
}

ParamSlider::~ParamSlider()
{
    param->removeListener (this);
}

void ParamSlider::setValueText (const String& paramID, float value01)
{
    MessageManagerLock mml;

    // special case: delay parameter in Sync mode
    if (paramID == delayTag && node.getDelaySync())
    {
        auto& rhythm = TempoSyncUtils::getRhythmForParam (value01);
        valueLabel.setText (rhythm.getLabel(), sendNotification);
        return;
    }

    // special case: mod freq. parameter in Sync mode
    if (paramID == modFreqTag && node.isLFOSynced())
    {
        auto& rhythm = TempoSyncUtils::getRhythmForParam (value01);
        valueLabel.setText (rhythm.getLabel(), sendNotification);
        return;
    }

    // special case: mod freq. parameter in Sync mode
    if (paramID == revTag && value01 == 0.0f)
    {
        valueLabel.setText ("OFF", sendNotification);
        return;
    }

    valueLabel.setText (param->getCurrentValueAsText(), sendNotification);
}

void ParamSlider::parameterValueChanged (int, float)
{
    auto value01 = param->convertTo0to1 (param->get());
    setValueText (param->paramID, value01);
    this->setValue (value01, dontSendNotification);
}

void ParamSlider::sliderValueChanged (Slider*)
{
    auto value01 = (float) this->getValue();
    auto diff01 = value01 - param->convertTo0to1 (param->get());

    ParamHelpers::setParameterValue (param, param->convertFrom0to1 (value01));

    // if slider is linked, set parameter for all nodes
    if (linkFlag.load())
    {
        if (isDragging)
            node.setParameterDiffListeners (param->paramID, diff01);
        else if (isDoubleClicking)
            node.setParameterDefaultListeners (param->paramID);
    }
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

void ParamSlider::paint (Graphics&)
{
    const auto& lockHelper = node.getInsanityLockHelper();
    auto textColour = lockHelper.isParamLocked (param->paramID) ? Colours::red
                                                                : (lockHelper.shouldParamReset (param->paramID) ? Colour (0xFF21CCA5) : Colours::white);
    valueLabel.setColour (Label::textColourId, textColour);
}

void ParamSlider::toggleParamLock()
{
    node.getInsanityLockHelper().toggleInsanityLock (param->paramID);
}

void ParamSlider::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        auto menu = node.createParamPopupMenu (param->paramID);
        menu.setLookAndFeel (&popupLNF.get());
        menu.showMenuAsync (PopupMenu::Options());

        return;
    }

    if (e.mods.isCommandDown())
    {
        if (param->paramID == delayTag || param->paramID == panTag)
            toggleParamLock();
        else if (param->paramID == modFreqTag)
            node.toggleLFOSync();

        return;
    }

    linkFlag.store (e.mods.isShiftDown());
    Slider::mouseDown (e);

    if (! isInGesture.load())
    {
        isInGesture.store (true);
        node.beginParameterChange ({ param->paramID });
    }
}

void ParamSlider::mouseDrag (const MouseEvent& e)
{
    isDragging = true;
    linkFlag.store (e.mods.isShiftDown());
    Slider::mouseDrag (e);

    if (! isInGesture.load())
    {
        isInGesture.store (true);
        node.beginParameterChange ({ param->paramID });
    }

    node.applyParameterChange (param->paramID, (float) this->getValue());
}

void ParamSlider::mouseDoubleClick (const MouseEvent& e)
{
    isDoubleClicking = true;
    linkFlag.store (e.mods.isShiftDown());
    valueLabel.hideEditor (true);

    if (e.mods.isCommandDown()) // CMD+click is for insanity lock!
        return;

    Slider::mouseDoubleClick (e);
    node.applyParameterChange (param->paramID, (float) this->getValue());
}

void ParamSlider::mouseUp (const MouseEvent& e)
{
    Slider::mouseUp (e);

    bool dontShowLabel = isDragging || e.mods.isAnyModifierKeyDown()
                         || e.mods.isPopupMenu() || showLabel || e.getNumberOfClicks() > 1;
    if (! dontShowLabel)
    {
        valueLabel.showEditor();
        if (auto editor = valueLabel.getCurrentTextEditor())
            editor->setJustification (Justification::centred);
    }

    isDragging = false;
    isDoubleClicking = false;
    linkFlag.store (false);

    if (isInGesture.load())
    {
        isInGesture.store (false);
        node.endParameterChange ({ param->paramID });
    }
}
