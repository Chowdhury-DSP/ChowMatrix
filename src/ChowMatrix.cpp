#include "ChowMatrix.h"
#include "gui/BottomBar/ABComp.h"
#include "gui/BottomBar/BottomBarLNF.h"
#include "gui/BottomBar/HostControlMenu.h"
#include "gui/BottomBar/TextSliderItem.h"
#include "gui/BottomBar/WetGainSlider.h"
#include "gui/DetailsView/NodeDetailsGUI.h"
#include "gui/IOSUtils/PopupMenuOptionsHelpers.h"
#include "gui/IOSUtils/TipJar.h"
#include "gui/LookAndFeel/InsanityLNF.h"
#include "gui/LookAndFeel/PresetsLNF.h"
#include "gui/MatrixView/GraphViewItem.h"
#include "state/PresetManager.h"

namespace
{
const String dryTag = "dry_param";
const String wetTag = "wet_param";
const String wetGainCompTag = "wet_gain_comp_param";

const Identifier pluginStateTag = "ChowMatrix_State";

constexpr double gainFadeTime = 0.05;
constexpr float negInfDB = -60.0f;
} // namespace

ChowMatrix::ChowMatrix() : insanityControl (vts, &inputNodes),
                           hostParamControl (vts, &inputNodes),
                           delayTypeControl (vts, &inputNodes, stateManager),
                           syncControl (vts, &inputNodes),
                           nodeParamControl (vts, &inputNodes, presetManager),
                           stateManager (vts, hostParamControl, inputNodes)
{
    manager.initialise (&inputNodes);

    dryParamDB = vts.getRawParameterValue (dryTag);
    wetParamDB = vts.getRawParameterValue (wetTag);
    wetGainCompParam = vts.getRawParameterValue (wetGainCompTag);

    dryGain.setRampDurationSeconds (gainFadeTime);
    wetGain.setRampDurationSeconds (gainFadeTime);

    for (auto& node : inputNodes)
        node.addChild();

    stateManager.loadDefaultABStates();
    presetManager = std::make_unique<PresetManager> (vts, stateManager);
}

void ChowMatrix::addParameters (Parameters& params)
{
    using namespace chowdsp::ParamUtils;

    NormalisableRange<float> gainRange (negInfDB, 12.0f);
    auto gainToString = [] (float x) { return x <= negInfDB ? "-inf dB" : String (x, 1, false) + " dB"; };
    auto stringToGain = [] (const String& t) { return t.getFloatValue(); };

    emplace_param<Parameter> (params, dryTag, "Dry", String(), gainRange, -12.0f, gainToString, stringToGain);
    emplace_param<Parameter> (params, wetTag, "Wet", String(), gainRange, -12.0f, gainToString, stringToGain);
    emplace_param<AudioParameterBool> (params, wetGainCompTag, "Wet Gain Comp.", false);

    InsanityControl::addParameters (params);
    DelayTypeControl::addParameters (params);
    SyncControl::addParameters (params);
    HostParamControl::addParameters (params);
}

void ChowMatrix::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const SpinLock::ScopedLockType stateLoadLock (stateManager.getStateLoadLock());

    for (size_t ch = 0; ch < 2; ++ch)
    {
        inputNodes[ch].prepare (sampleRate, samplesPerBlock);
        chBuffers[ch].setSize (1, samplesPerBlock);
    }

    dryBuffer.setSize (2, samplesPerBlock);
    dryGain.prepare ({ sampleRate, (uint32) samplesPerBlock, 2 });
    wetGain.prepare ({ sampleRate, (uint32) samplesPerBlock, 2 });
}

void ChowMatrix::releaseResources()
{
}

void ChowMatrix::processAudioBlock (AudioBuffer<float>& buffer)
{
    const SpinLock::ScopedTryLockType stateLoadTryLock (stateManager.getStateLoadLock());
    if (! stateLoadTryLock.isLocked())
        return;

    auto setGain = [] (auto& gainProc, float gainParamDB) {
        if (gainParamDB <= negInfDB)
            gainProc.setGainLinear (0.0f);
        else
            gainProc.setGainDecibels (gainParamDB);
    };

    // update BPM
    syncControl.setTempo (getPlayHead());

    // Keep dry signal
    dryBuffer.makeCopyOf (buffer, true);
    dsp::AudioBlock<float> dryBlock (dryBuffer);
    dsp::ProcessContextReplacing<float> dryContext (dryBlock);

    setGain (dryGain, dryParamDB->load());
    dryGain.process (dryContext);

    // copy input channels
    const int numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        chBuffers[ch].setSize (1, numSamples, false, false, true);
        chBuffers[ch].copyFrom (0, 0, buffer, ch, 0, numSamples);
    }

    // get wet signal
    buffer.clear();
    for (size_t ch = 0; ch < (size_t) buffer.getNumChannels(); ++ch)
        inputNodes[ch].process (chBuffers[ch], buffer);

    // wet gain (with gain compensation if needed)
    setGain (wetGain, wetParamDB->load());
    if (*wetGainCompParam == 1.0f)
    {
        auto wetAddedGain = 0.0f;
        for (size_t ch = 0; ch < (size_t) buffer.getNumChannels(); ++ch)
            wetAddedGain += inputNodes[ch].getNodeLevel (1.0f);

        if (wetAddedGain > 0.0f)
            wetGain.setGainLinear (wetGain.getGainLinear() / wetAddedGain);
    }

    dsp::AudioBlock<float> wetBlock (buffer);
    dsp::ProcessContextReplacing<float> wetContext (wetBlock);
    wetGain.process (wetContext);

    // sum with dry signal
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        buffer.addFrom (ch, 0, dryBuffer, ch, 0, numSamples);
}

