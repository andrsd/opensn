#include "framework/physics/field_function/grid_based_field_function_interface.h"
#include "framework/physics/field_function/field_function_grid_based.h"

namespace chi_physics
{

chi::InputParameters
GridBasedFieldFunctionInterface::GetInputParameters()
{
  chi::InputParameters params = FieldFunctionInterface::GetInputParameters();

  return params;
}

GridBasedFieldFunctionInterface::GridBasedFieldFunctionInterface(opensn::App& app,
                                                                 const chi::InputParameters& params)
  : FieldFunctionInterface(app, params)
{
}

std::shared_ptr<FieldFunctionGridBased>
GridBasedFieldFunctionInterface::GetGridBasedFieldFunction() const
{
  std::shared_ptr<FieldFunction> ff_ptr = GetFieldFunction();
  return std::dynamic_pointer_cast<FieldFunctionGridBased>(ff_ptr);
}

} // namespace chi_physics
