#include "modules/linear_boltzmann_solvers/executors/pi_keigen.h"
#include "framework/object_factory.h"
#include "modules/linear_boltzmann_solvers/a_lbs_solver/iterative_methods/ags_linear_solver.h"
#include "framework/app.h"
#include "framework/logging/log.h"
#include "framework/logging/log_exceptions.h"
#include "framework/utils/timer.h"
#include <iomanip>

namespace lbs
{

RegisterChiObject(lbs, XXPowerIterationKEigen);

chi::InputParameters
XXPowerIterationKEigen::GetInputParameters()
{
  chi::InputParameters params = chi_physics::Solver::GetInputParameters();

  params.SetGeneralDescription("Generalized implementation of a k-Eigenvalue solver using Power "
                               "Iteration.");
  params.SetDocGroup("LBSExecutors");

  params.ChangeExistingParamToOptional("name", "XXPowerIterationKEigen");

  params.AddRequiredParameter<size_t>("lbs_solver_handle", "Handle to an existing lbs solver");
  params.AddOptionalParameter("max_iters", 1000, "Maximum power iterations allowed");
  params.AddOptionalParameter("k_tol", 1.0e-10, "Tolerance on the k-eigenvalue");
  params.AddOptionalParameter(
    "reset_solution",
    true,
    "Flag, if set to true will initialize the phi-solution to all 1's before "
    "executing");

  params.AddOptionalParameter(
    "reinit_phi_1", true, "If true, reinitializes scalar phi fluxes to 1");

  return params;
}

XXPowerIterationKEigen::XXPowerIterationKEigen(opensn::App& app, const chi::InputParameters& params)
  : chi_physics::Solver(app, params),
    lbs_solver_(App().GetStackObject<LBSSolver>(params.GetParamValue<size_t>("lbs_solver_handle"))),
    max_iters_(params.GetParamValue<size_t>("max_iters")),
    k_tolerance_(params.GetParamValue<double>("k_tol")),
    reinit_phi_1_(params.GetParamValue<bool>("reinit_phi_1")),

    q_moments_local_(lbs_solver_->QMomentsLocal()),
    phi_old_local_(lbs_solver_->PhiOldLocal()),
    phi_new_local_(lbs_solver_->PhiNewLocal()),
    groupsets_(lbs_solver_->Groupsets()),
    front_gs_(groupsets_.front())
{
}

void
XXPowerIterationKEigen::Initialize()
{
  lbs_solver_->Initialize();

  active_set_source_function_ = lbs_solver_->GetActiveSetSourceFunction();
  primary_ags_solver_ = lbs_solver_->GetPrimaryAGSSolver();

  for (auto& wgs_solver : lbs_solver_->GetWGSSolvers())
  {
    auto context = wgs_solver->GetContext();
    auto wgs_context = std::dynamic_pointer_cast<lbs::WGSContext>(context);

    ChiLogicalErrorIf(not wgs_context, ": Cast failed");

    wgs_context->lhs_src_scope_.Unset(APPLY_WGS_FISSION_SOURCES); // lhs_scope
    wgs_context->rhs_src_scope_.Unset(APPLY_AGS_FISSION_SOURCES); // rhs_scope
  }

  primary_ags_solver_->SetVerbosity(lbs_solver_->Options().verbose_ags_iterations);

  front_wgs_solver_ = lbs_solver_->GetWGSSolvers().at(front_gs_.id_);
  front_wgs_context_ = std::dynamic_pointer_cast<lbs::WGSContext>(front_wgs_solver_->GetContext());

  ChiLogicalErrorIf(not front_wgs_context_, ": Casting failure");

  if (reinit_phi_1_) lbs_solver_->SetPhiVectorScalarValues(phi_old_local_, 1.0);
}

void
XXPowerIterationKEigen::Execute()
{
  using namespace chi_math;

  double F_prev = 1.0;
  k_eff_ = 1.0;
  double k_eff_prev = 1.0;
  double k_eff_change = 1.0;

  // Start power iterations
  int nit = 0;
  bool converged = false;
  while (nit < max_iters_)
  {
    // Set the fission source
    SetLBSFissionSource(phi_old_local_, false);
    Scale(q_moments_local_, 1.0 / k_eff_);

    // This solves the inners for transport
    primary_ags_solver_->Setup();
    primary_ags_solver_->Solve();

    // Recompute k-eigenvalue
    double F_new = lbs_solver_->ComputeFissionProduction(phi_new_local_);
    k_eff_ = F_new / F_prev * k_eff_;
    double reactivity = (k_eff_ - 1.0) / k_eff_;

    // Check convergence, bookkeeping
    k_eff_change = fabs(k_eff_ - k_eff_prev) / k_eff_;
    k_eff_prev = k_eff_;
    F_prev = F_new;
    nit += 1;

    if (k_eff_change < std::max(k_tolerance_, 1.0e-12)) converged = true;

    // Print iteration summary
    if (lbs_solver_->Options().verbose_outer_iterations)
    {
      std::stringstream k_iter_info;
      k_iter_info << App().ProgramTimer().GetTimeString() << " "
                  << "  Iteration " << std::setw(5) << nit << "  k_eff " << std::setw(11)
                  << std::setprecision(7) << k_eff_ << "  k_eff change " << std::setw(12)
                  << k_eff_change << "  reactivity " << std::setw(10) << reactivity * 1e5;
      if (converged) k_iter_info << " CONVERGED\n";

      App().Log().Log() << k_iter_info.str();
    }

    if (converged) break;
  } // for k iterations

  // Print summary
  App().Log().Log() << "\n";
  App().Log().Log() << "        Final k-eigenvalue    :        " << std::setprecision(7) << k_eff_;
  App().Log().Log() << "        Final change          :        " << std::setprecision(6)
                    << k_eff_change
                    << " (num_TrOps:" << front_wgs_context_->counter_applications_of_inv_op_ << ")"
                    << "\n";
  App().Log().Log() << "\n";

  if (lbs_solver_->Options().use_precursors)
  {
    lbs_solver_->ComputePrecursors();
    chi_math::Scale(lbs_solver_->PrecursorsNewLocal(), 1.0 / k_eff_);
  }

  lbs_solver_->UpdateFieldFunctions();

  App().Log().Log() << "LinearBoltzmann::KEigenvalueSolver execution completed\n\n";
}

void
XXPowerIterationKEigen::SetLBSFissionSource(const VecDbl& input, const bool additive)
{
  if (not additive) chi_math::Set(q_moments_local_, 0.0);
  active_set_source_function_(
    front_gs_, q_moments_local_, input, APPLY_AGS_FISSION_SOURCES | APPLY_WGS_FISSION_SOURCES);
}

void
XXPowerIterationKEigen::SetLBSScatterSource(const VecDbl& input,
                                            const bool additive,
                                            const bool suppress_wg_scat)
{
  if (not additive) chi_math::Set(q_moments_local_, 0.0);
  Source src = APPLY_AGS_SCATTER_SOURCES | APPLY_WGS_SCATTER_SOURCES;
  if (suppress_wg_scat) src |= SUPPRESS_WG_SCATTER;
  active_set_source_function_(front_gs_, q_moments_local_, input, src);
}

} // namespace lbs
