#pragma once

#include "framework/math/nonlinear_solver/nonlinear_solver.h"

#include "modules/linear_boltzmann_solvers/a_lbs_solver/acceleration/nl_keigen_acc_context.h"

#include <petscsnes.h>

namespace lbs::acceleration
{

class NLKEigenDiffSolver : public chi_math::NonLinearSolver
{
public:
  typedef std::shared_ptr<NLKEigenDiffContext> NLKEigenDiffContextPtr;

  explicit NLKEigenDiffSolver(opensn::App& app, NLKEigenDiffContextPtr nlk_diff_context_ptr)
    : chi_math::NonLinearSolver(app, nlk_diff_context_ptr)
  {
  }

  ~NLKEigenDiffSolver() override = default;

protected:
  void SetMonitor() override;

  void SetSystemSize() override;
  void SetSystem() override;
  void SetFunction() override;
  void SetJacobian() override;

protected:
  void SetInitialGuess() override;
  void PostSolveCallback() override;
};

} // namespace lbs::acceleration
