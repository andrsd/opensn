#pragma once

#include "framework/math/nonlinear_solver/nonlinear_solver.h"

#include "modules/linear_boltzmann_solvers/a_lbs_solver/acceleration/nl_keigen_acc_context.h"

#include <petscsnes.h>

namespace opensn
{
namespace lbs
{

class NLKEigenDiffSolver : public NonLinearSolver
{
public:
  typedef std::shared_ptr<NLKEigenDiffContext> NLKEigenDiffContextPtr;

  explicit NLKEigenDiffSolver(NLKEigenDiffContextPtr nlk_diff_context_ptr)
    : NonLinearSolver(nlk_diff_context_ptr)
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

} // namespace lbs
} // namespace opensn
