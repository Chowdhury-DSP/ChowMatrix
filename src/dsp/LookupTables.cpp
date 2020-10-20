#include "LookupTables.h"
#include <polylogarithm/Li2.hpp>

namespace
{
    constexpr int N = 1 << 16;
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

LookupTables::LookupTables()
{
    lut.initialise ([=] (double x) { return func (x); }, minVal, maxVal, N);
    lut_AD1.initialise ([=] (double x) { return func_AD1 (x); }, 2 * minVal, 2 * maxVal, 4 * N);
    lut_AD2.initialise ([=] (double x) { return func_AD2 (x); }, 4 * minVal, 4 * maxVal, 16 * N);
}
