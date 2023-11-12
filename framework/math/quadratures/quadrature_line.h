#pragma once

#include "framework/math/quadratures/quadrature_gausslegendre.h"

namespace chi_math
{
class QuadratureLine;
}

/**Quadrature for use on reference lines.*/
class chi_math::QuadratureLine : public chi_math::QuadratureGaussLegendre
{
public:
  explicit QuadratureLine(opensn::App& app, QuadratureOrder in_order)
    : QuadratureGaussLegendre(app, in_order)
  {
    SetRange({0, 1});
  }
};
