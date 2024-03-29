#include "NodeDetailsViewport.h"
#include "NodeDetailsGUI.h"

namespace
{
#if ! JUCE_IOS
constexpr int scrollThickness = 8;
#else
constexpr int scrollThickness = 12;
#endif
} // namespace

NodeDetailsViewport::NodeDetailsViewport (ChowMatrix& chowMatrix) : manager (chowMatrix.getManager()),
                                                                    detailsComp (chowMatrix)
{
    setViewedComponent (&detailsComp, false);
    setScrollBarsShown (true, true);
    setScrollBarThickness (scrollThickness);

    manager.addListener (this);

#if JUCE_IOS
    setScrollOnDragEnabled (false);
    dragToScrollListener = std::make_unique<TwoFingerDragToScrollListener> (*this);
#endif
}

NodeDetailsViewport::~NodeDetailsViewport()
{
#if JUCE_IOS
    dragToScrollListener.reset();
#endif

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
void NodeDetailsViewport::nodeSelected (DelayNode* selectedNode, NodeManager::ActionSource source)
{
    detailsComp.repaint();

    if (selectedNode == nullptr // No node selected!
        || source == NodeManager::ActionSource::DetailsView) // Selection came from here!
        return;

    int xOffset = (NodeInfoConsts::InfoWidthNoLabel - getWidth()) / 2 + scrollThickness; // offset for center of component
    auto position = detailsComp.getNodeDetailsPosition (selectedNode);
    position.addXY (xOffset, 0);
    setViewPosition (position);
}

void NodeDetailsViewport::nodeSoloed (DelayNode* /*soloedNode*/, NodeManager::ActionSource /*source*/)
{
    detailsComp.repaint();
}

void NodeDetailsViewport::nodeInsanityLockChanged (DelayNode* node)
{
    detailsComp.getNodeDetails (node)->repaint();
}
