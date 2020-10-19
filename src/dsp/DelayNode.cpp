#include "DelayNode.h"
#include "../gui/MatrixView/DelayNodeComponent.h"
#include "ParamHelpers.h"

using namespace ParamTags;

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
}

void DelayNode::cookParameters()
{
    processors.get<gainIdx>().setGainDecibels (*gainDB);
    processors.get<delayIdx>().setParameters ({
        *delayMs,
        *feedback,
        *lpfHz,
        *hpfHz,
        *distortion
    });

    panner.setPan (*pan);
}

void DelayNode::prepare (double sampleRate, int samplesPerBlock)
{
    DBaseNode::prepare (sampleRate, samplesPerBlock);

    cookParameters();
    processors.prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
    panner.prepare ({ sampleRate, (uint32) samplesPerBlock, 2 });
    
    childBuffer.setSize (1, samplesPerBlock);
    panBuffer.setSize (2, samplesPerBlock);
}

void DelayNode::process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples)
{
    dsp::AudioBlock<float> inBlock { inBuffer };
    cookParameters();
    processors.process<dsp::ProcessContextReplacing<float>> ({ inBlock });

    for (auto* child : children)
    {
        childBuffer.makeCopyOf (inBuffer, true);
        child->process (childBuffer, outBuffer, numSamples);
    }

    dsp::AudioBlock<float> panBlock { panBuffer };
    panner.process<dsp::ProcessContextNonReplacing<float>> ({ inBlock, panBlock });

    for (int ch = 0; ch < outBuffer.getNumChannels(); ++ch)
        outBuffer.addFrom (ch, 0, panBuffer, ch, 0, numSamples);
}

std::unique_ptr<NodeComponent> DelayNode::createEditor (GraphView* view)
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
    }
}

void DelayNode::deleteNode()
{
    clearChildren();
    parent->removeChild (this);
    nodeListeners.call (&DBaseNode::Listener::nodeRemoved, this);
}
