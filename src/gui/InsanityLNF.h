#pragma once

#include <JuceHeader.h>

class InsanityLNF : public chowdsp::ChowLNF
{
public:
    InsanityLNF() {}
    virtual ~InsanityLNF() {}

protected:
    void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const Slider::SliderStyle, Slider & slider) override
    {
        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

        Point<float> startPoint (slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                 slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

        Point<float> endPoint (slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : (float) y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = getSliderThumbRadius (slider);

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (maxPoint);

        ColourGradient gr (Colour (0xffb843c3), startPoint,
                           Colour (0xff38bb9d), endPoint, false);
        g.setGradientFill (gr);
        g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        auto thumbRect = Rectangle<float> (static_cast<float> (thumbWidth),
                                       static_cast<float> (thumbWidth)).withCentre (maxPoint);
        knob->drawWithin (g, thumbRect, RectanglePlacement::stretchToFit, 1.0f);
    }

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override { return robotoBold; }
};
