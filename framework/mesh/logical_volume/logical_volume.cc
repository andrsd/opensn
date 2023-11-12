#include "framework/mesh/logical_volume/logical_volume.h"

namespace chi_mesh
{

chi::InputParameters
LogicalVolume::GetInputParameters()
{
  return ChiObject::GetInputParameters();
}

LogicalVolume::LogicalVolume(opensn::App& app, const chi::InputParameters& params)
  : ChiObject(app, params)
{
}

} // namespace chi_mesh
