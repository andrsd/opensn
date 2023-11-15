#pragma once

#include "framework/math/functions/function.h"
#include "framework/mesh/mesh_vector.h"

namespace chi_math
{

/**
 * Base class for evaluating functions given spatial location
 *
 */
class ScalarFunction : public Function
{
public:
  static chi::InputParameters GetInputParameters();
  explicit ScalarFunction(opensn::App& app, const chi::InputParameters& params);

  /**
   * Evaluate this function
   *
   * \param xyz The xyz coordinates of the point where the function is called.
   * @return Function value
   */
  virtual double Evaluate(const chi_mesh::Vector3& xyz) const = 0;
};

} // namespace chi_math
