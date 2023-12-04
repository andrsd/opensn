#include "modules/linear_boltzmann_solvers/a_lbs_solver/preconditioning/lbs_shell_operations.h"

#include "modules/linear_boltzmann_solvers/a_lbs_solver/lbs_solver.h"
#include "modules/linear_boltzmann_solvers/a_lbs_solver/acceleration/diffusion_mip_solver.h"
#include "modules/linear_boltzmann_solvers/a_lbs_solver/iterative_methods/wgs_context.h"

namespace opensn
{
namespace lbs
{

int
MIP_TGDSA_PreConditionerMult(PC pc, Vec phi_input, Vec pc_output)
{
  void* context;
  PCShellGetContext(pc, &context);

  auto gs_context_ptr = (WGSContext*)(context);

  // Shorten some names
  LBSSolver& solver = gs_context_ptr->lbs_solver_;
  LBSGroupset& groupset = gs_context_ptr->groupset_;

  // Copy PETSc vector to STL
  auto& phi_delta = gs_context_ptr->lbs_solver_.PhiNewLocal();
  solver.SetPrimarySTLvectorFromGSPETScVec(groupset, phi_input, PhiSTLOption::PHI_NEW);

  // Apply TGDSA
  if (groupset.apply_tgdsa_)
  {
    std::vector<double> delta_phi_local;
    solver.AssembleTGDSADeltaPhiVector(groupset, phi_delta, delta_phi_local);
    groupset.tgdsa_solver_->Assemble_b(delta_phi_local);
    groupset.tgdsa_solver_->Solve(delta_phi_local);
    solver.DisAssembleTGDSADeltaPhiVector(groupset, delta_phi_local, phi_delta);
  }

  // Copy STL vector to PETSc Vec
  solver.SetGSPETScVecFromPrimarySTLvector(groupset, pc_output, PhiSTLOption::PHI_NEW);

  return 0;
}

} // namespace lbs
} // namespace opensn
