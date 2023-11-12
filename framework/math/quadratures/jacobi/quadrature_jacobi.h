#pragma once

#include "framework/math/quadratures/quadrature.h"

namespace chi_math
{

/**Jacobi quadrature.*/
class QuadratureJacobi : public chi_math::Quadrature
{
public:
  QuadratureJacobi(opensn::App& app, QuadratureOrder order, unsigned int alpha, unsigned int beta);

private:
  void Initialize(QuadratureOrder order);

  const unsigned int m_alpha_;
  const unsigned int m_beta_;
};

} // namespace chi_math
