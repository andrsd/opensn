#pragma once

#include "framework/math/spatial_discretization/spatial_discretization.h"
#include "framework/math/unknown_manager/unknown_manager.h"
#include "framework/math/spatial_discretization/finite_element/quadrature_point_data.h"

namespace opensn
{

/**Base Finite Element spatial discretization class.
 * \ingroup doc_SpatialDiscretization*/
class FiniteElementBase : public SpatialDiscretization
{
public:
  QuadratureOrder GetQuadratureOrder() const;

protected:
  explicit FiniteElementBase(const MeshContinuum& grid,
                             CoordinateSystemType cs_type,
                             SDMType sdm_type,
                             QuadratureOrder q_order)
    : SpatialDiscretization(grid, cs_type, sdm_type), q_order_(q_order)
  {
  }

  const QuadratureOrder q_order_;
};

} // namespace opensn
