#pragma once

#include <JuceHeader.h>

class LookupTables
{
public:
    LookupTables();

    dsp::LookupTableTransform<double> lut;
    dsp::LookupTableTransform<double> lut_AD1;
    dsp::LookupTableTransform<double> lut_AD2;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookupTables)
};
