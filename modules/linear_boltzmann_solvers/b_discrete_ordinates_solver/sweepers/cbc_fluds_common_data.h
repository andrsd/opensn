#pragma once

#include "framework/mesh/sweep_utilities/fluds/fluds_common_data.h"

#include <cinttypes>

namespace lbs
{

class CBC_FLUDSCommonData : public chi_mesh::sweep_management::FLUDSCommonData
{
public:
  CBC_FLUDSCommonData(
    const chi_mesh::sweep_management::SPDS& spds,
    const std::vector<chi_mesh::sweep_management::CellFaceNodalMapping>& grid_nodal_mappings);
};

} // namespace lbs