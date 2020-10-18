#include "ChowMatrix.h"
#include "gui/MatrixView/GraphView.h"
#include "gui/DetailsView/NodeDetailsGUI.h"

ChowMatrix::ChowMatrix()
{
    
}

void ChowMatrix::addParameters (Parameters& params)
{
}

void ChowMatrix::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
    {
        inputNodes[ch].prepare (sampleRate, samplesPerBlock);
        chBuffers[ch].setSize (1, samplesPerBlock);
    }
}

void ChowMatrix::releaseResources()
{

}

void ChowMatrix::processBlock (AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        chBuffers[ch].clear();
        chBuffers[ch].copyFrom (0, 0, buffer, ch, 0, numSamples);
    }

    buffer.clear();

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        inputNodes[ch].process (chBuffers[ch], buffer, numSamples);
}

AudioProcessorEditor* ChowMatrix::createEditor()
{
    auto builder = chowdsp::createGUIBuilder (magicState);
    builder->registerFactory ("GraphView", &GraphViewItem::factory);
    builder->registerFactory ("NodeDetails", &NodeDetailsItem::factory);

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

    if (xmlState == nullptr)
        return;

    auto vtsXml = xmlState->getChildByName (vts.state.getType());
    if (vtsXml == nullptr)
        return;

    auto childrenXml = xmlState->getChildByName ("children");
    if (childrenXml == nullptr)
        return;

    for (auto& node : inputNodes)
        node.clearChildren();

    vts.replaceState (ValueTree::fromXml (*vtsXml));

    int count = 0;
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
