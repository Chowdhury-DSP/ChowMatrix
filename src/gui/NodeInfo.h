#pragma once

#include "../dsp/DelayNode.h"
#include "ParamSlider.h"

namespace NodeInfoConsts
{
constexpr int InfoWidth = 95;
constexpr int InfoWidthNoLabel = 80;
constexpr int InfoHeight = 18;
constexpr int InfoHeightNoLabel = 22;

static StringArray skipParams { ParamTags::distTag, ParamTags::revTag, ParamTags::modFreqTag, ParamTags::delayModTag, ParamTags::panModTag };
} // namespace NodeInfoConsts

/** Node info component containing a column of ParamSliders */
class NodeInfo : public Component
{
public:
    NodeInfo (DelayNode& node, bool showLabel = true) : node (node),
                                                        showLabel (showLabel)
    {
        for (int i = 0; i < node.getNumParams(); ++i)
        {
            // in matrix view, don't show skip params
            if (showLabel && NodeInfoConsts::skipParams.contains (node.getNodeParameter (i)->paramID))
                continue;

            addAndMakeVisible (sliders.add (std::make_unique<ParamSlider> (node, dynamic_cast<AudioProcessorValueTreeState::Parameter*> (node.getNodeParameter (i)), showLabel)));
        }

        const int width = showLabel ? NodeInfoConsts::InfoWidth : NodeInfoConsts::InfoWidthNoLabel;
        const int height = showLabel ? NodeInfoConsts::InfoHeight : NodeInfoConsts::InfoHeightNoLabel;
        setSize (width, height * sliders.size());
    }

    void paint (Graphics& g) override
    {
        Colour background = showLabel ? Colour (0xFF344B6F) : Colour (0xFF162947);
        g.fillAll (background);
    }

    void resized() override
    {
        const int height = showLabel ? NodeInfoConsts::InfoHeight : NodeInfoConsts::InfoHeightNoLabel;
        for (int i = 0; i < sliders.size(); ++i)
            sliders[i]->setBounds (0, height * i, getWidth(), height);
    }

    DelayNode* getNode() { return &node; }

private:
    OwnedArray<ParamSlider> sliders;
    DelayNode& node;
    const bool showLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeInfo)
};
