#include "function.h"

namespace chi_math
{

chi::InputParameters
Function::GetInputParameters()
{
  chi::InputParameters params = ChiObject::GetInputParameters();
  return params;
}

Function::Function(const chi::InputParameters& params) : chi::ChiObject(params)
{
}

} // namespace chi_math
