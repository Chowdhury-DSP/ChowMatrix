#include "StateManager.h"

StateManager::StateManager (AudioProcessorValueTreeState& vts, std::array<InputNode, 2>& nodes) :
    vts (vts),
    inputNodes (nodes),
    presetManager (this, vts)
{}

void StateManager::loadDefaultABStates()
{
    for (auto& state : abStates)
        state = saveState();
}

void StateManager::toggleABState()
{
    // save current state
    int saveABState = (int) currentState;
    abStates[saveABState] = saveState();

    // load new state
    int loadABState = (int) ! currentState;
    loadState (abStates[loadABState].get());

    currentState = ! currentState; // swap!
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
    forEachXmlChildElement (*childrenXml, childXml)
    {
        if (count > 2)
            break;

        inputNodes[count++].loadXml (childXml);
    }
}
