// ====================================================================
// This file is part of Polylogarithm.
//
// Polylogarithm is licenced under the GNU Lesser General Public
// License (GNU LGPL) version 3.
// ====================================================================

#pragma once
#include <complex>

namespace polylogarithm {

/// real polylogarithm with n=2 (dilogarithm)
double Li2(double) noexcept;

/// real polylogarithm with n=2 (dilogarithm) with long double precision
long double Li2(long double) noexcept;

} // namespace polylogarithm
