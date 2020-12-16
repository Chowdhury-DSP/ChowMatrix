#include "NodeDetailsViewport.h"
#include "NodeDetailsGUI.h"

namespace
{
    constexpr int scrollThickness = 6;
}

NodeDetailsViewport::NodeDetailsViewport (ChowMatrix& chowMatrix) :
    manager (chowMatrix.getManager()),
    detailsComp (chowMatrix)
{
    setViewedComponent (&detailsComp, false);
    setScrollBarsShown (true, true);
    setScrollBarThickness (scrollThickness);

    manager.addListener (this);
}

NodeDetailsViewport::~NodeDetailsViewport()
{
    manager.removeListener (this);
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

// We use this function to center the details view on the selected node
void NodeDetailsViewport::nodeSelected (DelayNode* selectedNode, NodeManager::SelectionSource source)
{
    
    if (selectedNode == nullptr                                 // No node selected!
     || source == NodeManager::SelectionSource::DetailsView)    // Selection came from here!
        return;

    int xOffset = (NodeInfoConsts::InfoWidthNoLabel - getWidth()) / 2 + scrollThickness; // offset for center of component
    auto position = detailsComp.getNodeDetailsPosition (selectedNode);
    position.addXY (xOffset, 0);
    setViewPosition (position);
}
