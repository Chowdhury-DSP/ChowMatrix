#include "ChowMatrix.h"
#include "gui/BottomBar/ABComp.h"
#include "gui/BottomBar/BottomBarLNF.h"
#include "gui/BottomBar/HostControlMenu.h"
#include "gui/BottomBar/TextSliderItem.h"
#include "gui/DetailsView/NodeDetailsGUI.h"
#include "gui/InsanityLNF.h"
#include "gui/MatrixView/GraphViewItem.h"
#include "state/presets/PresetCompItem.h"
#include "state/presets/PresetsLNF.h"

namespace
{
const String dryTag = "dry_param";
const String wetTag = "wet_param";

constexpr double gainFadeTime = 0.05;
constexpr float negInfDB = -60.0f;
} // namespace

ChowMatrix::ChowMatrix() : insanityControl (vts, &inputNodes),
                           hostParamControl (vts, &inputNodes),
                           delayTypeControl (vts, &inputNodes, stateManager),
                           syncControl (vts, &inputNodes),
                           stateManager (vts, hostParamControl, inputNodes)
{
    manager.initialise (&inputNodes);

    dryParamDB = vts.getRawParameterValue (dryTag);
    wetParamDB = vts.getRawParameterValue (wetTag);

    dryGain.setRampDurationSeconds (gainFadeTime);
    wetGain.setRampDurationSeconds (gainFadeTime);

    for (auto& node : inputNodes)
        node.addChild();

    stateManager.loadDefaultABStates();
    stateManager.getPresetManager().hostUpdateFunc = std::bind (&ChowMatrix::updateHostPrograms, this);
}

void ChowMatrix::addParameters (Parameters& params)
{
    NormalisableRange<float> gainRange (negInfDB, 12.0f);

    auto gainToString = [] (float x) { return x <= negInfDB ? "-inf dB" : String (x, 1, false) + " dB"; };
    auto stringToGain = [] (const String& t) { return t.getFloatValue(); };

    params.push_back (std::make_unique<Parameter> (dryTag, "Dry", String(), gainRange, -12.0f, gainToString, stringToGain));

    params.push_back (std::make_unique<Parameter> (wetTag, "Wet", String(), gainRange, -12.0f, gainToString, stringToGain));

    InsanityControl::addParameters (params);
    DelayTypeControl::addParameters (params);
    SyncControl::addParameters (params);
    PresetManager::addParameters (params);
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

    // get parameters
    setGain (dryGain, dryParamDB->load());
    setGain (wetGain, wetParamDB->load());

    // update BPM
    syncControl.setTempo (getPlayHead());

    // Keep dry signal
    dryBuffer.makeCopyOf (buffer, true);
    dsp::AudioBlock<float> dryBlock (dryBuffer);
    dsp::ProcessContextReplacing<float> dryContext (dryBlock);
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
    builder->registerFactory ("PresetComp", &PresetCompItem::factory);
    builder->registerFactory ("ABComp", &ABCompItem::factory);
    builder->registerFactory ("HostControlMenu", &HostControlMenuItem::factory);
    builder->registerLookAndFeel ("InsanityLNF", std::make_unique<InsanityLNF>());
    builder->registerLookAndFeel ("BottomBarLNF", std::make_unique<BottomBarLNF>());
    builder->registerLookAndFeel ("PresetsLNF", std::make_unique<PresetsLNF>());

    // GUI trigger functions
    magicState.addTrigger ("flush_delays", [=] {
        NodeManager::doForNodes (&inputNodes, [] (DelayNode* n) { n->flushDelays(); });
    });

    magicState.addTrigger ("randomise", [=] {
        NodeManager::doForNodes (&inputNodes, [] (DelayNode* n) { n->randomiseParameters(); });
    });

    magicState.addTrigger ("insanity_reset", [=] {
        insanityControl.resetInsanityState();
    });

    auto editor = new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize, std::move (builder));
    updater.showUpdaterScreen (editor);

    // we need to set resize limits for StandalonePluginHolder
    editor->setResizeLimits (10, 10, 1000, 1000);

    return editor;
}

void ChowMatrix::getStateInformation (MemoryBlock& destData)
{
    MessageManagerLock mml;
    auto xml = stateManager.saveState();
    copyXmlToBinary (*xml, destData);
}

void ChowMatrix::setStateInformation (const void* data, int sizeInBytes)
{
    MessageManagerLock mml;
    auto xmlState = getXmlFromBinary (data, sizeInBytes);
    stateManager.loadState (xmlState.get());
}

int ChowMatrix::getNumPrograms()
{
    return stateManager.getPresetManager().getNumFactoryPresets();
}

int ChowMatrix::getCurrentProgram()
{
    auto& presetManager = stateManager.getPresetManager();
    const auto curPresetIdx = presetManager.getSelectedPresetIdx();

    if (curPresetIdx > presetManager.getNumFactoryPresets())
        return 0;

    return curPresetIdx;
}

void ChowMatrix::setCurrentProgram (int index)
{
    auto& presetManager = stateManager.getPresetManager();

    if (index > presetManager.getNumPresets() || index < 0) // out of range!
        return;

    if (index == presetManager.getSelectedPresetIdx()) // no update needed!
        return;

    MessageManager::callAsync ([&presetManager, index] { presetManager.setPreset (index); });
}

const String ChowMatrix::getProgramName (int index)
{
    return stateManager.getPresetManager().getPresetName (index);
}

void ChowMatrix::updateHostPrograms()
{
    MessageManager::callAsync ([=] { updateHostDisplay (AudioProcessorListener::ChangeDetails().withProgramChanged (true)); });
}

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowMatrix();
}
