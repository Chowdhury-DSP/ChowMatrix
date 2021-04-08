#include "HostControlMenuComp.h"

HostControlMenuComp::HostControlMenuComp (HostParamControl& controller, size_t idx) : PopupMenu::CustomComponent (false),
                                                                                      name ("Assign " + String (idx + 1))
{
    controller.loadParamList (paramList, xCallbacks, idx);

    for (auto& xCallback : xCallbacks)
    {
        auto newButton = xButtons.add (std::make_unique<DrawableButton> ("Delete", DrawableButton::ImageFitted));
        createXButton (*newButton);
        addAndMakeVisible (newButton);
        newButton->onClick = [=] {
            xCallback();
            triggerMenuItem();
        };
    }
}

void HostControlMenuComp::createXButton (DrawableButton& xButton)
{
    Rectangle<float> buttonZone (-10.0f, -10.0f, 20.0f, 20.0f);

    DrawableRectangle redSquare;
    redSquare.setRectangle (buttonZone);
    redSquare.setFill (Colours::red);

    Path p;
    constexpr auto thickness = 1.5f;
    constexpr auto indent = 2.0f;
    p.addRectangle (-8.0f, -thickness, 20.0f - indent * 2.0f, thickness * 2.0f);
    p.addRectangle (-thickness, -8.0f, thickness * 2.0f, 10.0f - indent - thickness);
    p.addRectangle (-thickness, thickness, thickness * 2.0f, 10.0f - indent - thickness);
    p.setUsingNonZeroWinding (false);
    p.applyTransform (AffineTransform::rotation (MathConstants<float>::halfPi / 2.0f));

    DrawablePath dp;
    dp.setPath (p);
    dp.setFill (Colours::white);

    DrawableComposite buttonImage;
    buttonImage.addAndMakeVisible (redSquare.createCopy().release());
    buttonImage.addAndMakeVisible (dp.createCopy().release());

    xButton.setImages (&buttonImage);
}

void HostControlMenuComp::getIdealSize (int& idealWidth, int& idealHeight)
{
    idealWidth = 250;
    idealHeight = labelHeight * jmax (paramList.size(), 1);
}

void HostControlMenuComp::paint (Graphics& g)
{
    auto bounds = getLocalBounds();
    bounds.removeFromRight (20); // save space for xButtons

    g.setColour (Colours::white);
    g.setFont (Font (font).boldened());

    auto labelBox = bounds.removeFromLeft (60);
    g.drawFittedText (name, labelBox.removeFromTop (labelHeight), Justification::centred, 1);

    if (paramList.isEmpty())
    {
        g.setColour (Colours::lightgrey.withAlpha (0.85f));
        g.setFont (Font (font));
        g.drawFittedText ("None", bounds, Justification::centred, 1);
    }
    else
    {
        g.setColour (Colours::white);
        g.setFont (Font (font));
        for (auto& pString : paramList)
            g.drawFittedText (pString, bounds.removeFromTop (labelHeight), Justification::centred, 1);
    }

    g.setColour (Colours::lightgrey.withAlpha (0.25f));
    auto lineY = (float) getHeight();
    g.drawLine (0, lineY, (float) getWidth(), lineY);
}

void HostControlMenuComp::resized()
{
    auto buttonsBox = getLocalBounds().removeFromRight (20);

    for (auto* xButton : xButtons)
        xButton->setBounds (buttonsBox.removeFromTop (labelHeight));
}
