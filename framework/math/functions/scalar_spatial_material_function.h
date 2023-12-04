#pragma once

#include "framework/math/functions/function.h"
#include "framework/mesh/mesh_vector.h"

namespace opensn
{

/**
 * Base class for evaluating functions given material ID and spatial location
 *
 */
class ScalarSpatialMaterialFunction : public Function
{
public:
  static InputParameters GetInputParameters();
  explicit ScalarSpatialMaterialFunction(const InputParameters& params);

  /**
   * Evaluate this function
   *
   * \param mat_id The material ID of the cell
   * \param xyz The xyz coordinates of the point where the function is called.
   * \return Function value
   */
  virtual double Evaluate(int mat_id, const Vector3& xyz) const = 0;
};

} // namespace opensn
