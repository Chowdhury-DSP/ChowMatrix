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
        0.0f, [] (float val) { return gainValToString (val); },
        [] (const String& s) { return stringToGainVal (s); }));

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

float stringToPanVal (const String& s) { return s.getFloatValue(); }

String fbValToString (float fbVal)
{
    String fbStr = String (int (fbVal * 100.0f));
    return fbStr + "%";
}

float stringToFbVal (const String& s) { return s.getFloatValue(); }

String gainValToString (float gainVal)
{
    String gainStr = String (gainVal, 2, false);
    return gainStr + " dB";
}

float stringToGainVal (const String& s) { return s.getFloatValue(); }

} // ParamHelpers
