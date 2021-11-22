#include "StateManager.h"

StateManager::StateManager (AudioProcessorValueTreeState& vts,
                            HostParamControl& paramControl,
                            std::array<InputNode, 2>& nodes) : vts (vts),
                                                               paramControl (paramControl),
                                                               inputNodes (nodes)
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

void StateManager::handleAsyncUpdate()
{
    ScopedLock sl (xmlStateChangingSection);
    if (xmlStateToLoad != nullptr)
    {
        loadStateInternal (xmlStateToLoad.get());
        xmlStateToLoad.reset();
    }
}

std::unique_ptr<XmlElement> StateManager::saveState()
{
    // save parameters
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml = std::make_unique<XmlElement> (stateXmlTag);
    xml->addChildElement (state.createXml().release());

    // save nodes
    std::unique_ptr<XmlElement> childrenXml = std::make_unique<XmlElement> (nodesXmlTag);
    for (auto& node : inputNodes)
        childrenXml->addChildElement (node.saveXml());
    xml->addChildElement (childrenXml.release());

    // save parameter mappings
    std::unique_ptr<XmlElement> paramMapXml = std::make_unique<XmlElement> (paramMapXmlTag);
    paramControl.saveGlobalMap (paramMapXml.get());
    xml->addChildElement (paramMapXml.release());

    return std::move (xml);
}

void StateManager::loadState (const XmlElement* xml)
{
    // load ValueTreeState synchronously
    if (xml == nullptr || ! xml->hasTagName (stateXmlTag)) // invalid XML
        return;

    auto vtsXml = xml->getChildByName (vts.state.getType());
    if (vtsXml == nullptr) // invalid ValueTreeState
        return;

    vts.replaceState (ValueTree::fromXml (*vtsXml)); //load parameters

    // load nodes synchronously if we're on the message thread
    if (MessageManager::existsAndIsCurrentThread())
    {
        loadStateInternal (xml);
        return;
    }

    // otherwise load nodes asynchronously
    ScopedLock sl (xmlStateChangingSection);
    xmlStateToLoad = std::make_unique<XmlElement> (*xml);
    triggerAsyncUpdate();
}

void StateManager::loadStateInternal (const XmlElement* xmlState)
{
    const SpinLock::ScopedLockType stateLoadingScopedLock (stateLoadingLock); // Lock our SpinLock so the processor won't try to run while we're loading new state
    isLoading.store (true);

    auto childrenXml = xmlState->getChildByName (nodesXmlTag);
    if (childrenXml == nullptr) // invalid children XML
        return;

    // clear current nodes
    for (auto& node : inputNodes)
        node.clearChildren();

    // load nodes
    size_t count = 0;
    for (auto* childXml : childrenXml->getChildIterator())
    {
        if (count > 2)
            break;

        inputNodes[count++].loadXml (childXml);
    }

    auto paramMapXml = xmlState->getChildByName (paramMapXmlTag);
    if (paramMapXml == nullptr) // invalid param map XML
        return;

    paramControl.loadGlobalMap (paramMapXml);
    isLoading.store (false);
}

const Identifier StateManager::stateXmlTag { "state" };
const Identifier StateManager::nodesXmlTag { "nodes" };
const Identifier StateManager::paramMapXmlTag { "param_maps" };
