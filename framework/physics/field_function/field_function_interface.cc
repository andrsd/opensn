#include "framework/physics/field_function/field_function_interface.h"
#include "framework/physics/field_function/field_function.h"
#include "framework/app.h"
#include "framework/logging/log.h"

namespace chi_physics
{

chi::InputParameters
FieldFunctionInterface::GetInputParameters()
{
  chi::InputParameters params;

  params.AddRequiredParameterBlock("field_function", "Field function handle or name.");
  params.SetParameterTypeMismatchAllowed("field_function");

  return params;
}

FieldFunctionInterface::FieldFunctionInterface(opensn::App& app, const chi::InputParameters& params)
  : app_(app), field_function_param_(params.GetParam("field_function"))
{
}

std::shared_ptr<chi_physics::FieldFunction>
FieldFunctionInterface::GetFieldFunction() const
{
  std::shared_ptr<chi_physics::FieldFunction> ref_ff_ptr = nullptr;
  if (field_function_param_.Type() == chi::ParameterBlockType::STRING)
  {
    const auto name = field_function_param_.GetValue<std::string>();
    for (const auto& ff_ptr : App().FieldFunctionStack())
      if (ff_ptr->TextName() == name) ref_ff_ptr = ff_ptr;

    ChiInvalidArgumentIf(ref_ff_ptr == nullptr, "Field function \"" + name + "\" not found.");
  }
  else if (field_function_param_.Type() == chi::ParameterBlockType::INTEGER)
  {
    const auto handle = field_function_param_.GetValue<size_t>();
    ref_ff_ptr = App().GetFieldFunction(handle, __FUNCTION__);
  }
  else
    ChiInvalidArgument("Argument can only be STRING or INTEGER");

  return ref_ff_ptr;
}

} // namespace chi_physics
