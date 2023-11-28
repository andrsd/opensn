#pragma once

#include "framework/physics/solver_base/solver.h"
#include "framework/math/math.h"
#include "framework/math/dynamic_matrix.h"
#include "framework/math/dynamic_vector.h"

namespace opensn
{
namespace prk
{
/**General transient solver for point kinetics.

* */
class TransientSolver : public opensn::Solver
{
private:
  std::vector<double> lambdas_;
  std::vector<double> betas_;
  double gen_time_;
  double rho_;
  double source_strength_;
  std::string time_integration_;

  size_t num_precursors_;
  DynamicMatrix<double> A_, I_;
  DynamicVector<double> x_t_, x_tp1_, q_;
  double beta_ = 1.0;
  double period_tph_ = 0.0;

public:
  /**Sets input parameters.*/
  static InputParameters GetInputParameters();
  /**Constructor.*/
  explicit TransientSolver(const InputParameters& params);

  void Initialize() override;
  void Execute() override;
  void Step() override;
  void Advance() override;

  ParameterBlock GetInfo(const ParameterBlock& params) const override;

  // Getters and Setters

  /**Returns the population at the previous time step.*/
  double PopulationPrev() const;
  /**Returns the population at the next time step.*/
  double PopulationNew() const;
  /**Returns the period computed for the last time step.*/
  double Period() const;
  /**Returns the time computed for the last time step.*/
  double TimePrev() const;
  /**Returns the time computed for the next time step.*/
  double TimeNew() const;
  /**Returns the solution at the previous time step.*/
  std::vector<double> SolutionPrev() const;
  /**Returns the solution at the next time step.*/
  std::vector<double> SolutionNew() const;

  /**\addtogroup prk
   *
   * \section Properties Properties that can be set
   * The following properties can be set via the lua call
   * `chiSolverSetProperties`
   * \copydoc prk::TransientSolver::SetProperties
   *
   * PRK Transient solver settable properties:
   * - `rho`, The current reactivity
   *
   * Parents:
   * \copydoc opensn::Solver::SetProperties
   */
  void SetProperties(const ParameterBlock& params) override;

  /**Sets the value of rho.*/
  void SetRho(double value);
};

} // namespace prk
} // namespace opensn
