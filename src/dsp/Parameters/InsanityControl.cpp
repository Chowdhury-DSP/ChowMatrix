#include "InsanityControl.h"
#include "../../NodeManager.h"

using namespace ParamTags;

namespace
{
    const String insanityTag = "insanity";
    constexpr float smoothFreq = 2.0f;
}

InsanityControl::InsanityControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) :
    BaseController (vts, nodes, { insanityTag })
{
    insanityParam = vts.getRawParameterValue (insanityTag);
    parameterChanged (insanityTag, insanityParam->load());
    startTimerHz (timerFreq);
}

void InsanityControl::addParameters (Parameters& params)
{
    auto insanityToString = [] (float x) { return String (x * 100.0f) + "%"; };
    auto stringToInsanity = [] (const String& t) { return t.getFloatValue() / 100.0f; };
    params.push_back (std::make_unique<AudioProcessorValueTreeState::Parameter> (insanityTag, "Insanity", String(),
        NormalisableRange<float> { 0.0f, 1.0f }, 0.0f, insanityToString, stringToInsanity));
}

void InsanityControl::timerCallback()
{
    if (insanityParam->load() == 0.0f) // nothing to do...
        return;

    // set randomised params
    float scale = 0.5f * std::pow (insanityParam->load(), 2.0f);
    doForNodes ([=] (DelayNode* n) {
        float delay = n->getDelay();
        float pan = n->getPan();

        delay += n->delaySmoother.processSample (delay_dist (generator) * scale);
        pan   += n->panSmoother.processSample   (pan_dist   (generator) * scale);

        if (! n->isParamLocked (delayTag))
            n->setDelay (jlimit (0.0f, 1.0f, delay));
        
        if (! n->isParamLocked (panTag))
            n->setPan (jlimit (-1.0f, 1.0f, pan));
    });

    startTimerHz (timerFreq); // update timer callback rate
}

void InsanityControl::parameterChanged (const String&, float newValue)
{
    // timer callback won't do anything, so reset smoothing filters
    if (newValue == 0.0f)
    {
        doForNodes ([=] (DelayNode* n) {
            n->delaySmoother.reset();
            n->panSmoother.reset();
        });
    }
    
    // calc new timer callback rate
    timerFreq = int (std::pow (10, 1.0f + 0.65f * std::sqrt (newValue)));
    
    // update smoothing filters
    auto smoothCoefs = dsp::IIR::Coefficients<float>::makeFirstOrderLowPass ((double) timerFreq, smoothFreq);
    doForNodes ([=] (DelayNode* n) {
        n->delaySmoother.coefficients = smoothCoefs;
        n->panSmoother = smoothCoefs;
    });
}

void InsanityControl::newNodeAdded (DelayNode* newNode)
{
    auto smoothCoefs = dsp::IIR::Coefficients<float>::makeFirstOrderLowPass ((double) timerFreq, smoothFreq);
    newNode->delaySmoother.coefficients = smoothCoefs;
    newNode->panSmoother = smoothCoefs;
}
