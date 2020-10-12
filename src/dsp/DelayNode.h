#pragma once

#include "BaseNode.h"
#include "DelayProc.h"

namespace DelayConsts
{
    constexpr float maxDelay = 500.0f;
}

class DelayNode : public BaseNode<DelayNode>
{
public:
    DelayNode();

    float getDelayMs() const noexcept { return delayMs->get(); }
    void setDelay (float newDelayMs) { *delayMs = newDelayMs; }

    float getPan() const noexcept { return pan->get(); }
    void setPan (float newPan) { *pan = newPan; }

    void prepare (double sampleRate, int samplesPerBlock) override;
    void process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples) override;

    std::unique_ptr<NodeComponent> createEditor (GraphView*) override;

    int getNumParams() const noexcept { return params.size(); }
    AudioParameterFloat* getParam (int idx) { return params[idx]; }

private:
    OwnedArray<AudioParameterFloat> params;

    AudioParameterFloat* delayMs = nullptr;
    AudioParameterFloat* pan = nullptr;

    float fs = 44100.0f;

    enum
    {
        delayIdx,
    };

    dsp::ProcessorChain<DelayProc> processors;

    AudioBuffer<float> childBuffer;
    AudioBuffer<float> panBuffer;
    dsp::Panner<float> panner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNode)
};

using DBaseNode = BaseNode<DelayNode>;
