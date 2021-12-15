#pragma once

#if JUCE_IOS

#include "../BottomBar/BottomBarLNF.h"

class TipJarLNF : public BottomBarLNF
{
    void drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box) override
    {
        auto cornerSize = 5.0f;
        Rectangle<int> boxBounds (0, 0, width, height);

        g.setColour (box.findColour (ComboBox::backgroundColourId));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
    }

    void positionComboBoxText (ComboBox& box, Label& label) override
    {
        LookAndFeel_V4::positionComboBoxText (box, label);
        label.setFont (getComboBoxFont (box).boldened());
    }
};

class TipJar : public ComboBox,
               private InAppPurchases::Listener
{
public:
    TipJar();
    ~TipJar() override;

    void productsInfoReturned (const Array<InAppPurchases::Product>& products) override;
    void doTipPurchase (const String& id);
    void setDisconnectedMenu();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TipJar)
};

class TipJarItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (TipJarItem)

    TipJarItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) : foleys::GuiItem (builder, node)
    {
        addAndMakeVisible (tipJar);
    }

    void update() override {}

    Component* getWrappedComponent() override { return &tipJar; }

private:
    TipJar tipJar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TipJarItem)
};

#endif // JUCE_IOS
