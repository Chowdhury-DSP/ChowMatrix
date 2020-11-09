#pragma once

#include <pch.h>

namespace ParamTags
{
    const String delayTag = "DLY";
    const String panTag   = "PAN";
    const String fbTag    = "FDBK";
    const String gainTag  = "GAIN";
    const String lpfTag   = "LPF";
    const String hpfTag   = "HPF";
    const String distTag  = "DIST";
}

using Parameter = AudioProcessorValueTreeState::Parameter;

namespace ParamHelpers
{

// parameter constants
constexpr float maxDelay = 1500.0f;
constexpr float centreDelay = 200.0f;
constexpr float maxFeedback = 0.99f;
constexpr float maxGain = 12.0f;
constexpr float minLPF = 200.0f;
constexpr float maxLPF = 20000.0f;
constexpr float minHPF = 20.0f;
constexpr float maxHPF = 2000.0f;
constexpr int numParams = 7;

/** Sets a parameter value */
void setParameterValue (Parameter* param, float newVal);

/** Creates a parameter layout for a DelayNode */
AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

String delayValToString (float delayVal);
float stringToDelayVal (const String& s);

String panValToString (float panVal);
float stringToPanVal (const String& s);

String fbValToString (float fbVal);
float stringToFbVal (const String& s);

String gainValToString (float gainVal);
float stringToGainVal (const String& s);

String freqValToString (float freqVal);
float stringToFreqVal (const String& s);

String distValToString (float distVal);
float stringToDistVal (const String& s);

using StringToValFunc = float (*) (const String&);
StringToValFunc getStringFuncForParam (const String& paramID);

/** Creates a tooltip for a given paramID */
String getTooltip (const String& paramID);

} // ParamHelpers