AudioProcessorEditor* ChowMatrix::createEditor()
{
    // Register GUI items for Foleys GUI Magic
    auto builder = chowdsp::createGUIBuilder (magicState);
    builder->registerFactory ("GraphView", &GraphViewItem::factory);
    builder->registerFactory ("NodeDetails", &NodeDetailsItem::factory);
    builder->registerFactory ("TextSlider", &TextSliderItem::factory);
    builder->registerFactory ("WetGainSlider", &WetGainSliderItem::factory);
    builder->registerFactory ("PresetComp", &chowdsp::PresetsItem<ChowMatrix>::factory);
    builder->registerFactory ("ABComp", &ABCompItem::factory);
    builder->registerFactory ("HostControlMenu", &HostControlMenuItem::factory);
    builder->registerLookAndFeel ("InsanityLNF", std::make_unique<InsanityLNF>());
    builder->registerLookAndFeel ("BottomBarLNF", std::make_unique<BottomBarLNF>());
    builder->registerLookAndFeel ("PresetsLNF", std::make_unique<PresetsLNF>());

#if JUCE_IOS
    builder->registerFactory ("TipJar", &TipJarItem::factory);
    builder->registerLookAndFeel ("TipJarLNF", std::make_unique<TipJarLNF>());
    builder->setIdsToIgnore ({ "NodeDetails" });
#endif

    // GUI trigger functions
    magicState.addTrigger ("flush_delays", [=] { NodeManager::doForNodes (&inputNodes, [] (DelayNode* n) { n->flushDelays(); }); });

    magicState.addTrigger ("randomise", [=] { NodeManager::doForNodes (&inputNodes, [] (DelayNode* n) { n->randomiseParameters(); }); });

    magicState.addTrigger ("insanity_reset", [=] { insanityControl.resetInsanityState(); });

    auto changeUIComponentsToIgnore = [=] (auto* editor, std::initializer_list<Identifier> ids, bool needsGraphView = false) {
        {
            graphViewPtr.reset();

            auto bounds = editor->getBounds();
            editor->getGUIBuilder().setIdsToIgnore (std::move (ids));
            editor->setConfigTree (editor->getGUIBuilder().getConfigTree());
            editor->setSize (bounds.getWidth(), bounds.getHeight());

            // this is not a good way to do this, but...
            // we need the graph view to exist for the node details to look right.
            if (needsGraphView)
                graphViewPtr = std::make_unique<GraphViewport> (*this);
        }
    };

    magicState.addTrigger ("view_control",
                           [=] {
                               if (auto* editor = dynamic_cast<foleys::MagicPluginEditor*> (getActiveEditor()))
                               {
                                   PopupMenu menu;
                                   menu.addItem ("Only show Matrix View", [=] { changeUIComponentsToIgnore (editor, { "NodeDetails" }); });
                                   menu.addItem ("Only show Details View", [=] { changeUIComponentsToIgnore (editor, { "GraphView" }, true); });
                                   menu.addItem ("Show both views", [=] { changeUIComponentsToIgnore (editor, {}); });

                                   chowdsp::SharedLNFAllocator lnfAllocator;
                                   menu.setLookAndFeel (lnfAllocator->getLookAndFeel<BottomBarLNF>());
                                   menu.showMenuAsync (PopupMenuOptionsHelpers::createPopupMenuOptions (nullptr, editor));
                               }
                           });

#if ! JUCE_IOS
    auto editor = new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize, std::move (builder));
#else
    auto editor = new foleys::MagicPluginEditor (magicState, BinaryData::gui_ios_xml, BinaryData::gui_ios_xmlSize, std::move (builder));
#endif

#if CHOWDSP_AUTO_UPDATE
    updater.showUpdaterScreen (editor);
#endif // CHOWDSP_AUTO_UPDATE

    // we need to set resize limits for StandalonePluginHolder
    editor->setResizeLimits (20, 20, 2000, 2000);

    return editor;
}

void ChowMatrix::getStateInformation (MemoryBlock& destData)
{
    auto xml = std::make_unique<XmlElement> (pluginStateTag);

    xml->addChildElement (stateManager.saveState().release());
    xml->addChildElement (presetManager->saveXmlState().release());

    copyXmlToBinary (*xml, destData);
}

void ChowMatrix::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary (data, sizeInBytes);

    if (! xml->hasTagName (pluginStateTag))
    {
        // backwards compatibility with older way of loading state
        stateManager.loadState (xml.get());
        return;
    }

    if (xml == nullptr) // invalid XML
        return;

    auto stateXml = xml->getChildByName (StateManager::stateXmlTag);
    if (stateXml == nullptr) // invalid StateManager state
        return;

    presetManager->loadXmlState (xml->getChildByName (chowdsp::PresetManager::presetStateTag));
    stateManager.loadState (stateXml);
}

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowMatrix();
}
