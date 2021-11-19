#include "GraphViewport.h"

namespace
{
/** Max dimensions for the GraphView internal component */
constexpr int maxDimX = 2500;
constexpr int maxDimY = 1200;

/** Will stop scrolling when node is this distance from edge */
constexpr int scrollDistanceFromEdge = 30;

/** Speed of autoscrolling */
constexpr int scrollSpeed = 2;
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
#endif
}

GraphViewport::~GraphViewport()
{
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
    homeButton.onClick = std::bind (&GraphViewport::centerView, this);
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
