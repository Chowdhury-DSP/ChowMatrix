#include "ChowMatrix.h"

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

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowMatrix();
}
