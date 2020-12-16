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
    float getDelayWithMod() const noexcept;
    void setDelay (float newDelay) { ParamHelpers::setParameterValue (delayMs, delayMs->convertFrom0to1 (newDelay)); }

    // Sync parameter functions
    void setDelaySync (bool shouldBeSynced);
    bool getDelaySync() const noexcept { return syncDelay; }
    void setTempo (double newTempoBPM) { tempoBPM = newTempoBPM; }

    // Get/Set pan param [-1,1]
    float getPan() const noexcept { return pan->get(); }
    float getPanWithMod() const noexcept { return pan->get() + panModValue; }
    void setPan (float newPan) { ParamHelpers::setParameterValue (pan, newPan); }

    /** Call node listeners to set parameter by a diff value */
    void setParameterDiffListeners (const String& paramID, float diff01);
    
    /** Sets the parameter with given ID by diff to a 0-1 normalized value */
    void setNodeParameterDiff (const String& paramID, float diff01);

    /** Randomise all the parameters of this delay node */
    void randomiseParameters();

    // Manage parameter locking for Insanity Control
    void toggleInsanityLock (const String& paramID);
    bool isParamLocked (const String& paramID) const noexcept;

    /** Sets the delay interpolation type for this node */
    void setDelayType (VariableDelay::DelayType type);
    
    /** Flushes delay buffers for this node */
    void flushDelays();
    void prepare (double sampleRate, int samplesPerBlock) override;
    void process (AudioBuffer<float>& inBuffer, AudioBuffer<float>& outBuffer) override;

    std::unique_ptr<NodeComponent> createNodeEditor (GraphView*) override;

    // Manage parameters
    int getNumParams() const noexcept { return paramIDs.size(); }
    String getParamID (int idx) { return paramIDs[idx]; }
    RangedAudioParameter* getNodeParameter (int idx) { return params.getParameter (paramIDs[idx]); }
    RangedAudioParameter* getNodeParameter (const String& id) { return params.getParameter (id); }

    XmlElement* saveXml() override;
    void loadXml (XmlElement*) override;
    void deleteNode();

    // Manage node index
    void setIndex (int newIdx) { nodeIdx = newIdx; }
    int getIndex() const noexcept {  return nodeIdx; }

    // Manage selection state
    bool getSelected() const noexcept { return isSelected; }
    void setSelected (bool shouldBeSelected);

    // Manage solo state
    enum SoloState
    {
        None,
        Mute,
        Solo,
    };

    SoloState getSoloed() const noexcept { return isSoloed.load(); }
    void setSoloed (SoloState newSoloState);

    // Filters to smooth random param changes from Insanity
    dsp::IIR::Filter<float> delaySmoother;
    dsp::IIR::Filter<float> panSmoother;

private:
    void cookParameters (bool force = false);
    void processPanner (dsp::AudioBlock<float>& inputBlock);
    void addToOutput (AudioBuffer<float>& outBuffer);

    AudioProcessorValueTreeState params;
    StringArray paramIDs;
    Random rand;

    bool syncDelay = false;
    double tempoBPM = 120.0;

    StringArray lockedParams;
    std::atomic<SoloState> isSoloed;
    SoloState prevSoloState = None;

    // parameter handles
    Parameter* delayMs    = nullptr;
    Parameter* pan        = nullptr;
    Parameter* feedback   = nullptr;
    Parameter* gainDB     = nullptr;
    Parameter* lpfHz      = nullptr;
    Parameter* hpfHz      = nullptr;
    Parameter* distortion = nullptr;
    Parameter* modFreq    = nullptr;
    Parameter* delayMod   = nullptr;
    Parameter* panMod     = nullptr;

    enum
    {
        gainIdx,
        delayIdx,
    };

    dsp::ProcessorChain<dsp::Gain<float>, DelayProc> processors;
    AudioBuffer<float> panBuffer;
    chowdsp::Panner<float> panner;
    
    chowdsp::SineWave<float> modSine;
    float panModValue = 0.0f;

    // needed for GUI
    int nodeIdx = 0;
    bool isSelected = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayNode)
};

using DBaseNode = BaseNode<DelayNode>;
