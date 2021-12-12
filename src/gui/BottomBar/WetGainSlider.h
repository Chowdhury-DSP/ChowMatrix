#pragma once

#include "../IOSUtils/LongPressActionHelper.h"
#include "../IOSUtils/PopupMenuOptionsHelpers.h"
#include "BottomBarLNF.h"
#include "ChowMatrix.h"
#include "TextSliderItem.h"

class WetGainSlider : public TextSlider
{
public:
    explicit WetGainSlider (AudioProcessorValueTreeState& vts) : attachment (
        *vts.getParameter ("wet_gain_comp_param"),
        [=] (float v) { updateMenu (v == 1.0f); },
        vts.undoManager)
    {
        attachment.sendInitialUpdate();
        gainCompMenu.setLookAndFeel (lnfAllocator->getLookAndFeel<BottomBarLNF>());

#if JUCE_IOS
        longPressAction.longPressCallback = [=] (Point<int>)
        {
            gainCompMenu.showMenuAsync (PopupMenuOptionsHelpers::createPopupMenuOptions (this));
        };
#endif
    }

    void mouseDown (const MouseEvent& e) override
    {
        if (e.mods.isPopupMenu())
        {
            gainCompMenu.showMenuAsync (PopupMenuOptionsHelpers::createPopupMenuOptions (this));
            return;
        }

        TextSlider::mouseDown (e);

#if JUCE_IOS
        longPressAction.startPress (e.getMouseDownPosition());
#endif
    }

    void mouseDrag (const MouseEvent& e) override
    {
        TextSlider::mouseDrag (e);

#if JUCE_IOS
        longPressAction.setDragDistance ((float) e.getDistanceFromDragStart());
#endif
    }

    void mouseUp (const MouseEvent& e) override
    {
        TextSlider::mouseUp (e);

#if JUCE_IOS
        longPressAction.abortPress();
#endif
    }

    void updateMenu (bool gainCompOn)
    {
        PopupMenu::Item gainCompItem;
        gainCompItem.itemID = 1;
        gainCompItem.text = "Automatic Gain Compensation";
        gainCompItem.action = [=] { attachment.setValueAsCompleteGesture ((float) ! gainCompOn); };
        gainCompItem.colour = gainCompOn ? Colour (0xFF21CCA5) : Colours::white;

        gainCompMenu.clear();
        gainCompMenu.addItem (gainCompItem);
    }

private:
    ParameterAttachment attachment;

    PopupMenu gainCompMenu;
    chowdsp::SharedLNFAllocator lnfAllocator;

#if JUCE_IOS
    LongPressActionHelper longPressAction;
#endif

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
