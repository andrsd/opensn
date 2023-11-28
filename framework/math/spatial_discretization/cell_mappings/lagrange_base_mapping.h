#pragma once

#include "framework/math/spatial_discretization/cell_mappings/cell_mapping.h"
#include "framework/math/serial_newton_iteration/serial_newton_iteration.h"

#include "framework/mesh/mesh.h"

namespace opensn
{
class Quadrature;

/**Base class for finite elements using Lagrange basis functions.
 *\ingroup doc_CellMappings*/
class LagrangeBaseMapping : public CellMapping
{
public:
  double ShapeValue(int i, const Vector3& xyz) const override;
  void ShapeValues(const Vector3& xyz, std::vector<double>& shape_values) const override;
  Vector3 GradShapeValue(int i, const Vector3& xyz) const override;
  void GradShapeValues(const Vector3& xyz, std::vector<Vector3>& gradshape_values) const override;

  VolumetricQuadraturePointData MakeVolumetricQuadraturePointData() const override;

  SurfaceQuadraturePointData MakeSurfaceQuadraturePointData(size_t face_index) const override;

protected:
  friend class WorldXYZToNaturalMappingHelper;
  typedef Vector3 Vec3;
  typedef std::vector<double> VecDbl;
  typedef std::vector<VecDbl> MatDbl;

  LagrangeBaseMapping(const MeshContinuum& grid,
                      const Cell& cell,
                      size_t num_nodes,
                      std::vector<std::vector<int>> face_node_mappings,
                      const Quadrature& volume_quadrature,
                      const Quadrature& surface_quadrature);

  /**Function using newton iteration to convert a world_xyz coordinate to
   * a quadrature point coordinate.*/
  Vec3 MapWorldXYZToNaturalXYZ(const Vec3& world_xyz) const;

  /**Used in the constructor this method simply collects the cell vertices.*/
  static std::vector<Vector3> GetVertexLocations(const MeshContinuum& grid, const Cell& cell);

  /** This section just determines a mapping of face dofs
   * to cell dofs. This is pretty simple since we can
   * just loop over each face dof then subsequently
   * loop over cell dofs, if the face dof node index equals
   * the cell dof node index then the mapping is assigned.
   *
   * This mapping is not used by any of the methods in
   *     this class but is used by methods requiring the
   *       surface integrals of the shape functions.
   */
  static std::vector<std::vector<int>> MakeFaceNodeMapping(const Cell& cell);

  /**Reference element shape function evaluation. Meant to be overridden by
   * a specific element mapping.*/
  virtual double RefShape(uint32_t i, const Vec3& qpoint) const = 0;

  /**Reference element shape function gradient evaluation. Meant to be
   * overridden by a specific element mapping.*/
  virtual Vec3 RefGradShape(uint32_t i, const Vec3& qpoint) const = 0;

  /**Reference element Jacobian evaluation. Meant to be overridden by
   * a specific element mapping.*/
  virtual MatDbl RefJacobian(const Vec3& qpoint) const = 0;

  /**Computes the determinant of the surface Jacobian and the surface normal
   * at the face quadrature point.*/
  virtual std::pair<double, Vec3>
  RefFaceJacobianDeterminantAndNormal(size_t face_index, const Vec3& qpoint_face) const;

  /**Converts a face quadrature point to a reference element position allowing
   * the reuse of the reference element shape functions. Meant to be overridden
   * by a specific element mapping.*/
  virtual Vec3 FaceToElementQPointConversion(size_t face_index, const Vec3& qpoint_face) const = 0;

  /**Overrideable method to return the appropriate face quadrature. Default
   * returns surface_quadrature. Some mappings might change quadrature based
   * on the face index.*/
  virtual const Quadrature& GetSurfaceQuadrature(size_t face_index) const;

  const Quadrature& volume_quadrature_;
  const Quadrature& surface_quadrature_;
};

/**Helper class to apply Newton's method to map world coordinates to
 * reference element coordinates.*/
class WorldXYZToNaturalMappingHelper : public NonLinearFunction
{
public:
  typedef Vector3 Vec3;
  WorldXYZToNaturalMappingHelper(const LagrangeBaseMapping& cell_mapping, const Vec3& world_x);

  VecDbl F(const VecDbl& x) const override;
  MatDbl J(const VecDbl& x) const override;

private:
  size_t dimension_;
  const LagrangeBaseMapping& cell_mapping_;
  const Vec3 world_x_;
};

} // namespace opensn
