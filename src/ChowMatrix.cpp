#include "ChowMatrix.h"
#include "gui/GraphView.h"

ChowMatrix::ChowMatrix()
{
    
}

void ChowMatrix::addParameters (Parameters& params)
{
    
}

void ChowMatrix::prepareToPlay (double sampleRate, int samplesPerBlock)
{

}

void ChowMatrix::releaseResources()
{

}

void ChowMatrix::processBlock (AudioBuffer<float>& buffer)
{

}

AudioProcessorEditor* ChowMatrix::createEditor()
{
    auto builder = chowdsp::createGUIBuilder (magicState);
    builder->registerFactory ("GraphView", &GraphViewItem::factory);

    return new foleys::MagicPluginEditor (magicState, BinaryData::gui_xml, BinaryData::gui_xmlSize, std::move (builder));
}

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowMatrix();
}
