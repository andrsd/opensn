#pragma once

#include "modules/linear_boltzmann_solvers/a_lbs_solver/lbs_solver.h"

namespace lbs
{

class DiffusionDFEMSolver : public LBSSolver
{
protected:
  typedef std::shared_ptr<acceleration::DiffusionMIPSolver> MIPSolverPtr;

public:
  std::vector<MIPSolverPtr> gs_mip_solvers_;

public:
  explicit DiffusionDFEMSolver(opensn::App& app, const chi::InputParameters& params);
  ~DiffusionDFEMSolver() override;
  void Initialize() override;
  void InitializeWGSSolvers() override;

public:
  static chi::InputParameters GetInputParameters();
};

} // namespace lbs
