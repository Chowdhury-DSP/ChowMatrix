#pragma once

#include "BaseNode.h"
#include "Delay/DelayProc.h"
#include "Parameters/ParamHelpers.h"

/**
 * Class for delay processing node
 */ 
class DelayNode : public BaseNode<DelayNode>,
                  public ProcessorBase
{
public:
    DelayNode();

    // Get/Set delay param [0,1]
    float getDelay() const noexcept { return delayMs->convertTo0to1 (delayMs->get()); }
    void setDelay (float newDelay) { ParamHelpers::setParameterValue (delayMs, delayMs->convertFrom0to1 (newDelay)); }

    // Sync parameter functions
    void setDelaySync (bool shouldBeSynced);
    bool getDelaySync() const noexcept { return syncDelay; }
    void setTempo (double newTempoBPM) { tempoBPM = newTempoBPM; }

    // Get/Set pan param [-1,1]
    float getPan() const noexcept { return pan->get(); }
    void setPan (float newPan) { ParamHelpers::setParameterValue (pan, newPan); }

    /** Call node listeners to set parameter */
    void setParameterListeners (const String& paramID, float value01);
    
    /** Sets the parameter with given ID to a 0-1 normalized value */
    void setParameter (const String& paramID, float value01);

    /** Randomise all the parameters of this delay node */
    void randomiseParameters();

    /** Sets the delay interpolation type for this node */
    void setDelayType (VariableDelay::DelayType type);
    
    /** Flushes delay buffers for this node */
    void flushDelays();
    void prepare (double sampleRate, int samplesPerBlock) override;
    void process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer, const int numSamples) override;

    std::unique_ptr<NodeComponent> createEditor (GraphView*) override;

    // Manage parameters
    int getNumParams() const noexcept { return paramIDs.size(); }
    String getParamID (int idx) { return paramIDs[idx]; }
    RangedAudioParameter* getNodeParameter (int idx) { return params.getParameter (paramIDs[idx]); }
    RangedAudioParameter* getNodeParameter (const String& id) { return params.getParameter (id); }

    XmlElement* saveXml() override;
    void loadXml (XmlElement*) override;
    void deleteNode();

    void setIndex (int newIdx) { nodeIdx = newIdx; }
    int getIndex() const noexcept {  return nodeIdx; }

    bool getSelected() const noexcept { return isSelected; }
    void setSelected (bool shouldBeSelected);
    void setNodeDetails (Component* detailsComp) { nodeDetails = detailsComp; }

    // Filters to smooth random param changes from Insanity
    dsp::IIR::Filter<float> delaySmoother;
    dsp::IIR::Filter<float> panSmoother;

private:
    void cookParameters();

    AudioProcessorValueTreeState params;
    StringArray paramIDs;
    Random rand;

    bool syncDelay = false;
    double tempoBPM = 120.0;

    // parameter handles
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

    // needed for GUI
    int nodeIdx = 0;
    bool isSelected = false;
    Component* nodeDetails;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNode)
};

using DBaseNode = BaseNode<DelayNode>;
