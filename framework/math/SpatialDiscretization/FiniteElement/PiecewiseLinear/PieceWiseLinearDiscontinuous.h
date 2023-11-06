#pragma once

#include "framework/math/SpatialDiscretization/FiniteElement/PiecewiseLinear/PieceWiseLinearBase.h"
#include "framework/math/SpatialDiscretization/CellMappings/PieceWiseLinearBaseMapping.h"

namespace chi_math::spatial_discretization
{

/**
 * Generalization of the Galerkin Finite Element Method with piecewise linear basis functions
 * for use by a Discontinuous Finite Element Method (DFEM).
 * \ingroup doc_SpatialDiscretization
 */
class PieceWiseLinearDiscontinuous : public PieceWiseLinearBase
{
public:
  /**
   * Construct a shared object using the protected constructor.
   */
  static std::shared_ptr<PieceWiseLinearDiscontinuous>
  New(const chi_mesh::MeshContinuum& grid,
      QuadratureOrder q_order = QuadratureOrder::SECOND,
      CoordinateSystemType cs_type = CoordinateSystemType::CARTESIAN);

  void BuildSparsityPattern(std::vector<int64_t>& nodal_nnz_in_diag,
                            std::vector<int64_t>& nodal_nnz_off_diag,
                            const UnknownManager& unknown_manager) const override;

  int64_t MapDOF(const chi_mesh::Cell& cell,
                 unsigned int node,
                 const UnknownManager& unknown_manager,
                 unsigned int unknown_id,
                 unsigned int component) const override;

  int64_t MapDOFLocal(const chi_mesh::Cell& cell,
                      unsigned int node,
                      const UnknownManager& unknown_manager,
                      unsigned int unknown_id,
                      unsigned int component) const override;

  int64_t MapDOF(const chi_mesh::Cell& cell, unsigned int node) const override
  {
    return MapDOF(cell, node, UNITARY_UNKNOWN_MANAGER, 0, 0);
  }

  int64_t MapDOFLocal(const chi_mesh::Cell& cell, unsigned int node) const override
  {
    return MapDOFLocal(cell, node, UNITARY_UNKNOWN_MANAGER, 0, 0);
  }

  size_t GetNumGhostDOFs(const UnknownManager& unknown_manager) const override;

  std::vector<int64_t> GetGhostDOFIndices(const UnknownManager& unknown_manager) const override;

protected:
  /**
   * Reorders the nodes for parallel computation in a Continuous Finite Element calculation.
   */
  void OrderNodes();

  std::vector<int64_t> cell_local_block_address_;
  std::vector<std::pair<uint64_t, int64_t>> neighbor_cell_block_address_;

private:
  explicit PieceWiseLinearDiscontinuous(const chi_mesh::MeshContinuum& grid,
                                        QuadratureOrder q_order,
                                        CoordinateSystemType cs_type);
};

} // namespace chi_math::spatial_discretization
