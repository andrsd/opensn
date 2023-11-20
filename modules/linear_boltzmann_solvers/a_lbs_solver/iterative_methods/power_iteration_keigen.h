#pragma once

namespace opensn
{
namespace lbs
{

class LBSSolver;

void
PowerIterationKEigen(LBSSolver& lbs_solver, double tolerance, int max_iterations, double& k_eff);
void
PowerIterationKEigen1(LBSSolver& lbs_solver, double tolerance, int max_iterations, double& k_eff);
void
PowerIterationKEigen2(LBSSolver& lbs_solver, double tolerance, int max_iterations, double& k_eff);

} // namespace lbs
} // namespace opensn
