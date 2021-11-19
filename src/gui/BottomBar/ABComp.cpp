#include "ABComp.h"

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
        const int leftIndent = 2;
        const int rightIndent = 2;
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent,
                              yIndent,
                              textWidth,
                              button.getHeight() - yIndent * 2,
                              Justification::centred,
                              2);
    }
};

ABComp::ABComp (StateManager& manager) : manager (manager)
{
    auto setupButton = [=, &manager] (TextButton& button, String name, String text, String tooltip, int index) {
        button.setColour (TextButton::buttonColourId, Colours::transparentBlack);
        button.setColour (TextButton::buttonOnColourId, Colour (0xFFC954D4).brighter (0.2f));

        button.setName (name);
        button.setButtonText (text);
        button.setTooltip (tooltip);
        button.setClickingTogglesState (false);

        button.setLookAndFeel (lnfAllocator->getLookAndFeel<ABLNF>());
        addAndMakeVisible (button);

        button.onClick = [=, &manager] {
            manager.setCurrentABState (index);
            refreshStates();
        };
    };

    setupButton (aButton, "A/B", "A", "Load state \"A\"", 0);
    setupButton (bButton, "A/B", "B", "Load state \"B\"", 1);
    setupButton (arrowButton, "A/B", String(), String(), -1);
    arrowButton.onClick = std::bind (&StateManager::copyABState, &manager);

    refreshStates();
}

ABComp::~ABComp()
{
    aButton.setLookAndFeel (nullptr);
    bButton.setLookAndFeel (nullptr);
    arrowButton.setLookAndFeel (nullptr);
}

void ABComp::refreshStates()
{
    auto selectedState = manager.getCurrentABState();

    if (selectedState == 0)
    {
        aButton.setToggleState (true, dontSendNotification);
        bButton.setToggleState (false, dontSendNotification);
        arrowButton.setButtonText (">");
        arrowButton.setTooltip ("Copy the current state into state B");
    }
    else if (selectedState == 1)
    {
        aButton.setToggleState (false, dontSendNotification);
        bButton.setToggleState (true, dontSendNotification);
        arrowButton.setButtonText ("<");
        arrowButton.setTooltip ("Copy the current state into state A");
    }

    repaint();
}

void ABComp::paint (Graphics&)
{
}

void ABComp::resized()
{
    const auto thirdWidth = getWidth() / 3;
    aButton.setBounds (0 * thirdWidth, 0, thirdWidth, getHeight());
    arrowButton.setBounds (1 * thirdWidth, 0, thirdWidth, getHeight());
    bButton.setBounds (2 * thirdWidth, 0, thirdWidth, getHeight());
}
