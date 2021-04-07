#pragma once

#include "ChowMatrix.h"
#include "dsp/Parameters/HostParamControl.h"

class HostControlMenu : public Component,
                        public SettableTooltipClient
{
public:
    HostControlMenu (HostParamControl& controller);

    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;

private:
    HostParamControl& controller;
    std::unique_ptr<Drawable> cog;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostControlMenu)
};

class HostControlMenuItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (HostControlMenuItem)

    HostControlMenuItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) : foleys::GuiItem (builder, node)
    {
        if (auto* proc = dynamic_cast<ChowMatrix*> (builder.getMagicState().getProcessor()))
        {
            comp = std::make_unique<HostControlMenu> (proc->getHostControl());
            addAndMakeVisible (comp.get());
        }
    }

    void update() override
    {
        comp->repaint();
    }

    Component* getWrappedComponent() override
    {
        return comp.get();
    }

private:
    std::unique_ptr<HostControlMenu> comp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HostControlMenuItem)
};
