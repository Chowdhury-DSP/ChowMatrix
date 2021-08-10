#include "DelayNode.h"
#include "../gui/MatrixView/DelayNodeComponent.h"
#include "Delay/TempoSyncUtils.h"
#include "Parameters/ParamHelpers.h"

using namespace ParamTags;
using namespace TempoSyncUtils;

DelayNode::DelayNode() : params (*this, nullptr, Identifier ("Parameters"), ParamHelpers::createParameterLayout()),
                         insanityLockHelper ([=] { nodeListeners.call (&Listener::nodeInsanityLockChanged, this); })
{
    auto loadParam = [=] (String paramID) -> Parameter* {
        paramIDs.add (paramID);
        return dynamic_cast<Parameter*> (params.getParameter (paramID));
    };

    delayMs = loadParam (delayTag);
    pan = loadParam (panTag);
    feedback = loadParam (fbTag);
    gainDB = loadParam (gainTag);
    lpfHz = loadParam (lpfTag);
    hpfHz = loadParam (hpfTag);
    pitchSt = loadParam (pitchTag);
    diffAmt = loadParam (diffTag);
    distortion = loadParam (distTag);
    revTimeMs = loadParam (revTag);
    modFreq = loadParam (modFreqTag);
    delayMod = loadParam (delayModTag);
    panMod = loadParam (panModTag);

    processors.get<gainIdx>().setRampDurationSeconds (0.05);
    panner.setRule (chowdsp::Panner<float>::Rule::squareRoot3dB);

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
        delayLenMs = 1000.0f * (float) getTimeForRythm (tempoBPM, rhythm);
    }

    processors.get<gainIdx>().setGainDecibels (*gainDB);
    processors.get<delayIdx>().setParameters ({ delayLenMs,
                                                *feedback,
                                                *lpfHz,
                                                *hpfHz,
                                                *distortion,
                                                *pitchSt,
                                                *diffAmt,
                                                *revTimeMs,
                                                modFreq,
                                                *delayMod,
                                                (float) tempoBPM,
                                                tempoSyncedLFO,
                                                getPlayHead() },
                                              force);

    if (tempoSyncedLFO)
        modSine.setFreqSynced (modFreq, (float) tempoBPM);
    else
        modSine.setFrequency (*modFreq);
    panner.setPan (*pan);
}

void DelayNode::setParameterDiffListeners (const String& paramID, float diff01)
{
    nodeListeners.call (&DBaseNode::Listener::setParameterDiff, this, paramID, diff01);
}

void DelayNode::setParameterDefaultListeners (const String& paramID)
{
    nodeListeners.call (&DBaseNode::Listener::setParameterDefault, this, paramID);
}

void DelayNode::setNodeParameterDiff (const String& paramID, float diff01)
{
    auto thisParam = params.getParameter (paramID);
    auto curValue = thisParam->getValue();
    auto newValue = jlimit (0.0f, 1.0f, curValue + diff01);
    thisParam->setValueNotifyingHost (newValue);
}

void DelayNode::setNodeParameter (const String& paramID, float value01)
{
    params.getParameter (paramID)->setValueNotifyingHost (value01);
}

void DelayNode::setNodeParameterToDefault (const String& paramID)
{
    auto* param = params.getParameter (paramID);
    param->setValueNotifyingHost (param->getDefaultValue());
}

void DelayNode::randomiseParameters()
{
    for (auto& paramID : paramIDs)
    {
        if (randLockHelper.isParamLocked (paramID))
            continue;

        auto* param = params.getParameter (paramID);
        param->setValueNotifyingHost (rand.nextFloat());
    }
}

PopupMenu DelayNode::createParamPopupMenu (const String& paramID)
{
    PopupMenu menu;
    nodeListeners.call (&Listener::addParameterMenus, menu, paramID, this);

    if (paramID == ParamTags::delayTag || paramID == ParamTags::panTag)
        insanityLockHelper.createPopupMenu (menu, paramID);

    randLockHelper.createPopupMenu (menu, paramID);

    if (paramID == ParamTags::modFreqTag)
    {
        PopupMenu::Item item ("LFO Sync");
        item.itemID = 1;
        item.setColour (Colour (tempoSyncedLFO ? 0xFF21CCA5 : 0xFFFFFFFF));
        item.action = [=] { toggleLFOSync(); };
        menu.addItem (item);
    }

    return menu;
}

