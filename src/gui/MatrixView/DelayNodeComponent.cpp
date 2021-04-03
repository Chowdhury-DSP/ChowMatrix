#include "DelayNodeComponent.h"
#include "GraphViewItem.h"

using namespace ParamTags;

DelayNodeComponent::DelayNodeComponent (DelayNode& node, GraphView* view) : NodeComponent (node, view),
                                                                            node (node),
                                                                            nodeInfo (node)
{
    setWantsKeyboardFocus (true);
    view->addChildComponent (nodeInfo);
    node.getNodeParameter (delayTag)->addListener (this);
    node.getNodeParameter (panTag)->addListener (this);
    node.getNodeParameter (modFreqTag)->addListener (this);

    // set initial timer frequency
    auto* modFreqParam = node.getNodeParameter (modFreqTag);
    auto modFreqValue = modFreqParam->convertFrom0to1 (modFreqParam->getValue());
    updateTimerFreq (modFreqValue);

    setName ("Delay Node");
#if JUCE_MAC
    setTooltip ("Click to select node, drag to move, alt+click to solo, CMD+click to delete");
#else
    setTooltip ("Click to select node, drag to move, alt+click to solo, CTRL+click to delete");
#endif
}

DelayNodeComponent::~DelayNodeComponent()
{
    node.getNodeParameter (delayTag)->removeListener (this);
    node.getNodeParameter (panTag)->removeListener (this);
    node.getNodeParameter (modFreqTag)->removeListener (this);
}

void DelayNodeComponent::mouseDown (const MouseEvent& e)
{
    if (e.mods.isCommandDown())
    {
        node.deleteNode();
        return;
    }

    if (e.mods.isAltDown())
        graphView->setSoloed (&node);

    graphView->setSelected (&node);
    grabKeyboardFocus();
    node.beginParameterChange (ParamTags::delayTag);
    node.beginParameterChange (ParamTags::panTag);
}

void DelayNodeComponent::mouseDrag (const MouseEvent& e)
{
    const auto relE = e.getEventRelativeTo (graphView);
    setCentrePosition (relE.getPosition());
    updateParams();
    updatePosition();
    graphView->mouseDrag (relE);
}

void DelayNodeComponent::mouseUp (const MouseEvent&)
{
    node.endParameterChange (ParamTags::delayTag);
    node.endParameterChange (ParamTags::panTag);
}

bool DelayNodeComponent::keyPressed (const KeyPress& key)
{
    if (key == KeyPress::deleteKey || key == KeyPress::backspaceKey)
    {
        if (node.getSelected())
            node.deleteNode();

        return true;
    }

    return false;
}

void DelayNodeComponent::paint (Graphics& g)
{
    // damp colours for muted nodes
    const auto alphaMult = node.getSoloed() == DelayNode::SoloState::Mute ? 0.4f : 1.0f;

    if (node.getSelected())
    {
        g.setColour (findColour (GraphView::nodeSelectedColour, true)
                         .withMultipliedAlpha (alphaMult));
        g.fillEllipse (getLocalBounds().toFloat());

        g.setColour (Colours::white.withMultipliedAlpha (alphaMult));
        g.drawEllipse (getLocalBounds().toFloat().reduced (1.0f), 2.0f);
    }
    else
    {
        g.setColour (nodeColour.withMultipliedAlpha (alphaMult));
        g.fillEllipse (getLocalBounds().toFloat());
    }

    g.setColour (Colours::white.withMultipliedAlpha (alphaMult));
    g.drawFittedText (String (node.getIndex() + 1), getLocalBounds(), Justification::centred, 1);
}

float DelayNodeComponent::getMaxDist() const noexcept
{
    return (float) graphView->getVisibleHeight() / 2.0f;
}

void DelayNodeComponent::updateParams()
{
    auto parentEditor = node.getParent()->getEditor();
    auto parentPos = parentEditor->getCentrePosition().toFloat();
    auto myPos = getCentrePosition().toFloat();

    auto newDelay = jlimit (0.0f, 1.0f, myPos.getDistanceFrom (parentPos) / getMaxDist());
    node.setDelay (newDelay);

    auto newPan = jlimit (-1.0f, 1.0f, parentPos.getAngleToPoint (myPos) / MathConstants<float>::halfPi);
    node.setPan (newPan);

    node.applyParameterChange (ParamTags::delayTag, newDelay);
    node.applyParameterChange (ParamTags::panTag, newPan * 0.5f + 0.5f);
}

void DelayNodeComponent::updatePosition()
{
    auto parentEditor = node.getParent()->getEditor();
    auto parentPos = parentEditor->getCentrePosition().toFloat();

    float delay = node.getDelayWithMod();
    float pan = node.getPanWithMod();

    float radius = delay * getMaxDist();
    float angle = (pan - 1.0f) * MathConstants<float>::halfPi;
    setCentrePosition (parentPos.translated (radius * std::cos (angle), radius * std::sin (angle)).toInt());

    for (int i = 0; i < node.getNumChildren(); ++i)
        node.getChild (i)->getEditor()->updatePosition();

    constexpr int pad = 5;
    bool showOnRight = (pan >= 0.0f
                        || getPosition().translated (-(nodeInfo.getWidth() + pad), 0).x < 0)
                       && getPosition().translated (nodeInfo.getWidth() + pad, 0).x < graphView->getWidth();

    int x = showOnRight ? getWidth() + pad : -(nodeInfo.getWidth() + pad);
    int y = jmin ((int) parentPos.y - getPosition().translated (0, nodeInfo.getHeight()).y, 0);
    auto infoPos = getPosition().translated (x, y);
    infoPos.setY (jmax (infoPos.y, 0));
    nodeInfo.setTopLeftPosition (infoPos);

    graphView->repaint();
}

void DelayNodeComponent::updateTimerFreq (float modFrequency)
{
    if (modFrequency == 0.0f)
        stopTimer();
    else
        startTimerHz (jmax (24, (int) std::ceil (modFrequency * 5)));
}

void DelayNodeComponent::parameterValueChanged (int idx, float value)
{
    if (node.getParamID (idx) == modFreqTag)
        updateTimerFreq (value);

    MessageManagerLock mml;
    updatePosition();
}

void DelayNodeComponent::timerCallback()
{
    updatePosition();
}

void DelayNodeComponent::selectionChanged()
{
    bool isSelected = node.getSelected();
    nodeInfo.setVisible (isSelected);
    nodeInfo.toFront (true);
}
