#pragma once

#include "framework/physics/solver_base/solver.h"
#include "framework/math/petsc_utils/petsc_utils.h"
#include "modules/fv_diffusion/fv_diffusion_bndry.h"
#include "framework/utils/timer.h"
#include "framework/mesh/mesh.h"
#include <map>

// forward declaration
namespace chi_mesh
{
class MeshContinuum;
typedef std::shared_ptr<MeshContinuum> MeshContinuumPtr;
} // namespace chi_mesh
namespace chi_math
{
class SpatialDiscretization;
typedef std::shared_ptr<SpatialDiscretization> SDMPtr;
class ScalarMaterialFunction;
} // namespace chi_math

namespace fv_diffusion
{
/** FV diffusion solver
 *
 */
class Solver : public chi_physics::Solver
{
public:
  chi_mesh::MeshContinuumPtr grid_ptr_ = nullptr;

  chi_math::SDMPtr sdm_ptr_ = nullptr;

  size_t num_local_dofs_ = 0;
  size_t num_globl_dofs_ = 0;

  /// approx solution
  Vec x_ = nullptr;
  /// RHS
  Vec b_ = nullptr;
  /// linear system matrix
  Mat A_ = nullptr;

  typedef std::pair<fv_diffusion::BoundaryType, std::vector<double>> BoundaryInfo;
  typedef std::map<std::string, BoundaryInfo> BoundaryPreferences;
  BoundaryPreferences boundary_preferences_;
  std::map<uint64_t, Boundary> boundaries_;

  explicit Solver(opensn::App& app, const std::string& in_solver_name);
  ~Solver() override;

  // void Initialize() override;
  void Initialize() override;
  void Execute() override;

  /**Updates the field functions with the latest data.*/
  void UpdateFieldFunctions();

private:
  std::shared_ptr<chi_math::ScalarMaterialFunction> d_coef_function_;
  std::shared_ptr<chi_math::ScalarMaterialFunction> sigma_a_function_;
  std::shared_ptr<chi_math::ScalarMaterialFunction> q_ext_function_;
};

} // namespace fv_diffusion
