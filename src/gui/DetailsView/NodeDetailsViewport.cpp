#include "NodeDetailsViewport.h"
#include "NodeDetailsGUI.h"

NodeDetailsViewport::NodeDetailsViewport (ChowMatrix& chowMatrix) :
    detailsComp (chowMatrix)
{
    setViewedComponent (&detailsComp, false);
    setScrollBarsShown (false, true);
    setScrollBarThickness (4);
}

void NodeDetailsViewport::resized()
{
    detailsComp.setSize (detailsComp.getWidth(), getHeight());
    detailsComp.setMinWidth (getWidth());
}

void NodeDetailsViewport::paint (Graphics&)
{
    auto& scrollbar = getHorizontalScrollBar();
    scrollbar.setColour (ScrollBar::thumbColourId,
        findColour (NodeDetailsGUI::scrollThumbColour, true));
    
    scrollbar.setColour (ScrollBar::trackColourId,
        findColour (NodeDetailsGUI::scrollTrackColour, true));
}
