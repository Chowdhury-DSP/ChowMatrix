#pragma once

#include <pch.h>

namespace ParamTags
{
const String delayTag = "DLY";
const String panTag = "PAN";
const String fbTag = "FDBK";
const String gainTag = "GAIN";
const String lpfTag = "LPF";
const String hpfTag = "HPF";
const String distTag = "DIST";
const String pitchTag = "PITCH";
const String diffTag = "DIFF";
const String revTag = "REV";
const String modFreqTag = "MOD_FREQ";
const String delayModTag = "MOD_DELAY";
const String panModTag = "MOD_PAN";
} // namespace ParamTags

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
constexpr float maxPitch = 12.0f;
constexpr float minModFreq = 0.0f;
constexpr float maxModFreq = 5.0f;
constexpr int numParams = 13;

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

String pitchValToString (float pitchVal);
float stringToPitchVal (const String& s);

String freqValToString (float freqVal);
float stringToFreqVal (const String& s);

String percentValToString (float percentVal);
float stringToPercentVal (const String& s);

using StringToValFunc = float (*) (const String&);
StringToValFunc getStringFuncForParam (const String& paramID);

/** Creates a tooltip for a given paramID */
String getTooltip (const String& paramID);

/** Gets a parameter name for a given parameter ID */
String getName (const String& paramID);

/** Gets the parameter ID for a given index */
String getParamID (int index);

/** Loads parameters from a comma-separated string into a StringArray */
void loadStringArray (StringArray& array, String string);

} // namespace ParamHelpers
