#pragma once

#include <pch.h>

namespace ADAAConsts
{
constexpr double TOL = 1.0e-4;
}

/**
 *  Class to implement 2nd-order anti-derivative anti-aliasing
 */
class ADAA2
{
public:
    ADAA2() = default;

    void prepare();

    inline double process (double x) noexcept
    {
        bool illCondition = std::abs (x - x2) < ADAAConsts::TOL;
        double d1 = calcD1 (x);

        double y = illCondition ? fallback (x) : (2.0 / (x - x2)) * (d1 - d2);

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
        double ad2_x0 = tables->lut_AD2.processSample (x0);

        double y = illCondition ? tables->lut_AD1.processSample (0.5 * (x0 + x1)) : (ad2_x0 - ad2_x1) / (x0 - x1);

        ad2_x1 = ad2_x0;
        return y;
    }

    inline double fallback (double x) const noexcept
    {
        double xBar = 0.5 * (x + x2);
        double delta = xBar - x;

        bool illCondition = std::abs (delta) < ADAAConsts::TOL;
        return illCondition ? tables->lut.processSample (0.5 * (xBar + x)) : (2.0 / delta) * (tables->lut_AD1.processSample (xBar) + (tables->lut_AD2.processSample (x) - tables->lut_AD2.processSample (xBar)) / delta);
    }

    double x1 = 0.0;
    double x2 = 0.0;
    double ad2_x1 = 0.0;
    double d2 = 0.0;

    SharedResourcePointer<LookupTables> tables;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADAA2)
};

/**
 * Class to implement tanh distortion
 */
class Distortion
{
public:
    Distortion();

    void setGain (float newGain) { gain.setTargetValue (newGain); }
    void prepare (const dsp::ProcessSpec& spec);
    void reset();

    inline float processSample (float x)
    {
        float curGain = gain.getNextValue();

        if (curGain < 0.6f)
            return x;

        return (float) adaa.process ((double) (curGain * x)) / curGain;
    }

private:
    ADAA2 adaa;
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> gain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Distortion)
};
