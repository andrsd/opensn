#include "modules/LinearBoltzmannSolvers/Executors/lbs_steady_state.h"

#include "framework/ChiObjectFactory.h"

#include "modules/LinearBoltzmannSolvers/A_LBSSolver/IterativeMethods/ags_linear_solver.h"

namespace lbs
{

RegisterChiObject(lbs, SteadyStateSolver);

chi::InputParameters
SteadyStateSolver::GetInputParameters()
{
  chi::InputParameters params = chi_physics::Solver::GetInputParameters();

  params.SetGeneralDescription(
    "Generalized implementation of a steady state solver. This solver calls"
    " the Across-Groupset (AGS) solver for the lbs-data block.");
  params.SetDocGroup("LBSExecutors");

  params.ChangeExistingParamToOptional("name", "SteadyStateSolver");

  params.AddRequiredParameter<size_t>("lbs_solver_handle", "Handle to an existing lbs solver");

  return params;
}

SteadyStateSolver::SteadyStateSolver(const chi::InputParameters& params)
  : chi_physics::Solver(params),
    lbs_solver_(Chi::GetStackItem<LBSSolver>(Chi::object_stack,
                                             params.GetParamValue<size_t>("lbs_solver_handle")))
{
}

void
SteadyStateSolver::Initialize()
{
  lbs_solver_.Initialize();
}

void
SteadyStateSolver::Execute()
{
  auto& ags_solver = *lbs_solver_.GetPrimaryAGSSolver();

  ags_solver.Setup();
  ags_solver.Solve();

  if (lbs_solver_.Options().use_precursors) lbs_solver_.ComputePrecursors();

  lbs_solver_.UpdateFieldFunctions();
}

} // namespace lbs
