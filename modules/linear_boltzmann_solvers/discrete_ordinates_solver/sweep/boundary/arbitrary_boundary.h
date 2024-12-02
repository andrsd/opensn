// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

#include "modules/linear_boltzmann_solvers/discrete_ordinates_solver/sweep/boundary/sweep_boundary.h"
#include "framework/mesh/mesh.h"
#include "framework/math/math.h"
#include <vector>
#include <limits>

namespace opensn
{

/// Specified incident fluxes on a boundary.
class ArbitraryBoundary : public SweepBoundary
{
private:
  std::unique_ptr<BoundaryFunction> boundary_function_;
  const uint64_t boundary_id_;

  using FaceData = std::vector<std::vector<double>>;
  using CellData = std::vector<FaceData>;
  std::vector<CellData> local_cell_data_;

public:
  explicit ArbitraryBoundary(size_t num_groups,
                             std::unique_ptr<BoundaryFunction> bndry_function,
                             uint64_t boundary_id,
                             CoordinateSystemType coord_type = CoordinateSystemType::CARTESIAN)
    : SweepBoundary(LBSBoundaryType::ARBITRARY, num_groups, coord_type),
      boundary_function_(std::move(bndry_function)),
      boundary_id_(boundary_id)
  {
  }

  double* PsiIncoming(uint64_t cell_local_id,
                      unsigned int face_num,
                      unsigned int fi,
                      unsigned int angle_num,
                      int group_num,
                      size_t gs_ss_begin) override;

  void Setup(const MeshContinuum& grid, const AngularQuadrature& quadrature) override;
};

} // namespace opensn
