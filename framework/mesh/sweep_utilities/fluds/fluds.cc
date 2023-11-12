#include "framework/mesh/sweep_utilities/fluds/fluds.h"
#include "framework/mesh/sweep_utilities/spds/spds.h"

namespace chi_mesh::sweep_management
{

FLUDS::FLUDS(size_t num_groups, size_t num_angles, const SPDS& spds)
  : num_groups_(num_groups),
    num_angles_(num_angles),
    num_groups_and_angles_(num_groups_ * num_angles_),
    spds_(spds)
{
}

opensn::App&
FLUDS::App() const
{
  return spds_.App();
}

} // namespace chi_mesh::sweep_management
