#include "ABComp.h"

ABComp::ABComp (StateManager& manager) :
    manager (manager)
{
    auto setupButton = [=, &manager] (TextButton& button, String name, String text,
                            String tooltip, int index) {
        button.setColour (TextButton::buttonColourId, Colours::transparentBlack);
        button.setColour (TextButton::buttonOnColourId, Colour (0xFFC954D4).brighter (0.2f));

        button.setName (name);
        button.setButtonText (text);
        button.setTooltip (tooltip);
        button.setClickingTogglesState (false);

        button.setLookAndFeel (&lnf);
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
    aButton.setBounds     (0 * thirdWidth, 0, thirdWidth, getHeight());
    arrowButton.setBounds (1 * thirdWidth, 0, thirdWidth, getHeight());
    bButton.setBounds     (2 * thirdWidth, 0, thirdWidth, getHeight());
}
