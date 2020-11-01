#pragma once

#include <JuceHeader.h>

/**
 * Shared resource class to store lookup tables
 * for nonlinear functions
 */ 
class LookupTables
{
public:
    LookupTables();

    dsp::LookupTableTransform<double> lut;      // tanh
    dsp::LookupTableTransform<double> lut_AD1;  // anti-derivative of tanh
    dsp::LookupTableTransform<double> lut_AD2;  // 2nd anti-derivative of tanh

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookupTables)
};
