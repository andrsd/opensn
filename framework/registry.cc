#include "framework/registry.h"
#include "framework/logging/log_exceptions.h"

namespace opensn
{

Registry&
Registry::Instance()
{
  static Registry registry;
  return registry;
}

bool
Registry::HasKey(const std::string& key)
{
  return object_registry_.count(key) > 0;
}

const Registry::ObjectRegistryEntry&
Registry::Find(const std::string& type) const
{
  auto iter = object_registry_.find(type);
  ChiInvalidArgumentIf(iter == object_registry_.end(),
                       "Object type \"" + type + "\" is not registered in ChiObjectFactory.");
  return iter->second;
}

void
Registry::Dump(std::ostream& stream, int verbosity) const
{
  stream << "\n\n";
  for (const auto& [key, entry] : object_registry_)
  {
    if (verbosity == 0)
    {
      stream << key;
      continue;
    }

    stream << "OBJECT_BEGIN " << key;

    if (entry.constructor_func == nullptr) stream << "NOT_CONSTRUCTIBLE";

    const auto in_params = entry.get_in_params_func();
    in_params.DumpParameters();

    stream << "OBJECT_END\n\n";
  }
  stream << "\n\n";
}

void
Registry::AssertRegistryKeyAvailable(const std::string& key, const std::string& calling_function)
{
  if (HasKey(key))
    ChiLogicalError(calling_function + ": Attempted to register Object \"" + key +
                    "\" but an object with the same name is already registered.");
}

} // namespace opensn
