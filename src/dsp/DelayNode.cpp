#include "DelayNode.h"
#include "../gui/MatrixView/DelayNodeComponent.h"
#include "Parameters/ParamHelpers.h"
#include "Delay/DelaySyncUtils.h"

using namespace ParamTags;
using namespace DelaySyncUtils;

DelayNode::DelayNode() :
    params (*this, nullptr, Identifier ("Parameters"), ParamHelpers::createParameterLayout())
{
    auto loadParam = [=] (String paramID) -> Parameter*
    {
        paramIDs.add (paramID);
        return dynamic_cast<Parameter*> (params.getParameter (paramID));
    };

    delayMs    = loadParam (delayTag);
    pan        = loadParam (panTag);
    feedback   = loadParam (fbTag);
    gainDB     = loadParam (gainTag);
    lpfHz      = loadParam (lpfTag);
    hpfHz      = loadParam (hpfTag);
    distortion = loadParam (distTag);

    processors.get<gainIdx>().setRampDurationSeconds (0.05);
    panner.setRule (dsp::PannerRule::squareRoot3dB);

    delaySmoother.reset();
    panSmoother.reset();
    isSoloed = None;
}

void DelayNode::setDelaySync (bool shouldBeSynced)
{
    syncDelay = shouldBeSynced;
    delayMs->sendValueChangedMessageToListeners (*delayMs);
}

void DelayNode::cookParameters (bool force)
{
    float delayLenMs = *delayMs;
    if (syncDelay)
    {
        auto& rhythm = getRhythmForParam (delayMs->convertTo0to1 (delayLenMs));
        delayLenMs = 1000.0f * (float) getDelayForRythm (tempoBPM, rhythm);
    }

    processors.get<gainIdx>().setGainDecibels (*gainDB);
    processors.get<delayIdx>().setParameters ({
        delayLenMs,
        *feedback,
        *lpfHz,
        *hpfHz,
        *distortion
    }, force);

    panner.setPan (*pan);
}

void DelayNode::setParameterListeners (const String& paramID, float value01)
{
    nodeListeners.call (&DBaseNode::Listener::setParameter, this, paramID, value01);
}

void DelayNode::setNodeParameter (const String& paramID, float value01)
{
    auto thisParam = params.getParameter (paramID);
    thisParam->setValueNotifyingHost (value01);
}

void DelayNode::randomiseParameters()
{
    for (auto& paramID : paramIDs)
    {
        auto* param = params.getParameter (paramID);
        param->setValueNotifyingHost (rand.nextFloat());
    }
}

void DelayNode::toggleInsanityLock (const String& paramID)
{
    if (lockedParams.contains (paramID))
        lockedParams.removeString (paramID);
    else
        lockedParams.addIfNotAlreadyThere (paramID);

    repaintEditors();
}

bool DelayNode::isParamLocked (const String& paramID) const noexcept
{
    return lockedParams.contains (paramID);
}

void DelayNode::setDelayType (VariableDelay::DelayType type)
{
    processors.get<delayIdx>().setDelayType (type);
}

void DelayNode::prepare (double newSampleRate, int newSamplesPerBlock)
{
    DBaseNode::prepare (newSampleRate, newSamplesPerBlock);

    processors.prepare ({ newSampleRate, (uint32) newSamplesPerBlock, 1 });
    panner.prepare ({ newSampleRate, (uint32) newSamplesPerBlock, 2 });
    cookParameters (true);
    
    childBuffer.setSize (1, newSamplesPerBlock);
    panBuffer.setSize (2, newSamplesPerBlock);
}

void DelayNode::flushDelays()
{
    processors.get<delayIdx>().flushDelay();
}

