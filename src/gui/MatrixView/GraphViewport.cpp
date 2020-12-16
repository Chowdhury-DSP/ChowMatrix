#include "GraphViewport.h"

namespace
{
    constexpr int maxDim = 1500;
    constexpr int buttonDim = 20;
}

GraphViewport::GraphViewport (ChowMatrix& plugin) :
    graphView (plugin, *this),
    aurora (plugin.getInsanityParam()),
    manager (plugin.getManager()),
    homeButton ("", DrawableButton::ImageStretched)
{
    setViewedComponent (&graphView, false);
    addAndMakeVisible (aurora);

    graphView.setBounds (0, 0, maxDim, maxDim);
    setScrollBarsShown (false, false, true, true);

    manager.addListener (this);
    setupHomeButton();
}

GraphViewport::~GraphViewport()
{
    manager.removeListener (this);
}

void GraphViewport::setupHomeButton()
{
    homeButton.setColour (DrawableButton::backgroundColourId, Colours::transparentBlack);
    homeButton.setColour (DrawableButton::backgroundOnColourId, Colours::transparentBlack);

    std::unique_ptr<Drawable> offImage (Drawable::createFromImageData
        (BinaryData::home_svg, BinaryData::home_svgSize));
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
    graphView.parentSizeChanged (getWidth(), getHeight());
    centerView();

    homeButton.setBounds (0, getHeight() - buttonDim, buttonDim, buttonDim);
}

void GraphViewport::mouseDrag (const MouseEvent& e)
{
    const auto myE = e.getEventRelativeTo (this);
    autoScroll (e.x - getViewPositionX(), e.y - getViewPositionY(), 50, 5);
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
    int x = (maxDim - getWidth()) / 2;
    int y = maxDim - getHeight();
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

void GraphViewport::nodeParamLockChanged (DelayNode* node)
{
    for (auto* comp : graphView.getDelayNodeComps())
        if (&comp->getNode() == node)
            comp->getNodeInfo().repaint();
}
