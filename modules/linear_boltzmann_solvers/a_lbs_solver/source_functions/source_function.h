#pragma once

#include "modules/linear_boltzmann_solvers/a_lbs_solver/lbs_structs.h"

#include "framework/physics/physics_material/multi_group_xs/multi_group_xs.h"

#include <memory>
#include <utility>

namespace lbs
{
class LBSSolver;
class LBSGroupset;

/**Implements a customizable source function using virtual methods.
 * This base class will function well for steady simulations and kEigenvalue
 * simulations. It needs some customization for adjoint and transient.*/
class SourceFunction
{
protected:
  const LBSSolver& lbs_solver_;

  bool apply_fixed_src_ = false;
  bool apply_wgs_scatter_src_ = false;
  bool apply_ags_scatter_src_ = false;
  bool apply_wgs_fission_src_ = false;
  bool apply_ags_fission_src_ = false;
  bool suppress_wg_scatter_src_ = false;

  size_t gs_i_ = 0;
  size_t gs_f_ = 0;
  size_t first_grp_ = 0;
  size_t last_grp_ = 0;

  double cell_volume_ = 0.0;
  size_t g_ = 0;
  const double* fixed_src_moments_ = nullptr;
  std::vector<double> default_zero_src_;

public:
  /**Constructor.*/
  explicit SourceFunction(const LBSSolver& lbs_solver);
  virtual ~SourceFunction() = default;

  /**Sets the source moments for the groups in the current group set.
   *
   * \param groupset The groupset the under consideration.
   * \param destination_q A vector to contribute the source to.
   * \param phi_local The primary STL vector to operate off.
   * \param source_flags Flags for adding specific terms into the
   *        destination vector. Available flags are for applying
   *        the material source, across/within-group scattering,
   *        and across/within-groups fission.
   *
   */
  virtual void operator()(LBSGroupset& groupset,
                          std::vector<double>& destination_q,
                          const std::vector<double>& phi,
                          int source_flags);

  virtual double AddSourceMoments() const;

  typedef std::vector<chi_physics::MultiGroupXS::Precursor> PrecursorList;
  /**Adds delayed particle precursor sources.*/
  virtual double AddDelayedFission(const PrecursorList& precursors,
                                   const std::vector<double>& nu_delayed_sigma_f,
                                   const double* phi) const;

  virtual void AddAdditionalSources(LBSGroupset& groupset,
                                    std::vector<double>& destination_q,
                                    const std::vector<double>& phi,
                                    int source_flags)
  {
    AddPointSources(groupset, destination_q, phi, source_flags);
  }

  /**Adds point sources to the source moments.*/
  void AddPointSources(LBSGroupset& groupset,
                       std::vector<double>& destination_q,
                       const std::vector<double>& phi,
                       int source_flags);
};

} // namespace lbs
