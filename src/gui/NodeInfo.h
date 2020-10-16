#pragma once

#include "ParamSlider.h"
#include "../dsp/DelayNode.h"

namespace NodeInfoConsts
{
    constexpr int InfoWidth = 120;
    constexpr int InfoWidthNoLabel = 60;
}

class NodeInfo : public Component
{
public:
    NodeInfo (DelayNode& node, bool showLabel = true) :
        showLabel (showLabel)
    {
        for (int i = 0; i < node.getNumParams(); ++i)
            addAndMakeVisible (sliders.add (std::make_unique<ParamSlider> (node.getParam (i), showLabel)));

        int width = showLabel ? NodeInfoConsts::InfoWidth : NodeInfoConsts::InfoWidthNoLabel;
        setSize (width, 30 * sliders.size());
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::slategrey);
    }

    void resized() override
    {
        for (int i = 0; i < sliders.size(); ++i)
            sliders[i]->setBounds (0, 30 * i, getWidth(), 30);
    }

private:
    OwnedArray<ParamSlider> sliders;
    const bool showLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeInfo)
};
