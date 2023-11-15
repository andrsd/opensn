#include "framework/math/functions/scalar_material_function.h"

namespace chi_math
{

chi::InputParameters
ScalarMaterialFunction::GetInputParameters()
{
  chi::InputParameters params = Function::GetInputParameters();
  return params;
}

ScalarMaterialFunction::ScalarMaterialFunction(opensn::App& app, const chi::InputParameters& params)
  : Function(app, params)
{
}

} // namespace chi_math
