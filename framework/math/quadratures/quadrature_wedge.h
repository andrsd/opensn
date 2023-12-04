#pragma once

#include "framework/math/quadratures/quadrature.h"

namespace opensn
{

/**Quadrature for a wedge (extruded triangle). This is a simple product
 * of a triangle quadrature and Gauss-Legendre line quadrature.*/
class QuadratureWedge : public Quadrature
{
public:
  // Constructor
  explicit QuadratureWedge(QuadratureOrder order);
};

} // namespace opensn
