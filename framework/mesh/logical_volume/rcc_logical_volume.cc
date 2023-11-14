#include "framework/mesh/logical_volume/rcc_logical_volume.h"

#include "framework/object_factory.h"

namespace chi_mesh
{

RegisterChiObject(chi_mesh, RCCLogicalVolume);

chi::InputParameters
RCCLogicalVolume::GetInputParameters()
{
  chi::InputParameters params = LogicalVolume::GetInputParameters();

  params.SetDocGroup("LuaLogicVolumes");

  params.AddOptionalParameter("r", 1.0, "Radius of the sphere.");
  params.AddOptionalParameter("x0", 0.0, "X-coordinate of the volume base");
  params.AddOptionalParameter("y0", 0.0, "Y-coordinate of the volume base");
  params.AddOptionalParameter("z0", 0.0, "Z-coordinate of the volume base");
  params.AddOptionalParameter("vx", 0.0, "X-component of the volume extrusion vector");
  params.AddOptionalParameter("vy", 0.0, "Y-component of the volume extrusion vector");
  params.AddOptionalParameter("vz", 1.0, "Z-component of the volume extrusion vector");

  return params;
}

RCCLogicalVolume::RCCLogicalVolume(opensn::App& app, const chi::InputParameters& params)
  : LogicalVolume(app, params),
    r_(params.GetParamValue<double>("r")),
    x0_(params.GetParamValue<double>("x0")),
    y0_(params.GetParamValue<double>("y0")),
    z0_(params.GetParamValue<double>("z0")),
    vx_(params.GetParamValue<double>("vx")),
    vy_(params.GetParamValue<double>("vy")),
    vz_(params.GetParamValue<double>("vz"))
{
}

bool
RCCLogicalVolume::Inside(const chi_mesh::Vector3& point) const
{
  typedef chi_mesh::Vector3 Vec3;

  const auto& pr = point;                // reference point
  const Vec3 p0(x0_, y0_, z0_);          // cylinder root
  const Vec3 cyl_dir_vec(vx_, vy_, vz_); // cylinder direction vector
  const Vec3 k_hat(0.0, 0.0, 1.0);       // k_hat

  const Vec3 p0r = pr - p0;
  const Vec3 cyl_unit_dir = cyl_dir_vec.Normalized(); // aka cud
  const double cyl_length = cyl_dir_vec.Norm();

  // Check if point is within normal extents
  const double p0r_dot_cud = p0r.Dot(cyl_unit_dir);
  if (p0r_dot_cud < 0.0 or p0r_dot_cud > cyl_length) return false;

  // Building rotation matrix
  // This rotation matrix must be such that,
  // when a coordinate system is rotated with it,
  // the new normal vector points along the
  Vec3 binorm;
  Vec3 tangent;
  if (std::abs(cyl_dir_vec.Dot(k_hat) / cyl_dir_vec.Norm()) > (1.0 - 1.0e-12))
  {
    binorm = Vec3(0.0, 1.0, 0.0);
    tangent = Vec3(1.0, 0.0, 0.0);
  }
  else
  {
    binorm = k_hat.Cross(cyl_dir_vec);
    binorm = binorm / binorm.Norm();
    tangent = binorm.Cross(cyl_dir_vec);
    tangent = tangent / tangent.Norm();
  }

  // Project p0r onto the binorm and tangent
  const Vec3 p0r_projected(p0r.Dot(tangent), p0r.Dot(binorm), 0.0);

  // Determine if point is within cylinder
  if (p0r_projected.NormSquare() <= r_ * r_) return true;
  else
    return false;
}

} // namespace chi_mesh
