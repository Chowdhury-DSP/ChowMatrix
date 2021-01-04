#pragma once

#include <pch.h>

#define DIFFUSE_FREQ 0

namespace ParamTags
{
    const String delayTag = "DLY";
    const String panTag   = "PAN";
    const String fbTag    = "FDBK";
    const String gainTag  = "GAIN";
    const String lpfTag   = "LPF";
    const String hpfTag   = "HPF";
    const String distTag  = "DIST";
    const String pitchTag = "PITCH";
    const String diffTag  = "DIFF";
#if DIFFUSE_FREQ
    const String diffFreqTag = "DIFF_FREQ";
#endif
    const String modFreqTag   = "MOD_FREQ";
    const String delayModTag  = "MOD_DELAY";
    const String panModTag    = "MOD_PAN";
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
#if DIFFUSE_FREQ
constexpr float minDiffFreq = 10.0f;
constexpr float maxDiffFreq = 1000.0f;
#endif
constexpr float maxPitch = 12.0f;
constexpr float minModFreq = 0.0f;
constexpr float maxModFreq = 5.0f;
#if DIFFUSE_FREQ
constexpr int numParams = 13;
#else
constexpr int numParams = 12;
#endif

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

} // ParamHelpers
