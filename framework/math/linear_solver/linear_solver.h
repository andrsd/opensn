#pragma once

#include "framework/math/linear_solver/linear_solver_context.h"
#include <string>
#include <utility>
#include <memory>
#include <petscksp.h>

namespace chi_math
{

struct LinearSolverContext;

/**
 * Linear solver
 *
 * Wrapper around PETSc KSP
 */
class LinearSolver
{
public:
  typedef std::shared_ptr<LinearSolverContext> LinSolveContextPtr;

  struct ToleranceOptions
  {
    double residual_relative = 1.0e-50;
    double residual_absolute = 1.0e-6;
    double residual_divergence = 1.0e6;
    int maximum_iterations = 100;
    int gmres_restart_interval = 100;
    double gmres_breakdown_tolerance = 1.0e6;
  } tolerance_options_;

  LinearSolver(const std::string& iterative_method, LinSolveContextPtr context_ptr);

  LinearSolver(const std::string& solver_name,
               const std::string& iterative_method,
               LinSolveContextPtr context_ptr);

  virtual ~LinearSolver();

  ToleranceOptions& ToleranceOptions() { return tolerance_options_; }
  void ApplyToleranceOptions();

  LinSolveContextPtr& GetContext() { return context_ptr_; }

  /**Sets a flag to suppress the KSPSolve() method from being called.*/
  void SetKSPSolveSuppressionFlag(bool flag) { suppress_kspsolve_ = flag; }
  bool GetKSPSolveSuppressionFlag() const { return suppress_kspsolve_; }

  /**
   * Set up the linaer solver
   */
  virtual void Setup();

  /**
   * Solve the system
   */
  virtual void Solve();

protected:
  bool IsSystemSet() const { return system_set_; }
  virtual void PreSetupCallback();
  virtual void SetOptions();
  virtual void SetSolverContext();
  virtual void SetConvergenceTest();
  virtual void SetMonitor();
  virtual void SetPreconditioner();
  virtual void SetSystemSize() = 0;
  virtual void SetSystem() = 0;
  virtual void PostSetupCallback();
  virtual void PreSolveCallback();
  virtual void SetInitialGuess() = 0;
  virtual void SetRHS() = 0;
  virtual void PostSolveCallback();

  /// Solver name
  const std::string solver_name_;
  const std::string iterative_method_;

  LinSolveContextPtr context_ptr_ = nullptr;

  /// Matrix
  Mat A_;
  /// Right-hand side
  Vec b_;
  /// Solution vector
  Vec x_;
  /// PETSc KSP object
  KSP ksp_;
  /// Number of local DOFs in the system
  int64_t num_local_dofs_ = 0;
  /// Number of global DOFs in the system
  int64_t num_globl_dofs_ = 0;

private:
  bool system_set_ = false;
  bool suppress_kspsolve_ = false;
};

} // namespace chi_math
