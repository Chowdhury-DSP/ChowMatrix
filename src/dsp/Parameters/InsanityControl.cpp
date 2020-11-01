#include "InsanityControl.h"
#include "../../NodeManager.h"

namespace
{
    const String insanityTag = "insanity";
    constexpr float smoothFreq = 2.0f;
}

InsanityControl::InsanityControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) :
    vts (vts),
    nodes (nodes)
{
    insanityParam = vts.getRawParameterValue (insanityTag);
    vts.addParameterListener (insanityTag, this);
    
    for (auto& node : *nodes)
    {
        node.addNodeListener (this);
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { n->addNodeListener (this); });
    }

    parameterChanged (insanityTag, insanityParam->load());
}

InsanityControl::~InsanityControl()
{
    for (auto& node : *nodes)
        NodeManager::doForNodes (&node, [=] (DelayNode* n) { n->removeNodeListener (this); });

    vts.removeParameterListener (insanityTag, this);
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
    for (auto& node : *nodes)
        NodeManager::doForNodes (&node, [=] (DelayNode* n) {
            float delay = n->getDelay();
            float pan = n->getPan();

            delay += n->delaySmoother.processSample (delay_dist (generator) * scale);
            pan   += n->panSmoother.processSample   (pan_dist   (generator) * scale);

            n->setDelay (jlimit (0.0f, 1.0f, delay));
            n->setPan (jlimit (-1.0f, 1.0f, pan));
        });
}

void InsanityControl::parameterChanged (const String&, float newValue)
{
    // timer callback won't do anything, so reset smoothing filters
    if (newValue == 0.0f)
    {
        for (auto& node : *nodes)
            NodeManager::doForNodes (&node, [=] (DelayNode* n) {
                n->delaySmoother.reset();
                n->panSmoother.reset();
            });
    }

    // update timer callback rate
    timerFreq = int (std::pow (10, 1.0f + std::sqrt (newValue)));
    startTimerHz (timerFreq);

    // update smoothing filters
    auto smoothCoefs = dsp::IIR::Coefficients<float>::makeFirstOrderLowPass ((double) timerFreq, smoothFreq);
    for (auto& node : *nodes)
        NodeManager::doForNodes (&node, [=] (DelayNode* n) {
            n->delaySmoother.coefficients = smoothCoefs;
            n->panSmoother = smoothCoefs;
        });
}

void InsanityControl::nodeAdded (DelayNode* newNode)
{
    newNode->addNodeListener (this);
    auto smoothCoefs = dsp::IIR::Coefficients<float>::makeFirstOrderLowPass ((double) timerFreq, smoothFreq);
    newNode->delaySmoother.coefficients = smoothCoefs;
    newNode->panSmoother = smoothCoefs;
}

void InsanityControl::nodeRemoved (DelayNode* nodeToRemove)
{
    nodeToRemove->removeNodeListener (this);
}
