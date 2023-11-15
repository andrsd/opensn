#include "framework/math/functions/scalar_function.h"

namespace chi_math
{

chi::InputParameters
ScalarFunction::GetInputParameters()
{
  chi::InputParameters params = Function::GetInputParameters();
  return params;
}

ScalarFunction::ScalarFunction(opensn::App& app, const chi::InputParameters& params)
  : Function(app, params)
{
}

} // namespace chi_math
