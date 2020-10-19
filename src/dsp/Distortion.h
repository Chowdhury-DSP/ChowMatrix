#pragma once

#include <JuceHeader.h>

namespace ADAAConsts
{
    constexpr double TOL = 1.0e-5;
}

class ADAA2
{
public:
    ADAA2() = default;

    using Func = std::function<double(double)>;
    void initialise (Func nl, Func ad1, Func ad2, size_t N);
    void prepare();

    inline double process (double x) noexcept
    {
        bool illCondition = std::abs (x - x2) < ADAAConsts::TOL;
        double d1 = calcD1 (x);

        double y = illCondition ?
            fallback (x) :
            (2.0 / (x - x2)) * (d1 - d2);

        // update state
        d2 = d1;
        x2 = x1;
        x1 = x;
        
        return y;
    }

private:
    inline double calcD1 (double x0) noexcept
    {
        bool illCondition = std::abs (x0 - x1) < ADAAConsts::TOL;
        double ad2_x0 = lut_AD2.processSample (x0);

        double y = illCondition ?
            lut_AD1.processSample (0.5 * (x0 + x1)) :
            (ad2_x0 - ad2_x1) / (x0 - x1);

        ad2_x1 = ad2_x0;
        return y;
    }

    inline double fallback (double x) const noexcept
    {
        double xBar = 0.5 * (x + x2);
        double delta = xBar - x;

        bool illCondition = std::abs (delta) < ADAAConsts::TOL;
        return illCondition ?
            lut.processSample (0.5 * (xBar + x)) :
            (2.0 / delta) * (lut_AD1.processSample (xBar)
                + (lut_AD2.processSample (x) - lut_AD2.processSample (xBar)) / delta);
    }

    double x1 = 0.0;
    double x2 = 0.0;
    double ad2_x1 = 0.0;
    double d2 = 0.0;

    dsp::LookupTableTransform<double> lut;
    dsp::LookupTableTransform<double> lut_AD1;
    dsp::LookupTableTransform<double> lut_AD2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADAA2)
};

class Distortion
{
public:
    Distortion();

    void setGain (float newGain) { gain.setTargetValue (newGain); }
    void prepare (dsp::ProcessSpec spec);
    void reset();

    inline float processSample (float x)
    {
        float curGain = gain.getNextValue();
        return (float) adaa.process ((double) (curGain * x)) / curGain;
    }

private:
    ADAA2 adaa;
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> gain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Distortion)
};
