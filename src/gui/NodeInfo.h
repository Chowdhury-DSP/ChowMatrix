#pragma once

#include "ParamSlider.h"
#include "../dsp/DelayNode.h"

class NodeInfo : public Component
{
public:
    NodeInfo (DelayNode& node)
    {
        for (int i = 0; i < node.getNumParams(); ++i)
            addAndMakeVisible (sliders.add (std::make_unique<ParamSlider> (node.getParam (i))));

        setSize (120, 30 * sliders.size());
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeInfo)
};
