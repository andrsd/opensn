// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "framework/object_factory.h"
#include "framework/parameters/parameter_block.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace opensn
{

ObjectFactory&
ObjectFactory::GetInstance() noexcept
{
  static ObjectFactory singleton;
  return singleton;
}

const std::map<std::string, ObjectFactory::ObjectRegistryEntry>&
ObjectFactory::GetRegistry() const
{
  return object_registry_;
}

bool
ObjectFactory::RegistryHasKey(const std::string& key) const
{
  return object_registry_.count(key) > 0;
}

size_t
ObjectFactory::MakeRegisteredObject(const ParameterBlock& params) const
{
  if (log.GetVerbosity() >= 2)
    log.Log() << "Making object with type from parameters";

  const std::string fname = __PRETTY_FUNCTION__;

  if (not params.Has("obj_type"))
    throw std::invalid_argument(fname + ": Requires a parameter block with a field called "
                                        "\"obj_type\". The given parameter block does not seem to "
                                        "have this parameter.");

  const auto type = params.GetParamValue<std::string>("obj_type");

  return MakeRegisteredObjectOfType(type, params);
}

size_t
ObjectFactory::MakeRegisteredObjectOfType(const std::string& type,
                                          const ParameterBlock& params) const
{
  if (log.GetVerbosity() >= 2)
    log.Log() << "Making object with specified type";

  const std::string fname = __PRETTY_FUNCTION__;

  if (object_registry_.count(type) == 0)
    throw std::logic_error(fname + ": No registered type \"" + type + "\" found.");

  if (log.GetVerbosity() >= 2)
    log.Log() << "Making object type " << type;

  auto object_entry = object_registry_.at(type);

  OpenSnLogicalErrorIf(not object_entry.constructor_func,
                       "Object is not constructable since it has no registered "
                       "constructor");

  auto input_params = object_entry.get_in_params_func();

  input_params.SetObjectType(type);
  input_params.SetErrorOriginScope(type);

  if (log.GetVerbosity() >= 2)
    log.Log() << "Assigning parameters for object " << type;

  input_params.AssignParameters(params);

  if (log.GetVerbosity() >= 2)
    log.Log() << "Constructing object " << type;

  auto new_object = object_entry.constructor_func(input_params);

  new_object->PushOntoStack(new_object);

  if (log.GetVerbosity() >= 2)
    log.Log() << "Done making object type " << type << " with handle " << new_object->GetStackID();

  return new_object->GetStackID();
}

InputParameters
ObjectFactory::GetRegisteredObjectParameters(const std::string& type) const
{
  auto iter = object_registry_.find(type);
  OpenSnInvalidArgumentIf(iter == object_registry_.end(),
                          "Object type \"" + type + "\" is not registered in ObjectFactory.");

  auto& reg_entry = iter->second;

  return reg_entry.get_in_params_func();
}

void
ObjectFactory::AssertRegistryKeyAvailable(const std::string& key,
                                          const std::string& calling_function) const
{
  if (RegistryHasKey(key))
    OpenSnLogicalError(calling_function + ": Attempted to register Object \"" + key +
                       "\" but an object with the same name is already registered.");
}

std::vector<ObjectDescription>
ObjectFactory::GetObjectDescriptions() const
{
  std::vector<ObjectDescription> descrs;
  for (const auto& [key, entry] : object_registry_)
  {
    ObjectDescription d;
    d.name = key;
    auto in_params = entry.get_in_params_func();
    for (std::size_t i = 0; i < in_params.GetNumParameters(); ++i)
    {
      auto& param = in_params.GetParam(i);
      ObjectDescription::Parameter p;
      auto name = param.GetName();
      const auto type = param.GetType();

      p.name = name;
      p.type = param.GetTypeName();
      p.description = in_params.GetParameterDocString(name);
      p.required = in_params.IsParamRequired(name);
      if (type != ParameterBlockType::BLOCK and type != ParameterBlockType::ARRAY)
        p.default_value = param.GetValue().PrintStr();
      const auto& linkage = in_params.GetParameterDocumentationLink(name);
      if (not linkage.empty())
        p.link = linkage;
      d.parameters.push_back(p);
    }
    descrs.push_back(d);
  }

  return descrs;
}

std::string
ToJSON(const std::vector<ObjectDescription>& obj_desc)
{
  json classes = json::array();

  for (auto& desc : obj_desc)
  {
    json obj;
    obj["name"] = desc.name;

    json parameters = json::array();
    for (auto& param : desc.parameters)
    {
      json p;
      p["name"] = param.name;
      p["type"] = param.type;
      p["description"] = param.description;
      p["required"] = param.required;
      if (not param.default_value.empty())
        p["default_value"] = param.default_value;
      if (not param.link.empty())
        p["link"] = param.link;
      parameters.push_back(p);
    }
    obj["parameters"] = parameters;

    classes.push_back(obj);
  }

  return classes.dump();
}

} // namespace opensn
