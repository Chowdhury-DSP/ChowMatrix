#include "SyncControl.h"
#include "../../NodeManager.h"

namespace
{
const String syncTag = "snyc";
}

SyncControl::SyncControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes) : BaseController (vts, nodes, { syncTag })
{
    syncParam = vts.getRawParameterValue (syncTag);
    parameterChanged (syncTag, syncParam->load());
}

void SyncControl::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterBool> (syncTag, "Sync", false));
}

void SyncControl::setTempo (AudioPlayHead* playhead)
{
    double newTempo = 120.0;

    // get tempo from host
    if (playhead)
    {
        AudioPlayHead::CurrentPositionInfo posInfo;
        playhead->getCurrentPosition (posInfo);
        newTempo = posInfo.bpm > 10.0 ? posInfo.bpm : 120.0;
    }

    // load new tempo if needed
    if (newTempo != tempo.load())
    {
        tempo.store (newTempo);
        doForNodes ([=] (DelayNode* n) { n->setTempo (newTempo); });
    }

    doForNodes ([=] (DelayNode* n) { n->setPlayHead (playhead); });
}

void SyncControl::newNodeAdded (DelayNode* newNode)
{
    newNode->setTempo (tempo.load());
    newNode->setDelaySync (static_cast<bool> (syncParam->load()));
}

void SyncControl::parameterChanged (const String&, float newValue)
{
    if (MessageManager::getInstance()->isThisTheMessageThread())
    {
        doForNodes ([=] (DelayNode* n)
                    { n->setDelaySync (static_cast<bool> (newValue)); });
    }
    else
    {
        MessageManager::callAsync ([=]
                                   { doForNodes ([=] (DelayNode* n)
                                                 { n->setDelaySync (static_cast<bool> (newValue)); }); });
    }
}
