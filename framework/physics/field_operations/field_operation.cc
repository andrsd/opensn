#include "framework/physics/field_operations/field_operation.h"

namespace chi_physics::field_operations
{

// Since there are no input parameters we will not register this object

chi::InputParameters
FieldOperation::GetInputParameters()
{
  return ChiObject::GetInputParameters();
}

FieldOperation::FieldOperation(opensn::App& app, const chi::InputParameters& params)
  : ChiObject(app, params)
{
}

} // namespace chi_physics::field_operations
