#include "ParamHelpers.h"

using namespace ParamTags;

namespace ParamHelpers
{

void setParameterValue (Parameter* param, float newVal)
{
    static_cast<AudioParameterFloat&> (*param) = newVal;
}

using Params = std::vector<std::unique_ptr<RangedAudioParameter>>;
AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    Params params;

    // set up delay line length
    NormalisableRange<float> delayRange { 0.0f, maxDelay };
    delayRange.setSkewForCentre (50.0f);

    params.push_back (std::make_unique<Parameter> (delayTag, "Delay", String(), delayRange,
        50.0f, [] (float val) { return delayValToString (val); },
        [] (const String& s) { return stringToDelayVal (s); }));

    // set up panner
    NormalisableRange<float> panRange { -1.0f, 1.0f };
    params.push_back (std::make_unique<Parameter> (panTag, "Pan", String(), panRange,
        0.0f, [] (float val) { return panValToString (val); },
        [] (const String& s) { return stringToPanVal (s); }));

    // set up feedback amount
    NormalisableRange<float> fbRange { 0.0f, maxFeedback };
    params.push_back (std::make_unique<Parameter> (fbTag, "Feedback", String(), fbRange,
        0.0f, [] (float val) { return fbValToString (val); },
        [] (const String& s) { return stringToFbVal (s); }));

    // set up gain
    NormalisableRange<float> gainRange { -maxGain, maxGain };
    params.push_back (std::make_unique<Parameter> (gainTag, "Gain", String(), gainRange,
        0.0f, &gainValToString, &stringToGainVal));

    // set up LPF
    NormalisableRange<float> lpfRange { minLPF, maxLPF };
    lpfRange.setSkewForCentre (std::sqrt (minLPF * maxLPF));
    params.push_back (std::make_unique<Parameter> (lpfTag, "LPF", String(), lpfRange,
        maxLPF, &freqValToString, &stringToFreqVal));

    // set up HPF
    NormalisableRange<float> hpfRange { minHPF, maxHPF };
    hpfRange.setSkewForCentre (std::sqrt (minHPF * maxHPF));
    params.push_back (std::make_unique<Parameter> (hpfTag, "HPF", String(), hpfRange,
        minHPF, &freqValToString, &stringToFreqVal));

    // set up distortion
    NormalisableRange<float> distRange { 0.0f, 1.0f };
    params.push_back (std::make_unique<Parameter> (distTag, "Distortion", String(), distRange,
        0.0f, &distValToString, &stringToDistVal));

    return { params.begin(), params.end() };
}

String delayValToString (float delayVal)
{
    String delayStr = String (delayVal, 2, false);
    return delayStr + " ms";
}

float stringToDelayVal (const String& s) { return s.getFloatValue(); }

String panValToString (float panVal)
{
    String panChar = "";
    if (panVal > 0.0f)
        panChar = "R";
    else if (panVal < 0.0f)
        panChar = "L";
    
    String panStr = String (int (panVal * 50.0f));
    return panStr + panChar;
}

float stringToPanVal (const String& s) { return s.getFloatValue() / 50.0f; }

String fbValToString (float fbVal)
{
    String fbStr = String (int (fbVal * 100.0f));
    return fbStr + "%";
}

float stringToFbVal (const String& s) { return s.getFloatValue() / 100.0f; }

String gainValToString (float gainVal)
{
    String gainStr = String (gainVal, 2, false);
    return gainStr + " dB";
}

float stringToGainVal (const String& s) { return s.getFloatValue(); }

String freqValToString (float freqVal)
{
    if (freqVal < 1000.0f)
        return String (freqVal, 2, false) + " Hz";

    return String (freqVal / 1000.0f, 2, false) + " kHz";
}

float stringToFreqVal (const String& s)
{
    auto freqVal = s.getFloatValue();
    
    if (s.getLastCharacter() == 'k')
        freqVal *= 1000.0f;

    return freqVal;
}

String distValToString (float distVal)
{
    String distStr = String (int (distVal * 100.0f));
    return distStr + "%";
}

float stringToDistVal (const String& s) { return s.getFloatValue() / 100.0f; }

// Map to connect paramIDs to stringToVal functions
std::unordered_map<String, StringToValFunc> funcMap {
    { delayTag, stringToDelayVal },
    { panTag,   stringToPanVal },
    { fbTag,    stringToFbVal },
    { lpfTag,   stringToFreqVal },
    { hpfTag,   stringToFreqVal },
    { distTag,  stringToDistVal },
};

StringToValFunc getStringFuncForParam (const String& paramID)
{
    return funcMap[paramID];
}

String getTooltip (const String& paramID)
{
    if (paramID == delayTag)
        return "Sets the length of the delay between the previous delay node and this one.";

    if (paramID == panTag)
        return "Sets the stereo image for this delay node.";

    if (paramID == fbTag)
        return "Sets the feedback gain for this delay line";

    if (paramID == gainTag)
        return "Sets the gain for this delay node";

    if (paramID == lpfTag)
        return "Sets the cutoff frequency of a lowpass filter on this delay node";

    if (paramID == hpfTag)
        return "Sets the cutoff frequency of a highpass filter on this delay node";

    if (paramID == distTag)
        return "Sets the amount of distortion on this delay node";

    return {};
}

} // ParamHelpers
