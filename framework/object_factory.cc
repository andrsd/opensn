#include "framework/object_factory.h"
#include "framework/app.h"
#include "framework/logging/log.h"
#include "framework/registry.h"

namespace opensn
{

ObjectFactory::ObjectFactory(opensn::App& app) : app_(app)
{
}

size_t
ObjectFactory::MakeRegisteredObject(const chi::ParameterBlock& params) const
{
  if (app_.Log().GetVerbosity() >= 2) app_.Log().Log() << "Making object with type from parameters";

  const std::string fname = __PRETTY_FUNCTION__;

  if (not params.Has("chi_obj_type"))
    throw std::invalid_argument(fname +
                                ": Requires a parameter block with a field called "
                                "\"chi_obj_type\". The given parameter block does not seem to "
                                "have this parameter.");

  const auto type = params.GetParamValue<std::string>("chi_obj_type");

  return MakeRegisteredObjectOfType(type, params);
}

size_t
ObjectFactory::MakeRegisteredObjectOfType(const std::string& type,
                                          const chi::ParameterBlock& params) const
{
  if (app_.Log().GetVerbosity() >= 2) app_.Log().Log() << "Making object with specified type";

  const std::string fname = __PRETTY_FUNCTION__;

  auto registry = opensn::Registry::Instance();
  if (!registry.HasKey(type))
    throw std::logic_error(fname + ": No registered type \"" + type + "\" found.");

  if (app_.Log().GetVerbosity() >= 2) app_.Log().Log() << "Making object type " << type;

  auto object_entry = registry.Find(type);
  ChiLogicalErrorIf(not object_entry.constructor_func,
                    "Object is not constructable since it has no registered constructor");

  auto input_params = object_entry.get_in_params_func();

  input_params.SetObjectType(type);
  input_params.SetErrorOriginScope(type);

  if (app_.Log().GetVerbosity() >= 2)
    app_.Log().Log() << "Assigning parameters for object " << type;

  input_params.AssignParameters(params);

  if (app_.Log().GetVerbosity() >= 2) app_.Log().Log() << "Constructing object " << type;

  auto new_object = object_entry.constructor_func(app_, input_params);

  new_object->PushOntoStack(new_object);

  if (app_.Log().GetVerbosity() >= 2)
    app_.Log().Log() << "Done making object type " << type << " with handle "
                     << new_object->StackID();

  return new_object->StackID();
}

chi::InputParameters
ObjectFactory::GetRegisteredObjectParameters(const std::string& type)
{
  auto registry = opensn::Registry::Instance();
  auto entry = registry.Find(type);
  auto params = entry.get_in_params_func();
  params.SetApp(&app_);
  return params;
}

} // namespace opensn
