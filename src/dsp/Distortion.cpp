#include "Distortion.h"
#include <polylogarithm/Li2.hpp>

namespace
{
    constexpr double minVal = -10.0;
    constexpr double maxVal = 10.0;
}

inline double func (double x) noexcept
{
    return std::tanh (x);
}

/** First antiderivative of hard clipper */
inline double func_AD1 (double x) noexcept
{
    return std::log (std::cosh (x));
}

/** Second antiderivative of hard clipper */
inline double func_AD2 (double x) noexcept
{
    const auto expVal = std::exp (-2 * x);
    return 0.5 * ((double) polylogarithm::Li2 (-expVal)
        - x * (x + 2.0 * std::log (expVal + 1.) - 2.0 * std::log (std::cosh (x))))
        + (std::pow (MathConstants<double>::pi, 2) / 24.0);
}

//===============================================================
Distortion::Distortion()
{
    adaa.initialise (&func, &func_AD1, &func_AD2, 1 << 18);
    gain.setCurrentAndTargetValue (1.0f);
}

void Distortion::prepare (dsp::ProcessSpec spec)
{
    adaa.prepare();
    gain.reset (spec.sampleRate, 0.05);
}

void Distortion::reset()
{
    adaa.prepare();
    gain.setCurrentAndTargetValue (gain.getTargetValue());
}

//===============================================================
void ADAA2::initialise (Func nl, Func ad1, Func ad2, size_t N)
{
    lut.initialise ([=] (double x) { return nl (x); }, minVal, maxVal, N);
    lut_AD1.initialise ([=] (double x) { return ad1 (x); }, 2 * minVal, 2 * maxVal, 4 * N);
    lut_AD2.initialise ([=] (double x) { return ad2 (x); }, 4 * minVal, 4 * maxVal, 16 * N);
}

void ADAA2::prepare()
{
    x1 = 0.0;
    x2 = 0.0;
    ad2_x1 = 0.0;
    d2 = 0.0;
}
