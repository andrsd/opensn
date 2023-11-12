#include "modules/linear_boltzmann_solvers/executors/lbs_transient.h"
#include "framework/app.h"
#include "framework/object_factory.h"
#include "framework/math/time_integrations/time_integration.h"

namespace lbs
{

RegisterChiObject(lbs, TransientSolver);

chi::InputParameters
TransientSolver::GetInputParameters()
{
  chi::InputParameters params = chi_physics::Solver::GetInputParameters();

  params.SetGeneralDescription("Generalized implementation of a transient solver. This solver calls"
                               " the Across-Groupset (AGS) solver for the lbs-data block.");
  params.SetDocGroup("LBSExecutors");

  params.ChangeExistingParamToOptional("name", "TransientSolver");

  params.AddRequiredParameter<size_t>("lbs_solver_handle", "Handle to an existing lbs solver");

  params.AddRequiredParameter<size_t>("time_integration",
                                      "Handle to a time integration scheme to use");

  return params;
}

TransientSolver::TransientSolver(opensn::App& app, const chi::InputParameters& params)
  : chi_physics::Solver(app, params),
    lbs_solver_(App().GetStackObject<LBSSolver>(params.GetParamValue<size_t>("lbs_solver_handle"))),
    time_integration_(App().GetStackObject<chi_math::TimeIntegration>(
      params.GetParamValue<size_t>("time_integration")))
{
}

void
TransientSolver::Initialize()
{
  lbs_solver_->Initialize();
}

void
TransientSolver::Execute()
{
}

void
TransientSolver::Step()
{
}

void
TransientSolver::Advance()
{
}

} // namespace lbs
