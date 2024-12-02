// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "framework/field_functions/field_function.h"
#include "framework/logging/log_exceptions.h"

namespace opensn
{

InputParameters
FieldFunction::GetInputParameters()
{
  InputParameters params = Object::GetInputParameters();

  params.AddRequiredParameter<std::string>("name", "The field function name.");
  params.AddOptionalParameter(
    "unknown_type", "Scalar", "The type of the variable for this field function");
  params.AddOptionalParameter("num_components",
                              1,
                              "The number of components to attach to the variable. "
                              "This is only valid for VectorN unknowns.");

  // Constrain values
  params.ConstrainParameterRange(
    "unknown_type", AllowableRangeList::New({"Scalar", "Vector2", "Vector3", "VectorN"}));
  params.ConstrainParameterRange("num_components", AllowableRangeLowLimit::New(1));

  return params;
}

FieldFunction::FieldFunction(const InputParameters& params)
  : Object(params),
    name_(params.GetParamValue<std::string>("name")),
    unknown_(
      (params.GetParamValue<std::string>("unknown_type") == "Scalar") ? Unknown(UnknownType::SCALAR)
      : (params.GetParamValue<std::string>("unknown_type") == "Vector2")
        ? Unknown(UnknownType::VECTOR_2)
      : (params.GetParamValue<std::string>("unknown_type") == "Vector3")
        ? Unknown(UnknownType::VECTOR_2)
      : (params.GetParamValue<std::string>("unknown_type") == "VectorN")
        ? Unknown(UnknownType::VECTOR_N, params.GetParamValue<unsigned int>("num_components"))
        : Unknown(UnknownType::SCALAR)),
    unknown_manager_({unknown_})
{
}

FieldFunction::FieldFunction(std::string name, Unknown unknown)
  : name_(std::move(name)), unknown_(std::move(unknown)), unknown_manager_({unknown_})
{
}

void
FieldFunction::PushOntoStack(std::shared_ptr<Object> new_object)
{
  auto ff_ptr = std::dynamic_pointer_cast<FieldFunction>(new_object);

  OpenSnLogicalErrorIf(not ff_ptr, "Bad trouble when casting object to field function");

  field_function_stack.push_back(ff_ptr);
  new_object->SetStackID(field_function_stack.size() - 1);
}

} // namespace opensn
