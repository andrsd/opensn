#pragma once

#include "framework/math/nonlinear_solver/nonlinear_solver.h"
#include "modules/linear_boltzmann_solvers/a_lbs_solver/iterative_methods/nl_keigen_ags_context.h"

namespace lbs
{

class NLKEigenvalueAGSSolver : public chi_math::NonLinearSolver
{
public:
  typedef std::shared_ptr<NLKEigenAGSContext> NLKAGSContextPtr;

  explicit NLKEigenvalueAGSSolver(opensn::App& app, NLKAGSContextPtr nlk_ags_context_ptr)
    : chi_math::NonLinearSolver(app, nlk_ags_context_ptr)
  {
  }

  ~NLKEigenvalueAGSSolver() override = default;

protected:
  void PreSetupCallback() override;
  void SetMonitor() override;

  void SetSystemSize() override;
  void SetSystem() override;
  void SetFunction() override;
  void SetJacobian() override;

protected:
  void SetInitialGuess() override;
  void PostSolveCallback() override;
};

} // namespace lbs