void DelayNode::process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples)
{
    // process through node delay processors
    dsp::AudioBlock<float> inBlock { inBuffer };
    cookParameters();
    processors.process<dsp::ProcessContextReplacing<float>> ({ inBlock });

    // process through children
    for (auto* child : children)
    {
        childBuffer.makeCopyOf (inBuffer, true);
        child->process (childBuffer, outBuffer, numSamples);
    }

    // apply pan
    dsp::AudioBlock<float> panBlock { panBuffer };
    panner.process<dsp::ProcessContextNonReplacing<float>> ({ inBlock, panBlock });

    addToOutput (outBuffer, numSamples);
}

void DelayNode::addToOutput (AudioBuffer<float>& outBuffer, const int numSamples)
{
    if (isSoloed == Mute && prevSoloState == Mute) // mute state
        return;

    if (isSoloed != Mute && prevSoloState != Mute) // normal state
    {
        for (int ch = 0; ch < outBuffer.getNumChannels(); ++ch)
            outBuffer.addFrom (ch, 0, panBuffer, ch, 0, numSamples);
        return;  
    }

    int fadeSamples = jmin (numSamples, 256);
    if (isSoloed == Mute && prevSoloState != Mute) // fade-out state
    {
        for (int ch = 0; ch < outBuffer.getNumChannels(); ++ch)
            outBuffer.addFromWithRamp (ch, 0, panBuffer.getWritePointer (ch), fadeSamples, 1.0f, 0.0f);
    }
    else if (isSoloed != Mute && prevSoloState == Mute) // fade-in state
    {
        for (int ch = 0; ch < outBuffer.getNumChannels(); ++ch)
        {
            outBuffer.addFromWithRamp (ch, 0, panBuffer.getWritePointer (ch), fadeSamples, 0.0f, 1.0f);

            if (numSamples > fadeSamples)
                outBuffer.addFrom (ch, fadeSamples, panBuffer, ch, fadeSamples, numSamples - fadeSamples);
        }
    }
    
    prevSoloState = isSoloed;
}

std::unique_ptr<NodeComponent> DelayNode::createNodeEditor (GraphView* view)
{
    auto editorPtr = std::make_unique<DelayNodeComponent> (*this, view);
    editor = editorPtr.get();
    return std::move (editorPtr);
}

XmlElement* DelayNode::saveXml()
{
    auto state = params.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    xml->addChildElement (DBaseNode::saveXml());
    xml->setAttribute ("locked", lockedParams.joinIntoString (",") + ",");

    return xml.release();
}

void DelayNode::loadXml (XmlElement* xmlState)
{
    if (xmlState == nullptr)
        return;

    if (xmlState->hasTagName (params.state.getType()))
    {
        params.replaceState (ValueTree::fromXml (*xmlState));
        DBaseNode::loadXml (xmlState->getChildByName ("children"));

        lockedParams.clear();
        auto lockedParamsString = xmlState->getStringAttribute ("locked", String());
        while (lockedParamsString.isNotEmpty())
        {
            auto splitIdx = lockedParamsString.indexOfChar (',');
            if (splitIdx <= 0)
                break;

            lockedParams.add (lockedParamsString.substring (0, splitIdx));
            lockedParamsString = lockedParamsString.substring (splitIdx + 1);
        }
    }

    repaintEditors();
}

void DelayNode::deleteNode()
{
    clearChildren();
    parent->removeChild (this);
    nodeListeners.call (&DBaseNode::Listener::nodeRemoved, this);
}

void DelayNode::setSelected (bool shouldBeSelected)
{ 
    isSelected = shouldBeSelected;

    if (auto edCast = dynamic_cast<DelayNodeComponent*> (editor))
        edCast->selectionChanged();

    repaintEditors();
}

void DelayNode::setSoloed (SoloState newSoloState)
{
    isSoloed = newSoloState;
    repaintEditors (true);
}

void DelayNode::repaintEditors (bool repaintWholeGraph)
{
    if (auto edCast = dynamic_cast<DelayNodeComponent*> (editor))
    {
        edCast->repaint();

        if (repaintWholeGraph)
            edCast->getParentComponent()->repaint();
    }

    if (nodeDetails)
        nodeDetails->repaint();
}
