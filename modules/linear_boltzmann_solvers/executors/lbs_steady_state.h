#pragma once

#include "modules/linear_boltzmann_solvers/a_lbs_solver/lbs_solver.h"

namespace lbs
{

class SteadyStateSolver : public chi_physics::Solver
{
protected:
  std::shared_ptr<LBSSolver> lbs_solver_;

public:
  static chi::InputParameters GetInputParameters();

  explicit SteadyStateSolver(opensn::App& app, const chi::InputParameters& params);

  void Initialize() override;
  void Execute() override;
};

} // namespace lbs
