#include "DelayTypeControl.h"
#include "../../NodeManager.h"

namespace
{
const String delayTypeTag = "delay_type";
}

static inline VariableDelay::DelayType getDelayType (float param)
{
    return static_cast<VariableDelay::DelayType> (int (param));
}

DelayTypeControl::DelayTypeControl (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>* nodes, StateManager& stateMgr) : BaseController (vts, nodes, { delayTypeTag }),
                                                                                                                                  stateManager (stateMgr)
{
    delayTypeParam = vts.getRawParameterValue (delayTypeTag);
    parameterChanged (delayTypeTag, delayTypeParam->load());
}

void DelayTypeControl::addParameters (Parameters& params)
{
    params.push_back (std::make_unique<AudioParameterChoice> (delayTypeTag,
                                                              "Delay Type",
                                                              StringArray ({ "Glitch", "Rough", "Smooth", "Ultra Smooth", "Liquid", "Super Liquid", "Lo-Fi", "Analog", "Alien" }),
                                                              2));
}

void DelayTypeControl::parameterChanged (const String& paramID, float newValue)
{
    if (stateManager.getIsLoading())
    {
        // StateManager is currently loading a new state.
        // Let's wait until it's done and call again...
        Thread::sleep (2);
        MessageManager::callAsync ([=] { parameterChanged (paramID, newValue); });
    }
    else
    {
        const SpinLock::ScopedTryLockType stateLoadTryLock (stateManager.getStateLoadLock());

        if (stateLoadTryLock.isLocked())
        {
            // We're sure it's safe to set delay types now!
            auto type = getDelayType (newValue);
            doForNodes ([=] (DelayNode* n) { n->setDelayType (type); });
        }
        else
        {
            // Can't reset delay types while processing audio!
            // Let's wait and try again...
            Thread::sleep (2);
            MessageManager::callAsync ([=] { parameterChanged (paramID, newValue); });
        }
    }
}

void DelayTypeControl::newNodeAdded (DelayNode* newNode)
{
    newNode->setDelayType (getDelayType (delayTypeParam->load()));
}
