#pragma once

#include <vector>
#include <set>
#include <cstddef>
#include <cstdint>

#include "framework/mesh/sweep_utilities/fluds/fluds_common_data.h"

namespace opensn
{
class App;
}

namespace chi_mesh
{
class GridFaceHistogram;

} // namespace chi_mesh

namespace chi_mesh::sweep_management
{

class SPDS;

class FLUDS
{
public:
  FLUDS(size_t num_groups, size_t num_angles, const SPDS& spds);

  const SPDS& GetSPDS() const { return spds_; }

  opensn::App& App() const;

  virtual void ClearLocalAndReceivePsi() {}
  virtual void ClearSendPsi() {}
  virtual void AllocateInternalLocalPsi(size_t num_grps, size_t num_angles) {}
  virtual void AllocateOutgoingPsi(size_t num_grps, size_t num_angles, size_t num_loc_sucs) {}

  virtual void AllocateDelayedLocalPsi(size_t num_grps, size_t num_angles) {}
  virtual void AllocatePrelocIOutgoingPsi(size_t num_grps, size_t num_angles, size_t num_loc_deps)
  {
  }
  virtual void
  AllocateDelayedPrelocIOutgoingPsi(size_t num_grps, size_t num_angles, size_t num_loc_deps)
  {
  }

  virtual std::vector<double>& DelayedLocalPsi() = 0;
  virtual std::vector<double>& DelayedLocalPsiOld() = 0;

  virtual std::vector<std::vector<double>>& DeplocIOutgoingPsi() = 0;

  virtual std::vector<std::vector<double>>& PrelocIOutgoingPsi() = 0;

  virtual std::vector<std::vector<double>>& DelayedPrelocIOutgoingPsi() = 0;

  virtual std::vector<std::vector<double>>& DelayedPrelocIOutgoingPsiOld() = 0;

  virtual ~FLUDS() = default;

protected:
  const size_t num_groups_;
  const size_t num_angles_;
  const size_t num_groups_and_angles_;
  const SPDS& spds_;
};

} // namespace chi_mesh::sweep_management
