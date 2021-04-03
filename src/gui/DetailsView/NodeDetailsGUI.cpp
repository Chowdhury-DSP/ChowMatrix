#include "NodeDetailsGUI.h"
#include "../../dsp/Parameters/ParamHelpers.h"

NodeDetailsGUI::NodeDetailsGUI (ChowMatrix& chowMatrix) : nodeDetailsViewport (chowMatrix)
{
    setColour (nodeColour, Colours::green);
    setColour (nodeSelectedColour, Colours::pink);
    setColour (scrollThumbColour, Colours::pink);
    setColour (scrollTrackColour, Colours::grey);

    auto node = std::make_unique<DelayNode>();
    auto& params = node->getParameters();
    for (int i = 0; i < 1 + params.size(); ++i)
    {
        String name = "Node";
        String text = "Node";
        String param = "parameters";
        if (i > 0)
        {
            name = params[i - 1]->getName (1024);
            text = name;
            param = name;
        }

        auto* l = labels.add (std::make_unique<Label> (name, text));
        l->setFont (16.0f);

        if (i < 3) // Delay time and pan have "Insanity Lock" option
#if JUCE_MAC
            l->setTooltip ("Use shift+drag to change all nodes in unison, or CMD+click for Insanity Lock/Reset");
#else
            l->setTooltip ("Use shift+drag to change all nodes in unison, or CTRL+click for Insanity Lock/Reset");
#endif
        else if (i == 10) // Mod. Freq. has "Tempo Sync" option
#if JUCE_MAC
            l->setTooltip ("Use shift+drag to change all nodes in unison, or CMD+click for Tempo Sync");
#else
            l->setTooltip ("Use shift+drag to change all nodes in unison, or CTRL+click for Tempo Sync");
#endif
        else
            l->setTooltip ("Use shift+drag to adjust all nodes in unison. Use right-click to assign to target");

        addAndMakeVisible (l);
    }

    addAndMakeVisible (nodeDetailsViewport);
}

void NodeDetailsGUI::resized()
{
    const auto labelWidth = int ((float) getLocalBounds().getWidth() * 0.12f);
    const auto yOffset = nodeDetailsViewport.getViewPositionY();

    labels[0]->setBounds (0, -yOffset, labelWidth, DetailsConsts::buttonHeight);
    for (int i = 1; i < labels.size(); ++i)
        labels[i]->setBounds (0, labels[i - 1]->getBottom(), labelWidth, NodeInfoConsts::InfoHeightNoLabel);

    nodeDetailsViewport.setBounds (labelWidth, 0, getWidth() - labelWidth, getHeight());
}
