#include "StateManager.h"

StateManager::StateManager (AudioProcessorValueTreeState& vts,
                            HostParamControl& paramControl,
                            std::array<InputNode, 2>& nodes) : vts (vts),
                                                               paramControl (paramControl),
                                                               inputNodes (nodes),
                                                               presetManager (this, vts)
{
}

void StateManager::loadDefaultABStates()
{
    for (auto& state : abStates)
        state = saveState();
}

void StateManager::copyABState()
{
    const auto saveABState = static_cast<size_t> (! currentState);
    abStates[saveABState] = saveState();
}

void StateManager::setCurrentABState (int newState)
{
    if ((bool) newState == currentState)
        return;

    currentState = newState;
    copyABState();
    loadState (abStates[(size_t) currentState].get());
}

std::unique_ptr<XmlElement> StateManager::saveState()
{
    // save parameters
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml = std::make_unique<XmlElement> ("state");
    xml->addChildElement (state.createXml().release());

    // save nodes
    std::unique_ptr<XmlElement> childrenXml = std::make_unique<XmlElement> ("nodes");
    for (auto& node : inputNodes)
        childrenXml->addChildElement (node.saveXml());
    xml->addChildElement (childrenXml.release());

    // save parameter mappings
    std::unique_ptr<XmlElement> paramMapXml = std::make_unique<XmlElement> ("param_maps");
    paramControl.saveGlobalMap (paramMapXml.get());
    xml->addChildElement (paramMapXml.release());

    return std::move (xml);
}

void StateManager::loadState (XmlElement* xmlState)
{
    MessageManagerLock mml; // lock MessageManager so other parameter changes won't happen while we're loading new state
    const SpinLock::ScopedLockType stateLoadingScopedLock (stateLoadingLock); // Lock our SpinLock so the processor won't try to run while we're loading new state

    if (xmlState == nullptr) // invalid XML
        return;

    auto vtsXml = xmlState->getChildByName (vts.state.getType());
    if (vtsXml == nullptr) // invalid ValueTreeState
        return;

    auto childrenXml = xmlState->getChildByName ("nodes");
    if (childrenXml == nullptr) // invalid children XML
        return;

    // clear current nodes
    for (auto& node : inputNodes)
        node.clearChildren();

    vts.replaceState (ValueTree::fromXml (*vtsXml)); //load parameters

    // load nodes
    size_t count = 0;

    for (auto* childXml : childrenXml->getChildIterator())
    {
        if (count > 2)
            break;

        inputNodes[count++].loadXml (childXml);
    }

    auto paramMapXml = xmlState->getChildByName ("param_maps");
    if (paramMapXml == nullptr) // invalid param map XML
        return;

    paramControl.loadGlobalMap (paramMapXml);
}