void DelayNode::toggleLFOSync()
{
    tempoSyncedLFO = ! tempoSyncedLFO;
    modFreq->sendValueChangedMessageToListeners (*modFreq);
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
    modSine.prepare ({ newSampleRate, (uint32) newSamplesPerBlock, 1 });
    cookParameters (true);

    panBuffer.setSize (2, newSamplesPerBlock);
}

void DelayNode::flushDelays()
{
    processors.get<delayIdx>().flushDelay();
}

float DelayNode::getDelayWithMod() const noexcept
{
    return delayMs->convertTo0to1 (delayMs->get() + processors.get<delayIdx>().getModDepth());
}

void DelayNode::process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer)
{
    // process through node delay processors
    dsp::AudioBlock<float> inBlock { inBuffer };
    cookParameters();
    processors.process<dsp::ProcessContextReplacing<float>> ({ inBlock });

    DBaseNode::process (inBuffer, outBuffer); // process through children
    processPanner (inBlock); // apply pan
    addToOutput (outBuffer);
}

void DelayNode::processPanner (dsp::AudioBlock<float>& inputBlock)
{
    if (*panMod == 0.0f || *modFreq == 0.0f) // no modulation needed
    {
        modSine.reset();
        panModValue = 0.0f;
        panBuffer.setSize (2, (int) inputBlock.getNumSamples(), false, false, true);
        dsp::AudioBlock<float> panBlock { panBuffer };
        panner.process<dsp::ProcessContextNonReplacing<float>> ({ inputBlock, panBlock });
    }
    else
    {
        auto* x = inputBlock.getChannelPointer (0);
        auto* left = panBuffer.getWritePointer (0);
        auto* right = panBuffer.getWritePointer (1);
        modSine.setPlayHead (getPlayHead());

        for (size_t i = 0; i < inputBlock.getNumSamples(); ++i)
        {
            panModValue = 0.33f * *panMod * modSine.processSample(); // max pan mod = 33%
            panner.setPan (jlimit (-1.0f, 1.0f, *pan + panModValue));
            std::tie (left[i], right[i]) = panner.processSample (x[i]);
        }
    }
}

void DelayNode::addToOutput (AudioBuffer<float>& outBuffer)
{
    const int numSamples = outBuffer.getNumSamples();

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
    std::unique_ptr<XmlElement> xml = std::make_unique<XmlElement> ("delay_node");

    auto state = params.copyState();
    std::unique_ptr<XmlElement> xmlState (state.createXml());
    insanityLockHelper.saveState (xmlState.get());
    randLockHelper.saveState (xmlState.get());
    xmlState->setAttribute ("lfo_sync", tempoSyncedLFO);
    xml->addChildElement (xmlState.release());
    nodeListeners.call (&Listener::saveExtraNodeState, xml.get(), this);
    xml->addChildElement (DBaseNode::saveXml());

    return xml.release();
}

void DelayNode::loadXml (XmlElement* xml)
{
    if (xml == nullptr)
        return;

    nodeListeners.call (&Listener::loadExtraNodeState, xml, this);

    if (auto* xmlState = xml->getChildByName (params.state.getType()))
    {
        params.replaceState (ValueTree::fromXml (*xmlState));
        insanityLockHelper.loadState (xmlState);
        randLockHelper.loadState (xmlState);
        tempoSyncedLFO = xmlState->getBoolAttribute ("lfo_sync");
    }

    // refresh gui
    delayMs->sendValueChangedMessageToListeners (*delayMs);
    pan->sendValueChangedMessageToListeners (*pan);
    modFreq->sendValueChangedMessageToListeners (*modFreq);

    if (auto* childrenXml = xml->getChildByName ("children"))
        DBaseNode::loadXml (childrenXml);
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
}

void DelayNode::setSoloed (SoloState newSoloState)
{
    isSoloed = newSoloState;
}

void DelayNode::beginParameterChange (const StringArray& IDs)
{
    nodeListeners.call (&Listener::beginParameterChange, IDs, this);
}

void DelayNode::endParameterChange (const StringArray& IDs)
{
    nodeListeners.call (&Listener::endParameterChange, IDs, this);
}

void DelayNode::applyParameterChange (const String& paramID, float value01)
{
    nodeListeners.call (&Listener::applyParameterChange, paramID, this, value01);
}
