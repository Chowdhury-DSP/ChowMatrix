#include "GraphViewport.h"
#include "ChowMatrix.h"

namespace
{
/** Max dimensions for the GraphView internal component */
constexpr int maxDimX = 1500;
constexpr int maxDimY = 800;

/** Will stop scrolling when node is this distance from edge */
constexpr int scrollDistanceFromEdge = 30;

/** Speed of autoscrolling */
constexpr int scrollSpeed = 1;
} // namespace

GraphViewport::GraphViewport (ChowMatrix& plugin) : graphView (plugin, *this),
                                                    aurora (plugin.getInsanityParam()),
                                                    manager (plugin.getManager()),
                                                    homeButton ("", DrawableButton::ImageStretched)
{
    setViewedComponent (&graphView, false);
    addAndMakeVisible (aurora);

    graphView.setBounds (0, 0, maxDimX, maxDimY);
    setScrollBarsShown (false, false, true, true);

    manager.addListener (this);
    setupHomeButton();

#if JUCE_IOS
    setScrollOnDragEnabled (false);
    dragToScrollListener = std::make_unique<TwoFingerDragToScrollListener> (*this);
    startTimer (250);
#endif
}

GraphViewport::~GraphViewport()
{
    stopTimer();

#if JUCE_IOS
    dragToScrollListener.reset();
#endif

    manager.removeListener (this);
}

void GraphViewport::setupHomeButton()
{
    homeButton.setColour (DrawableButton::backgroundColourId, Colours::transparentBlack);
    homeButton.setColour (DrawableButton::backgroundOnColourId, Colours::transparentBlack);

    std::unique_ptr<Drawable> offImage (Drawable::createFromImageData (BinaryData::home_svg, BinaryData::home_svgSize));
    auto downImage = offImage->createCopy();

    offImage->replaceColour (Colours::black, Colours::white.withAlpha (0.33f));
    downImage->replaceColour (Colours::black, Colours::white);
    homeButton.setImages (offImage.get(), downImage.get(), downImage.get());

    addAndMakeVisible (homeButton);
    homeButton.onClick = [=] { centerView(); };
}

void GraphViewport::timerCallback()
{
#if JUCE_IOS
    if (firstTouch)
    {
        stopTimer();
        return;
    }

    // for some reason, the AUv3 UI opens with the GUI not centered
    // this is a workaround that forces it to re-center every timerCallback
    // until the first touch occurs
    centerView();
#endif
}

void GraphViewport::resized()
{
    aurora.setBounds (getLocalBounds());
    graphView.updateParentSize (getWidth(), getHeight());
    centerView();

    const int buttonDim = jmax (proportionOfWidth (0.025f), 20);
    homeButton.setBounds (0, getHeight() - buttonDim, buttonDim, buttonDim);
}

void GraphViewport::mouseDrag (const MouseEvent& e)
{
#if JUCE_IOS
    firstTouch = true;
#endif

    autoScroll (e.x - getViewPositionX(), e.y - getViewPositionY(), scrollDistanceFromEdge, scrollSpeed);
    graphView.repaint();
}

void GraphViewport::centerView()
{
    if (auto selectedNode = manager.getSelected())
    {
        if (auto centreNodeEditor = selectedNode->getEditor())
        {
            auto pos = centreNodeEditor->getPosition()
                           .translated (-getWidth() / 2, -getHeight() / 2);
            setViewPosition (pos);
            return;
        }
    }

    // no node selected
    int x = (maxDimX - getWidth()) / 2;
    int y = maxDimY - getHeight();
    setViewPosition (x, y);
}

void GraphViewport::nodeSelected (DelayNode* selectedNode, NodeManager::ActionSource source)
{
#if JUCE_IOS
    firstTouch = true;
#endif

    for (auto* comp : graphView.getDelayNodeComps())
        comp->selectionChanged();

    if (selectedNode != nullptr && source != NodeManager::ActionSource::GraphView)
        centerView();

    graphView.repaint();
}

void GraphViewport::nodeSoloed (DelayNode* /*soloedNode*/, NodeManager::ActionSource /*source*/)
{
    graphView.repaint();
}

void GraphViewport::nodeInsanityLockChanged (DelayNode* node)
{
    for (auto* comp : graphView.getDelayNodeComps())
        if (&comp->getNode() == node)
            comp->getNodeInfo().repaint();
}
