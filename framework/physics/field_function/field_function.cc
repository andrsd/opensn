#include "framework/physics/field_function/field_function.h"
#include "framework/logging/log_exceptions.h"
#include "framework/app.h"

namespace chi_physics
{

chi::InputParameters
FieldFunction::GetInputParameters()
{
  chi::InputParameters params = ChiObject::GetInputParameters();

  params.AddRequiredParameter<std::string>("name",
                                           "Named to be associated with this field function");

  params.AddOptionalParameter(
    "unknown_type", "Scalar", "The type of the variable for this field function");

  params.AddOptionalParameter("num_components",
                              1,
                              "The number of components to attach to the variable. "
                              "Only effective when \"type\" is VectorN.");

  // Constrain values
  using namespace chi_data_types;
  params.ConstrainParameterRange(
    "unknown_type", AllowableRangeList::New({"Scalar", "Vector2", "Vector3", "VectorN"}));

  params.ConstrainParameterRange("num_components", AllowableRangeLowLimit::New(1));

  return params;
}

FieldFunction::FieldFunction(opensn::App& app, const chi::InputParameters& params)
  : ChiObject(app, params),
    text_name_(params.GetParamValue<std::string>("name")),
    unknown_((params.GetParamValue<std::string>("unknown_type") == "Scalar")
               ? chi_math::Unknown(chi_math::UnknownType::SCALAR)
             : (params.GetParamValue<std::string>("unknown_type") == "Vector2")
               ? chi_math::Unknown(chi_math::UnknownType::VECTOR_2)
             : (params.GetParamValue<std::string>("unknown_type") == "Vector3")
               ? chi_math::Unknown(chi_math::UnknownType::VECTOR_2)
             : (params.GetParamValue<std::string>("unknown_type") == "VectorN")
               ? chi_math::Unknown(chi_math::UnknownType::VECTOR_N,
                                   params.GetParamValue<unsigned int>("num_components"))
               : chi_math::Unknown(chi_math::UnknownType::SCALAR)),
    unknown_manager_(app, {unknown_})
{
}

FieldFunction::FieldFunction(opensn::App& app,
                             const std::string& text_name,
                             chi_math::Unknown unknown)
  : ChiObject(app),
    text_name_(text_name),
    unknown_(std::move(unknown)),
    unknown_manager_(app, {unknown_})
{
}

void
FieldFunction::PushOntoStack(std::shared_ptr<ChiObject>& new_object)
{
  auto ff_ptr = std::dynamic_pointer_cast<FieldFunction>(new_object);

  ChiLogicalErrorIf(not ff_ptr, "Bad trouble when casting object to field function");

  App().FieldFunctionStack().push_back(ff_ptr);
  new_object->SetStackID(App().FieldFunctionStack().size() - 1);
}

} // namespace chi_physics
