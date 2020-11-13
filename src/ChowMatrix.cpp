#include "ChowMatrix.h"
#include "gui/BottomBar/BottomBarLNF.h"
#include "gui/BottomBar/TextSliderItem.h"
#include "gui/DetailsView/NodeDetailsGUI.h"
#include "gui/InsanityLNF.h"
#include "gui/MatrixView/GraphView.h"

namespace
{
    const String dryTag = "dry_param";
    const String wetTag = "wet_param";

    constexpr double gainFadeTime = 0.05;
    constexpr float negInfDB = -60.0f;
}

ChowMatrix::ChowMatrix() :
    insanityControl (vts, &inputNodes),
    delayTypeControl (vts, &inputNodes),
    syncControl (vts, &inputNodes)
{
    manager.initialise (&inputNodes);

    dryParamDB = vts.getRawParameterValue (dryTag);
    wetParamDB = vts.getRawParameterValue (wetTag);

    dryGain.setRampDurationSeconds (gainFadeTime);
    wetGain.setRampDurationSeconds (gainFadeTime);

    for (auto& node : inputNodes)
        node.addChild();
}

void ChowMatrix::addParameters (Parameters& params)
{
    NormalisableRange<float> gainRange (negInfDB, 12.0f);

    auto gainToString = [] (float x) { return x <= negInfDB ? "-inf dB" : String (x, 1, false) + " dB"; };
    auto stringToGain = [] (const String& t) { return t.getFloatValue(); };

    params.push_back (std::make_unique<Parameter> (dryTag, "Dry", String(),
        gainRange, 0.0f, gainToString, stringToGain));

    params.push_back (std::make_unique<Parameter> (wetTag, "Wet", String(),
        gainRange, 0.0f, gainToString, stringToGain));

    InsanityControl::addParameters (params);
    DelayTypeControl::addParameters (params);
    SyncControl::addParameters (params);
}

void ChowMatrix::prepareToPlay (double sampleRate, int samplesPerBlock)
{
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
        chBuffers[ch].clear();
        chBuffers[ch].copyFrom (0, 0, buffer, ch, 0, numSamples);
    }

    // get wet signal
    buffer.clear();
    for (size_t ch = 0; ch < (size_t) buffer.getNumChannels(); ++ch)
        inputNodes[ch].process (chBuffers[ch], buffer, numSamples);

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
    builder->registerLookAndFeel ("InsanityLNF", std::make_unique<InsanityLNF>());
    builder->registerLookAndFeel ("BottomBarLNF", std::make_unique<BottomBarLNF>());

    // GUI trigger functions
    magicState.addTrigger ("flush_delays", [=] {
        NodeManager::doForNodes (&inputNodes, [] (DelayNode* n) { n->flushDelays(); });
    });

    magicState.addTrigger ("randomise", [=] {
        NodeManager::doForNodes (&inputNodes, [] (DelayNode* n) { n->randomiseParameters(); });
    });

    return new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize, std::move (builder));
}

void ChowMatrix::getStateInformation (MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<XmlElement> xml = std::make_unique<XmlElement> ("state");
    xml->addChildElement (state.createXml().release());

    std::unique_ptr<XmlElement> childrenXml = std::make_unique<XmlElement> ("children");
    for (auto& node : inputNodes)
        childrenXml->addChildElement (node.saveXml());
    
    xml->addChildElement (childrenXml.release());
    
    copyXmlToBinary (*xml, destData);
}

void ChowMatrix::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState == nullptr) // invalid XML
        return;

    auto vtsXml = xmlState->getChildByName (vts.state.getType());
    if (vtsXml == nullptr) // invalid ValueTreeState
        return;

    auto childrenXml = xmlState->getChildByName ("children");
    if (childrenXml == nullptr) // invalid children XML
        return;

    for (auto& node : inputNodes)
        node.clearChildren();

    vts.replaceState (ValueTree::fromXml (*vtsXml));

    size_t count = 0;
    forEachXmlChildElement (*childrenXml, childXml)
    {
        if (count > 2)
            break;

        inputNodes[count++].loadXml (childXml);
    }
}

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowMatrix();
}
