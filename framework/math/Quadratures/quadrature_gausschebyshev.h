#pragma once

#include "opensn/framework/math/Quadratures/quadrature.h"

namespace chi_math
{

//######################################################### Class Def
/**Gauss-Chebyshev quadrature.*/
class QuadratureGaussChebyshev : public chi_math::Quadrature
{
public:
  static chi::InputParameters GetInputParameters();
  explicit QuadratureGaussChebyshev(const chi::InputParameters& params);

  explicit QuadratureGaussChebyshev(unsigned int N, bool verbose = false);

private:
  void Initialize(unsigned int N);
};

} // namespace chi_math
