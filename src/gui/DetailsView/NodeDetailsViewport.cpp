#include "NodeDetailsViewport.h"
#include "NodeDetailsGUI.h"

namespace
{
    constexpr int scrollThickness = 4;
}

NodeDetailsViewport::NodeDetailsViewport (ChowMatrix& chowMatrix) :
    detailsComp (chowMatrix)
{
    setViewedComponent (&detailsComp, false);
    setScrollBarsShown (true, true);
    setScrollBarThickness (scrollThickness);
}

void NodeDetailsViewport::resized()
{
    detailsComp.setSize (detailsComp.getWidth(), getHeight());
    detailsComp.setMinWidth (getWidth() - scrollThickness);
}

void NodeDetailsViewport::paint (Graphics&)
{
    auto setScrollBarColours = [=] (ScrollBar& scrollbar) {
        scrollbar.setColour (ScrollBar::thumbColourId,
            findColour (NodeDetailsGUI::scrollThumbColour, true));

        scrollbar.setColour (ScrollBar::trackColourId,
            findColour (NodeDetailsGUI::scrollTrackColour, true));
    };

    setScrollBarColours (getHorizontalScrollBar());
    setScrollBarColours (getVerticalScrollBar());
}

void NodeDetailsViewport::visibleAreaChanged (const Rectangle<int>&)
{
    getParentComponent()->resized();
}
