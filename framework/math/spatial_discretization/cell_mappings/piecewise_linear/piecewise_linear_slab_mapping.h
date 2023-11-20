#pragma once

#include "framework/math/spatial_discretization/cell_mappings/piecewise_linear_base_mapping.h"
#include "framework/math/quadratures/quadrature_line.h"
#include "framework/mesh/cell/cell.h"
#include <array>

namespace opensn
{

/**Object for handling slab shaped piecewise linear shape functions.
 * \ingroup doc_CellMappings*/
class PieceWiseLinearSlabMapping : public PieceWiseLinearBaseMapping
{
public:
  /**Constructor for a slab view.*/
  PieceWiseLinearSlabMapping(const Cell& slab_cell,
                             const MeshContinuum& ref_grid,
                             const QuadratureLine& volume_quadrature);

  VolumetricQuadraturePointData MakeVolumetricQuadraturePointData() const override;

  SurfaceQuadraturePointData MakeSurfaceQuadraturePointData(size_t face_index) const override;

  /**
   * Define standard slab linear shape functions
   */
  double SlabShape(uint32_t index,
                   const Vector3& qpoint,
                   bool on_surface = false,
                   uint32_t edge = 0) const;

  double SlabGradShape(uint32_t index) const;

  /**
   * Actual shape functions as function of cartesian coordinates
   */
  double ShapeValue(int i, const Vector3& xyz) const override;

  Vector3 GradShapeValue(int i, const Vector3& xyz) const override;

  void ShapeValues(const Vector3& xyz, std::vector<double>& shape_values) const override;

  void GradShapeValues(const Vector3& xyz, std::vector<Vector3>& gradshape_values) const override;

private:
  Vector3 v0_;
  uint64_t v0i_;
  uint64_t v1i_;
  std::array<Normal, 2> normals_;
  const QuadratureLine& volume_quadrature_;
  double h_;
};

} // namespace opensn
