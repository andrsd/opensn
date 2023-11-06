#pragma once

#include "opensn/framework/mesh/LogicalVolume/LogicalVolume.h"

namespace chi_mesh
{

// ###################################################################
/**Spherical logical volume.*/
class SphereLogicalVolume : public LogicalVolume
{
public:
  static chi::InputParameters GetInputParameters();
  explicit SphereLogicalVolume(const chi::InputParameters& params);

  bool Inside(const chi_mesh::Vector3& point) const override;

protected:
  double r_;
  double x0_, y0_, z0_;
};

} // namespace chi_mesh
