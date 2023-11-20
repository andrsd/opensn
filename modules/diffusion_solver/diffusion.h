#pragma once

#include <petscksp.h>

/**\defgroup LuaDiffusion Diffusion Solver
 * \ingroup LuaModules*/

namespace opensn
{
namespace diffusion
{
class Solver;
class Boundary;
class BoundaryDirichlet;
class BoundaryReflecting;
class BoundaryRobin;

/**Customized monitor for PETSc Krylov sub-space solvers.*/
PetscErrorCode KSPMonitorAChiTech(KSP ksp, PetscInt n, PetscReal rnorm, void* monitordestroy);

/**Customized convergence test.*/
PetscErrorCode DiffusionConvergenceTestNPT(
  KSP ksp, PetscInt n, PetscReal rnorm, KSPConvergedReason* convergedReason, void* monitordestroy);
} // namespace diffusion
} // namespace opensn
