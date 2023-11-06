#pragma once

#include "framework/materials/chi_material_property.h"
#include "framework/math/Functions/function_dimA_to_dimB.h"

namespace chi
{

/**General material base class for a scalar material property that
 * is possibly a function of position (x,y,z), time t, and maybe a set
 * of variables (v).*/
class MaterialPropertyScalarFuncXYZTV : public MaterialProperty
{
protected:
  const chi_math::FunctionDimAToDimB& function_;
  const std::vector<std::string> dependent_variables_;

public:
  static InputParameters GetInputParameters();
  explicit MaterialPropertyScalarFuncXYZTV(const InputParameters& params);

  double Evaluate(const std::vector<double>& vars);
};

} // namespace chi
