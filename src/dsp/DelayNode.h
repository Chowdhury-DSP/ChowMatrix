#pragma once

#include "BaseNode.h"
#include "DelayProc.h"
#include "ProcessorBase.h"
#include "ParamHelpers.h"

class DelayNode : public BaseNode<DelayNode>,
                  private ProcessorBase
{
public:
    DelayNode();

    float getDelay() const noexcept { return delayMs->convertTo0to1 (delayMs->get()); }
    void setDelay (float newDelay) { ParamHelpers::setParameterValue (delayMs, delayMs->convertFrom0to1 (newDelay)); }

    float getPan() const noexcept { return pan->get(); }
    void setPan (float newPan) { ParamHelpers::setParameterValue (pan, newPan); }

    void prepare (double sampleRate, int samplesPerBlock) override;
    void process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples) override;

    std::unique_ptr<NodeComponent> createEditor (GraphView*) override;

    int getNumParams() const noexcept { return paramIDs.size(); }
    String getParamID (int idx) { return paramIDs[idx]; }
    RangedAudioParameter* getNodeParameter (int idx) { return params.getParameter (paramIDs[idx]); }
    RangedAudioParameter* getNodeParameter (const String& id) { return params.getParameter (id); }

    XmlElement* saveXml() override;
    void loadXml (XmlElement*) override;
    void deleteNode();

private:
    void cookParameters();

    AudioProcessorValueTreeState params;
    StringArray paramIDs;

    Parameter* delayMs = nullptr;
    Parameter* pan = nullptr;
    Parameter* feedback = nullptr;
    Parameter* gainDB = nullptr;
    Parameter* lpfHz = nullptr;
    Parameter* hpfHz = nullptr;
    Parameter* distortion = nullptr;

    enum
    {
        gainIdx,
        delayIdx,
    };

    dsp::ProcessorChain<dsp::Gain<float>, DelayProc> processors;

    AudioBuffer<float> childBuffer;
    AudioBuffer<float> panBuffer;
    dsp::Panner<float> panner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNode)
};

using DBaseNode = BaseNode<DelayNode>;
