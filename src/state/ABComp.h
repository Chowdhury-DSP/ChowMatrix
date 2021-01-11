#pragma once

#include "../ChowMatrix.h"
#include "StateManager.h"
#include "../gui/BottomBar/BottomBarLNF.h"

/** GUI component to manage A/B states */
class ABComp : public Component
{
public:
    ABComp (StateManager& manager);
    ~ABComp();

    void paint (Graphics& g) override;
    void resized() override;

    void refreshStates();

private:
    struct ABLNF : public BottomBarLNF
    {
        Font getTextButtonFont (TextButton&, int) override { return Font (15.0f).boldened(); }

        void drawButtonText (Graphics& g, TextButton& button, bool, bool) override
        {
            Font font (getTextButtonFont (button, button.getHeight()));
            g.setFont (font);
            g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                                                    : TextButton::textColourOffId)
                               .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

            const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
            const int leftIndent  = 2;
            const int rightIndent = 2;
            const int textWidth = button.getWidth() - leftIndent - rightIndent;

            if (textWidth > 0)
                g.drawFittedText (button.getButtonText(),
                                  leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                                  Justification::centred, 2);
        }
    };
    ABLNF lnf;

    StateManager& manager;

    TextButton aButton;
    TextButton bButton;
    TextButton arrowButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ABComp)
};

class ABCompItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ABCompItem)

    ABCompItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) :
        foleys::GuiItem (builder, node)
    {
        if (auto* proc = dynamic_cast<ChowMatrix*> (builder.getMagicState().getProcessor()))
        {
            comp = std::make_unique<ABComp> (proc->getStateManager());
            addAndMakeVisible (comp.get());
        }
    }

    void update() override
    {
    }

    Component* getWrappedComponent() override
    {
        return comp.get();
    }

private:
    std::unique_ptr<ABComp> comp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ABCompItem)
};
