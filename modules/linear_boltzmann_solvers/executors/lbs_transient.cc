// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "modules/linear_boltzmann_solvers/executors/lbs_transient.h"
#include "framework/object_factory.h"
#include "framework/math/time_integrations/time_integration.h"

namespace opensn
{

OpenSnRegisterObjectInNamespace(lbs, TransientSolver);

InputParameters
TransientSolver::GetInputParameters()
{
  InputParameters params = opensn::Solver::GetInputParameters();

  params.SetGeneralDescription("Generalized implementation of a transient solver. This solver calls"
                               " the Across-Groupset (AGS) solver for the lbs-data block.");
  params.SetDocGroup("LBSExecutors");

  params.ChangeExistingParamToOptional("name", "TransientSolver");

  params.AddRequiredParameter<std::shared_ptr<Solver>>("lbs_solver", "An existing lbs solver");

  params.AddRequiredParameter<size_t>("time_integration",
                                      "Handle to a time integration scheme to use");

  return params;
}

TransientSolver::TransientSolver(const InputParameters& params)
  : opensn::Solver(params),
    lbs_solver_(std::dynamic_pointer_cast<LBSSolver>(
      params.GetParamValue<std::shared_ptr<Solver>>("lbs_solver"))),
    time_integration_(GetStackItemPtrAsType<TimeIntegration>(
      object_stack, params.GetParamValue<size_t>("time_integration")))
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

} // namespace opensn
