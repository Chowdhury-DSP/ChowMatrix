#pragma once

#include "../../ChowMatrix.h"
#include "../../state/StateManager.h"
#include "BottomBarLNF.h"

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
    StateManager& manager;

    TextButton aButton;
    TextButton bButton;
    TextButton arrowButton;

    chowdsp::SharedLNFAllocator lnfAllocator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ABComp)
};

class ABCompItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ABCompItem)

    ABCompItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) : foleys::GuiItem (builder, node)
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
