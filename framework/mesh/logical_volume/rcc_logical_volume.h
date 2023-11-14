#pragma once

#include "framework/mesh/logical_volume/logical_volume.h"

namespace chi_mesh
{

/**Right Circular Cylinder (RCC) logical volume.
 *
 * Determining whether a point is within an RCC is tricky.
 * */
class RCCLogicalVolume : public LogicalVolume
{
public:
  static chi::InputParameters GetInputParameters();
  explicit RCCLogicalVolume(opensn::App& app, const chi::InputParameters& params);

  bool Inside(const chi_mesh::Vector3& point) const override;

protected:
  double r_;
  double x0_, y0_, z0_;
  double vx_, vy_, vz_;
};

} // namespace chi_mesh
