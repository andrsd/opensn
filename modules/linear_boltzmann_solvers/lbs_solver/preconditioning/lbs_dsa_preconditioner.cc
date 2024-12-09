// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "modules/linear_boltzmann_solvers/lbs_solver/preconditioning/lbs_shell_operations.h"
#include "modules/linear_boltzmann_solvers/lbs_solver/lbs_vecops.h"
#include "modules/linear_boltzmann_solvers/lbs_solver/lbs_solver.h"
#include "modules/linear_boltzmann_solvers/lbs_solver/acceleration/diffusion_mip_solver.h"
#include "modules/linear_boltzmann_solvers/lbs_solver/iterative_methods/wgs_context.h"

namespace opensn
{

int
WGDSA_TGDSA_PreConditionerMult(PC pc, Vec phi_input, Vec pc_output)
{
  void* context;
  PCShellGetContext(pc, &context);

  auto gs_context_ptr = (WGSContext*)(context);

  // Shorten some names
  LBSSolver& lbs_solver = gs_context_ptr->lbs_solver;
  LBSGroupset& groupset = gs_context_ptr->groupset;

  // Copy PETSc vector to STL
  auto& phi_new_local = gs_context_ptr->lbs_solver.GetPhiNewLocal();
  LBSVecOps::SetPrimarySTLvectorFromGSPETScVec(
    lbs_solver, groupset, phi_input, PhiSTLOption::PHI_NEW);

  // Apply WGDSA
  if (groupset.apply_wgdsa)
  {
    std::vector<double> delta_phi_local;
    lbs_solver.AssembleWGDSADeltaPhiVector(groupset, phi_new_local, delta_phi_local);

    groupset.wgdsa_solver->Assemble_b(delta_phi_local);
    groupset.wgdsa_solver->Solve(delta_phi_local);

    lbs_solver.DisAssembleWGDSADeltaPhiVector(groupset, delta_phi_local, phi_new_local);
  }
  // Apply TGDSA
  if (groupset.apply_tgdsa)
  {
    std::vector<double> delta_phi_local;
    lbs_solver.AssembleTGDSADeltaPhiVector(groupset, phi_new_local, delta_phi_local);

    groupset.tgdsa_solver->Assemble_b(delta_phi_local);
    groupset.tgdsa_solver->Solve(delta_phi_local);

    lbs_solver.DisAssembleTGDSADeltaPhiVector(groupset, delta_phi_local, phi_new_local);
  }

  // Copy STL vector to PETSc Vec
  LBSVecOps::SetGSPETScVecFromPrimarySTLvector(
    lbs_solver, groupset, pc_output, PhiSTLOption::PHI_NEW);

  return 0;
}

int
WGDSA_TGDSA_PreConditionerMult2(WGSContext& gs_context_ptr, Vec phi_input, Vec pc_output)
{
  // Shorten some names
  LBSSolver& lbs_solver = gs_context_ptr.lbs_solver;
  LBSGroupset& groupset = gs_context_ptr.groupset;

  // Copy PETSc vector to STL
  auto& phi_new_local = gs_context_ptr.lbs_solver.GetPhiNewLocal();
  LBSVecOps::SetPrimarySTLvectorFromGSPETScVec(
    lbs_solver, groupset, phi_input, PhiSTLOption::PHI_NEW);

  // Apply WGDSA
  if (groupset.apply_wgdsa)
  {
    std::vector<double> delta_phi_local;
    lbs_solver.AssembleWGDSADeltaPhiVector(groupset, phi_new_local, delta_phi_local);

    groupset.wgdsa_solver->Assemble_b(delta_phi_local);
    groupset.wgdsa_solver->Solve(delta_phi_local);

    lbs_solver.DisAssembleWGDSADeltaPhiVector(groupset, delta_phi_local, phi_new_local);
  }
  // Apply TGDSA
  if (groupset.apply_tgdsa)
  {
    std::vector<double> delta_phi_local;
    lbs_solver.AssembleTGDSADeltaPhiVector(groupset, phi_new_local, delta_phi_local);

    groupset.tgdsa_solver->Assemble_b(delta_phi_local);
    groupset.tgdsa_solver->Solve(delta_phi_local);

    lbs_solver.DisAssembleTGDSADeltaPhiVector(groupset, delta_phi_local, phi_new_local);
  }

  // Copy STL vector to PETSc Vec
  LBSVecOps::SetGSPETScVecFromPrimarySTLvector(
    lbs_solver, groupset, pc_output, PhiSTLOption::PHI_NEW);

  return 0;
}

} // namespace opensn
