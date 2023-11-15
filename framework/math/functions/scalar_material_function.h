#pragma once

#include "framework/math/functions/function.h"
#include "framework/mesh/mesh_vector.h"

namespace chi_math
{

/**
 * Base class for evaluating functions given material ID and spatial location
 *
 */
class ScalarMaterialFunction : public Function
{
public:
  static chi::InputParameters GetInputParameters();
  explicit ScalarMaterialFunction(opensn::App& app, const chi::InputParameters& params);

  /**
   * Evaluate this function
   *
   * \param mat_id The material ID of the cell
   * \param xyz The xyz coordinates of the point where the function is called.
   * @return Function value
   */
  virtual double Evaluate(int mat_id, const chi_mesh::Vector3& xyz) const = 0;
};

} // namespace chi_math
