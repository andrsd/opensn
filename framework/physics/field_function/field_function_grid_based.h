#pragma once

#include "framework/physics/field_function/field_function.h"

#include "framework/mesh/mesh.h"

#include "framework/math/parallel_vector/ghosted_parallel_stl_vector.h"
#include "framework/math/petsc_utils/petsc_forward_declarations.h"

#include <string>
#include <memory>
#include <vector>
#include <utility>

namespace chi_math
{
class SpatialDiscretization;
typedef std::shared_ptr<SpatialDiscretization> SDMPtr;
class GhostedParallelSTLVector;
} // namespace chi_math

namespace chi_physics
{

/***/
class FieldFunctionGridBased : public FieldFunction
{
public:
  typedef std::pair<chi_mesh::Vector3, chi_mesh::Vector3> BoundingBox;

  /**Returns required input parameters.*/
  static chi::InputParameters GetInputParameters();

  /**ObjectMaker based constructor.*/
  explicit FieldFunctionGridBased(opensn::App& app, const chi::InputParameters& params);

  /**Creates a field function, filling it with zeros.*/
  FieldFunctionGridBased(opensn::App& app,
                         const std::string& text_name,
                         chi_math::SDMPtr& discretization_ptr,
                         chi_math::Unknown unknown);

  /**Creates a field function with an associated field vector.
   * The field's data vector is set to the incoming field vector.*/
  FieldFunctionGridBased(opensn::App& app,
                         const std::string& text_name,
                         chi_math::SDMPtr& sdm_ptr,
                         chi_math::Unknown unknown,
                         const std::vector<double>& field_vector);

  /**Creates a field function where all the values are assigned to
   * the single supplied value.*/
  FieldFunctionGridBased(opensn::App& app,
                         const std::string& text_name,
                         chi_math::SDMPtr& sdm_ptr,
                         chi_math::Unknown unknown,
                         double field_value);

  virtual ~FieldFunctionGridBased() = default;

  /**
   * Returns the spatial discretization method.
   */
  const chi_math::SpatialDiscretization& GetSpatialDiscretization() const;

  /**
   * Returns a read-only reference to the locally stored field data.
   */
  const std::vector<double>& FieldVectorRead() const;

  /**
   * Returns a reference to the locally stored field data.
   */
  std::vector<double>& FieldVector();

  /**
   * Updates the field vector with a local STL vector.
   */
  void UpdateFieldVector(const std::vector<double>& field_vector);

  /**
   * Updates the field vector with a PETSc vector. This only operates locally.
   */
  void UpdateFieldVector(const Vec& field_vector);

  /**
   * Static method to export multiple grid-based field functions.
   */
  typedef std::vector<std::shared_ptr<const FieldFunctionGridBased>> FFList;

  /**
   * Export multiple field functions to VTK.
   */
  static void
  ExportMultipleToVTK(opensn::App& app, const std::string& file_base_name, const FFList& ff_list);

  /**
   * Makes a copy of the locally stored data with ghost access.
   */
  std::vector<double> GetGhostedFieldVector() const;

  /**
   * Returns the component values at requested point.
   */
  virtual std::vector<double> GetPointValue(const chi_mesh::Vector3& point) const;

  /**Evaluates the field function, on a cell, at the specified point.*/
  double Evaluate(const chi_mesh::Cell& cell,
                  const chi_mesh::Vector3& position,
                  unsigned int component) const override;

protected:
  chi_math::SDMPtr sdm_;
  std::unique_ptr<chi_math::GhostedParallelSTLVector> ghosted_field_vector_;

private:
  /**
   * Static method for making the GetSpatialDiscretization for the constructors.
   */
  static chi_math::SDMPtr MakeSpatialDiscretization(opensn::App& app,
                                                    const chi::InputParameters& params);

  /**
   * Static method for making the ghosted vector for the constructors.
   */
  static std::unique_ptr<chi_math::GhostedParallelSTLVector>
  MakeFieldVector(const chi_math::SpatialDiscretization& discretization,
                  const chi_math::UnknownManager& uk_man);

  const BoundingBox local_grid_bounding_box_;
};

} // namespace chi_physics
