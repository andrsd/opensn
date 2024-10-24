// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

#include "framework/math/quadratures/spatial/spatial_quadrature.h"
#include "framework/math/quadratures/gausslegendre_quadrature.h"

namespace opensn
{

/**Quadrature for use on reference lines.*/
class LineQuadrature : public SpatialQuadrature
{
private:
  /**Interval on which the quadrature is defined
   * (relevant for one-dimensional quadratures only).*/
  std::pair<double, double> range_;

public:
  explicit LineQuadrature(QuadratureOrder order);

  /**Get the range on which the quadrature is defined.*/
  const std::pair<double, double>& GetRange() const { return range_; }

  /**Set the range on which the quadrature is defined.
   * Note that calling this method results in translation
   * of the abscissae and scaling of the weights.*/
  void SetRange(const std::pair<double, double>& range);
};

} // namespace opensn
