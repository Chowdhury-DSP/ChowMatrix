#pragma once

#include "BottomBarLNF.h"
#include "ChowMatrix.h"
#include "TextSliderItem.h"

class WetGainSlider : public TextSlider
{
public:
    explicit WetGainSlider (AudioProcessorValueTreeState& vts) : attachment (
        *vts.getParameter ("wet_gain_comp_param"),
        [=] (float v)
        { updateMenu (v == 1.0f); },
        vts.undoManager)
    {
        attachment.sendInitialUpdate();
        gainCompMenu.setLookAndFeel (lnfAllocator->getLookAndFeel<BottomBarLNF>());
    }

    void mouseDown (const MouseEvent& e) override
    {
        if (e.mods.isPopupMenu())
        {
            gainCompMenu.showMenuAsync (PopupMenu::Options());
            return;
        }

        TextSlider::mouseDown (e);
    }

    void updateMenu (bool gainCompOn)
    {
        PopupMenu::Item gainCompItem;
        gainCompItem.itemID = 1;
        gainCompItem.text = "Automatic Gain Compensation";
        gainCompItem.action = [=]
        { attachment.setValueAsCompleteGesture ((float) ! gainCompOn); };
        gainCompItem.colour = gainCompOn ? Colour (0xFF21CCA5) : Colours::white;

        gainCompMenu.clear();
        gainCompMenu.addItem (gainCompItem);
    }

private:
    ParameterAttachment attachment;

    PopupMenu gainCompMenu;
    chowdsp::SharedLNFAllocator lnfAllocator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WetGainSlider)
};

class WetGainSliderItem : public TextSliderItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (WetGainSliderItem)

    WetGainSliderItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) : TextSliderItem (builder, node)
    {
        auto* plugin = dynamic_cast<ChowMatrix*> (builder.getMagicState().getProcessor());
        slider = std::make_unique<WetGainSlider> (plugin->getVTS());
        addAndMakeVisible (slider.get());
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WetGainSliderItem)
};
