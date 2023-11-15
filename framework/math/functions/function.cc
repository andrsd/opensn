#include "function.h"

namespace chi_math
{

chi::InputParameters
Function::GetInputParameters()
{
  chi::InputParameters params = ChiObject::GetInputParameters();
  return params;
}

Function::Function(opensn::App& app, const chi::InputParameters& params)
  : chi::ChiObject(app, params)
{
}

} // namespace chi_math
