#include "NodeDetailsGUI.h"
#include "../../dsp/ParamHelpers.h"

NodeDetailsGUI::NodeDetailsGUI (ChowMatrix& chowMatrix) :
    nodeDetailsViewport (chowMatrix)
{
    setColour (nodeColour, Colours::green);
    setColour (nodeSelectedColour, Colours::pink);
    setColour (scrollThumbColour, Colours::pink);
    setColour (scrollTrackColour, Colours::grey);

    auto node = std::make_unique<DelayNode>();
    for (int i = 0; i < 1 + node->getNumParameters(); ++i)
    {
        String name = "Node";
        String text = "Node";
        if (i > 0)
        {
            name = node->getParameters()[i-1]->getName(1024);
            text = name;
        }

        auto* l = labels.add (std::make_unique<Label> (name, text));
        l->setFont (16.0f);
        addAndMakeVisible (l);
    }

    addAndMakeVisible (nodeDetailsViewport);    
}

void NodeDetailsGUI::resized()
{
    const auto labelBounds = getLocalBounds()
        .getProportion (Rectangle<float> { 0.12f, 1.0f / (float) labels.size() });
    const auto labelWidth = labelBounds.getWidth();
    const auto labelHeight = labelBounds.getHeight();

    labels[0]->setBounds (0, 0, labelWidth, DetailsConsts::buttonHeight);
    for (int i = 1; i < labels.size(); ++i)
        labels[i]->setBounds (0, labels[i-1]->getBottom(), labelWidth, NodeInfoConsts::InfoHeightNoLabel);

    nodeDetailsViewport.setBounds (labelWidth, 0, getWidth() - labelWidth, getHeight());
}
