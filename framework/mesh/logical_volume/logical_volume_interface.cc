#include "framework/mesh/logical_volume/logical_volume_interface.h"
#include "framework/mesh/logical_volume/logical_volume.h"
#include "framework/app.h"

namespace chi_mesh
{

chi::InputParameters
LogicalVolumeInterface::GetInputParameters()
{
  chi::InputParameters params;

  params.AddOptionalParameter("logical_volume", 0, "Handle to a logical_volume.");

  return params;
}

LogicalVolumeInterface::LogicalVolumeInterface(opensn::App& app, const chi::InputParameters& params)
  : logical_volume_(params.ParametersAtAssignment().Has("logical_volume")
                      ? app.GetStackObject<LogicalVolume>(
                          params.GetParamValue<size_t>("logical_volume"), __FUNCTION__)
                      : std::shared_ptr<LogicalVolume>(nullptr))
{
}

const std::shared_ptr<LogicalVolume>
LogicalVolumeInterface::GetLogicalVolume() const
{
  return logical_volume_;
}

} // namespace chi_mesh
