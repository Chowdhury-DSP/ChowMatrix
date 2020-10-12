#include "DelayNode.h"
#include "../gui/DelayNodeComponent.h"

using namespace DelayConsts;

DelayNode::DelayNode()
{
    NormalisableRange<float> delayRange { 0.0f, maxDelay };
    delayMs = params.add (std::make_unique<AudioParameterFloat> ("DLY", "Delay", delayRange, 50.0f));

    NormalisableRange<float> panRange { -1.0f, 1.0f };
    pan = params.add (std::make_unique<AudioParameterFloat> ("PAN", "Pan", panRange, 0.0f));

    panner.setRule (dsp::PannerRule::squareRoot3dB);
}

void DelayNode::prepare (double sampleRate, int samplesPerBlock)
{
    DBaseNode::prepare (sampleRate, samplesPerBlock);

    processors.prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
    panner.prepare ({ sampleRate, (uint32) samplesPerBlock, 2 });
    
    childBuffer.setSize (1, samplesPerBlock);
    panBuffer.setSize (2, samplesPerBlock);
}

void DelayNode::process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples)
{
    dsp::AudioBlock<float> inBlock { inBuffer };
    processors.get<delayIdx>().setDelay (*delayMs);
    processors.process<dsp::ProcessContextReplacing<float>> ({ inBlock });

    for (auto* child : children)
    {
        childBuffer.makeCopyOf (inBuffer, true);
        child->process (childBuffer, outBuffer, numSamples);
    }

    panner.setPan (*pan);
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
