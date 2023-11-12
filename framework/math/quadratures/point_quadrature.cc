#include "framework/math/quadratures/point_quadrature.h"

namespace chi_math
{

PointQuadrature::PointQuadrature(opensn::App& app) : Quadrature(app, QuadratureOrder::CONSTANT)
{
  qpoints_ = {{0.0, 0.0, 0.0}};
  weights_ = {1.0};
}

} // namespace chi_math
