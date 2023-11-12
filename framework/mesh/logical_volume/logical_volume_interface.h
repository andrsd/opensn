#pragma once

#include "framework/parameters/input_parameters.h"
#include "framework/app.h"

namespace chi_mesh
{

class LogicalVolume;

/**Interface class to add a dependency on a logical volume. Two things need to
 * be done to use this interface. 1) Derive from it. 2) Add its parameters to
 * the child class. Now it will require a handle to logical volume in the input
 * language.*/
class LogicalVolumeInterface
{
protected:
  static chi::InputParameters GetInputParameters();

  explicit LogicalVolumeInterface(opensn::App& app, const chi::InputParameters& params);

  const std::shared_ptr<LogicalVolume> GetLogicalVolume() const;

private:
  const std::shared_ptr<LogicalVolume> logical_volume_;
};

} // namespace chi_mesh
